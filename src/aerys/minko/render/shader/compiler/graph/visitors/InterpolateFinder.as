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
	public class InterpolateFinder extends AbstractVisitor
	{
		public function InterpolateFinder()
		{
		}
		
		override protected function visitTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
			if (node is Interpolate)
				_shaderGraph.interpolates.push(node);
			else
				visitArguments(node, isVertexShader);
		}
		
		override protected function visitNonTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
		}
	}
}