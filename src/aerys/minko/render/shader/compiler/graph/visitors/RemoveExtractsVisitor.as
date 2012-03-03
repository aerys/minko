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

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class RemoveExtractsVisitor extends AbstractVisitor
	{
		public function RemoveExtractsVisitor() 
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
			var extract : Extract;
			
			while (instruction.arg1 is Extract)
			{
				extract = Extract(instruction.arg1);
				
				instruction.arg1 = extract.child;
				instruction.arg1Components = Components.applyCombination(extract.components, instruction.arg1Components);
			}
			visit(instruction.arg1, isVertexShader);
			
			if (!instruction.isSingle)
			{
				while (instruction.arg2 is Extract)
				{
					extract = Extract(instruction.arg2);
					
					instruction.arg2 = extract.child;
					instruction.arg2Components = Components.applyCombination(extract.components, instruction.arg2Components);
				}
				visit(instruction.arg2, isVertexShader);
			}
		}
		
		override protected function visitInterpolate(interpolate	: Interpolate, 
													 isVertexShader	: Boolean) : void
		{
			while (interpolate.arg is Extract)
			{
				var extract : Extract = Extract(interpolate.arg);
				
				interpolate.arg = extract.child;
				interpolate.components = Components.applyCombination(extract.components, interpolate.components);
			}
			
			visit(interpolate.arg, true);
		}
		
		override protected function visitOverwriter(overwriter		: Overwriter, 
													isVertexShader	: Boolean) : void
		{
			var args		: Vector.<INode>	= overwriter.args;
			var components	: Vector.<uint>		= overwriter.components;
			var numArgs		: uint				= args.length;
			
			for (var argId : uint = 0; argId < numArgs; ++argId)
			{
				while (args[argId] is Extract)
				{
					var extract : Extract = Extract(args[argId]);
					
					args[argId] = extract.child;
					components[argId] = Components.applyCombination(extract.components, components[argId]);
				}
				
				visit(args[argId], isVertexShader);
			}
		}
		
		override protected function visitVariadicExtract(variadicExtract : VariadicExtract, 
														 isVertexShader	 : Boolean) : void
		{
			if (!isVertexShader)
				throw new Error('VariadicExtract are only allowed in the vertex shader.');
			
			while (variadicExtract.index is Extract)
			{
				var extract : Extract = Extract(variadicExtract.index);
				
				variadicExtract.index = extract.child;
				variadicExtract.indexComponentSelect = Components.getReadAtIndex(
					variadicExtract.indexComponentSelect,
					extract.components
				);
			}
			
			visit(variadicExtract.index, isVertexShader);
			
			if (variadicExtract.constant is Extract)
				throw new Error("Cannot use indirect adressing on a swizzled constant");
			
			visit(variadicExtract.constant, isVertexShader);
		}
		
		override protected function visitExtract(extract		: Extract, 
												 isVertexShader	: Boolean) : void
		{
			throw new Error('This should never be reached');
		}
		
		override protected function visitAttribute(attribute		: Attribute, 
												   isVertexShader	: Boolean) : void
		{
		}
		
		override protected function visitConstant(constant			: Constant,
												  isVertexShader	: Boolean) : void
		{
		}
		
		override protected function visitBindableConstant(parameter			: BindableConstant,
														  isVertexShader	: Boolean) : void
		{
		}
		
		override protected function visitSampler(sampler		: Sampler,
												 isVertexShader	: Boolean) : void
		{
		}
		
		override protected function visitBindableSampler(bindableSampler	: BindableSampler,
														 isVertexShader		: Boolean) : void
		{
		}
	}
}
