package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Attribute;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableConstant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableSampler;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Sampler;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Extract;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Interpolate;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.VariadicExtract;
	import aerys.minko.render.shader.compiler.register.Components;
	
	import flash.utils.Dictionary;

	/**
	 * @private
	 * @author Romain Gilliotte
	 */
	public class OverwriterCleanerVisitor extends AbstractVisitor
	{
		private static const TYPE_PRIORITY : Dictionary = new Dictionary();
		{
			TYPE_PRIORITY[Constant]			= 0;
			TYPE_PRIORITY[BindableConstant]	= 1;
			TYPE_PRIORITY[Attribute]		= 2;
			TYPE_PRIORITY[Interpolate]		= 3;
			TYPE_PRIORITY[Instruction]		= 4;
		}
		
		public function OverwriterCleanerVisitor()
		{
		}
		
		override protected function visitTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
			visitArguments(node, true);
			
			if (node is Overwriter)
				visitOverwriter(Overwriter(node), true);
		}
		
		override protected function visitNonTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
		}
		
		override protected function visitOverwriter(overwriter		: Overwriter, 
													isVertexShader	: Boolean) : void
		{
			var numArgs		: uint = overwriter.numArguments;
			var argId		: int; // we use int instead of uint to be able to loop backwards
			var arg			: AbstractNode;
			var component	: uint;
			
			// visit sons
			for (argId = 0; argId < numArgs; ++argId)
				visit(overwriter.getArgumentAt(argId), isVertexShader);
			
			// expand other overwriters inside this one
			// also, replace non component wise operation by moves.
			for (argId = 0; argId < numArgs; ++argId)
			{
				arg			= overwriter.getArgumentAt(argId);
				component	= overwriter.getComponentAt(argId);
				
				if (arg is Overwriter)
				{
					// remove the argument
					overwriter.removeArgumentAt(argId);
					--numArgs;
					
					// inject modified arguments into the current overwriter
					var innerNumArgs 	: uint = Overwriter(arg).numArguments;
					
					for (var innerArgId : uint = 0; innerArgId < innerNumArgs; ++innerArgId)
					{
						// modify argument
						var innerArg		: AbstractNode	= Overwriter(arg).getArgumentAt(innerArgId);
						var innerComponent	: uint	= Overwriter(arg).getComponentAt(innerArgId);
						
						innerComponent = Components.applyCombination(innerComponent, component);
						
						// inject it
						overwriter.addArgumentAt(argId, innerArg, innerComponent);
						
						// we already know that the arguments we are injecting are not Overwriters 
						// and are component wise (becase we visited each argument first)
						// ==> its OK to skip looping over them.
//						++argId;
						
						// numArgs increase
						++numArgs;
					}
				}
			}
			
			if (numArgs > 1)
			{
				// mask components in reverse order so that no overwriting occurs 
				// (which would be OK btw, but this is going to help us detect useless nodes).
				for (argId = numArgs - 2; argId >= 0; --argId)
					overwriter.setComponentAt(
						argId, 
						Components.applyMask(
							overwriter.getComponentAt(argId),
							overwriter.getComponentAt(argId + 1)
						)
					);
				
				// remove any node if its component value is empty: 
				// there is no need to compute it if we are going to overwrite it
				for (argId = numArgs - 1; argId >= 0; --argId)
					if (Components.isEmpty(overwriter.getComponentAt(argId)))
					{
						overwriter.removeArgumentAt(argId);
						--numArgs;
					}
			}
			
			// Sort arguments by type, so that the ResolveConstantComputationVisitor 
			// and ResolveParametrizedComputation can work easily.
			// (The argument order does not matter anymore because we masked each component with the following one). 
			for (argId = 0; argId < numArgs - 1; ++argId)
			{
				var argument1	: AbstractNode	= overwriter.getArgumentAt(argId);
				var argument2	: AbstractNode	= overwriter.getArgumentAt(argId + 1);
				var component1	: uint			= overwriter.getComponentAt(argId);
				var component2	: uint			= overwriter.getComponentAt(argId + 1);
				
				if (TYPE_PRIORITY[Object(argument1).constructor] > TYPE_PRIORITY[Object(argument2).constructor])
				{
					overwriter.setArgumentAt(argId, argument2);
					overwriter.setArgumentAt(argId + 1, argument1);
					
					overwriter.setComponentAt(argId, component2);
					overwriter.setComponentAt(argId + 1, component1);
					
					if (argId != 0)
						argId -= 2;
				}
			}
			
			// remove the whole node if we reduced numArgs to 1
			if (numArgs == 1)
				replaceInParentsAndSwizzle(overwriter, overwriter.getArgumentAt(0), overwriter.getComponentAt(0));
		}
	}
}
