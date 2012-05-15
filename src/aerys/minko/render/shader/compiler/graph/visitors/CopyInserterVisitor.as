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

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class CopyInserterVisitor extends AbstractVisitor
	{
		public function CopyInserterVisitor()
		{
		}
		
		override protected function visitTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
			visitArguments(node, true);
			
			if (node is Instruction)
			{
				var instruction : Instruction = Instruction(node);
				
				if (isConstant(instruction.argument1) 
					&& (instruction.isSingle || isConstant(instruction.argument2)))
					instruction.argument1 = new Instruction(Instruction.MOV, instruction.argument1);
			}
		}
		
		override protected function visitNonTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
		}
		
		
		private function isConstant(node : AbstractNode) : Boolean
		{
			return node is Constant || node is BindableConstant || node is VariadicExtract;
		}
	}
}