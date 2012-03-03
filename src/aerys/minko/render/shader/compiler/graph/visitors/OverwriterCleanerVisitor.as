package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
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
	 * 
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
			super(true);
		}
		
		override protected function start() : void
		{
		}
		
		override protected function finish() : void
		{
		}
		
		override protected function visitInstruction(instruction	: Instruction, 
													 isVertexShader	: Boolean) : void
		{
			visit(instruction.arg1, isVertexShader);
			
			if (!instruction.isSingle)
				visit(instruction.arg2, isVertexShader);
		}
		
		override protected function visitInterpolate(interpolate	: Interpolate, 
													 isVertexShader	: Boolean) : void
		{
			visit(interpolate.arg, true);
		}
		
		override protected function visitVariadicExtract(variadicExtract : VariadicExtract, 
														 isVertexShader	 : Boolean) : void
		{
			if (!isVertexShader)
				throw new Error('VariadicExtract can only be found on vertex shader.');
			
			visit(variadicExtract.index, true);
			visit(variadicExtract.constant, true);
		}
		
		override protected function visitOverwriter(overwriter		: Overwriter, 
													isVertexShader	: Boolean) : void
		{
			var args		: Vector.<INode>	= overwriter.args;
			var components	: Vector.<uint>		= overwriter.components;
			var numArgs		: uint				= args.length;
			
			var argId		: int; // we use int instead of uint to be able to loop backwards
			var arg			: INode;
			var component	: uint;
			
			// visit sons
			for (argId = 0; argId < numArgs; ++argId)
				visit(args[argId], isVertexShader);
			
			// expand other overwriters inside this one
			// also, replace non component wise operation by moves.
			for (argId = 0; argId < numArgs; ++argId)
			{
				arg = args[argId];
				component = components[argId];
				
				if (arg is Overwriter)
				{
					// remove the argument
					args.splice(argId, 1);
					components.splice(argId, 1);
					--numArgs;
					
					// inject modified arguments into the current overwriter
					var innerArgs	 	: Vector.<INode>	= Overwriter(arg).args;
					var innerNumArgs 	: uint				= innerArgs.length;
					var innerComponents	: Vector.<uint>		= Overwriter(arg).components;
					
					for (var innerArgId : uint = 0; innerArgId < innerNumArgs; ++innerArgId)
					{
						// modify argument
						var innerArg		: INode	= innerArgs[innerArgId];
						var innerComponent	: uint	= Components.applyCombination(innerComponents[innerArgId], component);
						
						// inject it
						components.splice(argId, 0, innerComponent);
						args.splice(argId, 0, innerArg);
						
						// we already know that the arguments we are injecting are not Overwriters 
						// and are component wise (becase we visited each argument first)
						// ==> its OK to skip looping over them.
//						++argId;
						
						// numArgs increase
						++numArgs;
					}
				}
				else if (arg is Instruction && !Instruction(arg).isComponentWise)
				{
					// wrap the argument into a mov instruction to make it component wise.
					args[argId] = new Instruction(Instruction.MOV, args[argId]);
				}
			}
			
			if (numArgs > 1)
			{
				// mask components in reverse order so that no overwriting occurs 
				// (which would be OK btw, but this is going to help us detect useless nodes).
				for (argId = numArgs - 2; argId >= 0; --argId)
					components[argId] = Components.applyMask(components[argId], components[argId + 1]);
				
				// remove any node if its component value is empty: 
				// there is no need to compute it if we are going to overwrite it
				for (argId = numArgs - 1; argId >= 0; --argId)
					if (Components.isEmpty(components[argId]))
					{
						components.splice(argId, 1);
						args.splice(argId, 1);
						--numArgs;
					}
			}
			
			// Sort arguments by type, so that the ResolveConstantComputationVisitor 
			// and ResolveParametrizedComputation can work easily.
			// (The argument order does not matter anymore because we masked each component with the following one). 
			for (argId = 0; argId < numArgs - 1; ++argId)
				if (TYPE_PRIORITY[Object(args[argId]).constructor] > TYPE_PRIORITY[Object(args[argId + 1]).constructor])
				{
					arg						= args[argId];
					args[argId]				= args[argId + 1];
					args[argId + 1]			= arg;
					
					component				= components[argId];
					components[argId]		= components[argId + 1];
					components[argId + 1]	= component;
					
					if (argId != 0)
						argId -= 2;
				}
			
			// remove the whole node if we reduced numArgs to 1
			if (numArgs == 1)
				replaceInParentAndSwizzle(overwriter, args[0], components[0]);
			
			// tell the overwriter it has been updated
			overwriter.invalidateHashAndSize();
		}
		
		override protected function visitAttribute(attribute : Attribute, isVertexShader : Boolean) : void
		{
		}
		
		override protected function visitExtract(extract : Extract, isVertexShader : Boolean):void
		{
		}
		
		override protected function visitConstant(constant : Constant, isVertexShader : Boolean):void
		{
		}
		
		override protected function visitBindableConstant(bindableConstant : BindableConstant, isVertexShader : Boolean) : void
		{
		}
		
		override protected function visitSampler(sampler : Sampler, isVertexShader : Boolean) : void
		{
		}
		
		override protected function visitBindableSampler(bindableSampler : BindableSampler, isVertexShader : Boolean) : void
		{
		}
	}
}
