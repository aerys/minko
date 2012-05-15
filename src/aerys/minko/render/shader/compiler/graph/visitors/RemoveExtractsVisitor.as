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

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class RemoveExtractsVisitor extends AbstractVisitor
	{
		public function RemoveExtractsVisitor() 
		{
		}
		
		override protected function visitTraversable(node : AbstractNode, isVertexShader : Boolean):void
		{
			visitArguments(node, isVertexShader);
			
			if (node is Extract)
				replaceInParentsAndSwizzle(node, Extract(node).argument, Extract(node).component);
		}
		
		override protected function visitNonTraversable(node : AbstractNode, isVertexShader : Boolean):void
		{
		}
	}
}
