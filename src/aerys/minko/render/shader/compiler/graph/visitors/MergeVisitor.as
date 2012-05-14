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
	
	import flash.utils.Dictionary;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class MergeVisitor extends AbstractVisitor
	{
		private var _hashsToNodes	: Dictionary;
		
		public function MergeVisitor()
		{
		}
		
		override protected function start() : void
		{
			super.start();
			
			_hashsToNodes = new Dictionary();
		}
		
		override protected function finish() : void
		{
			_hashsToNodes = null;
			
			super.finish();
		}
		
		override protected function visitTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
			var numArgs	: uint = node.numArguments;
			
			for (var argId : uint = 0; argId < numArgs; ++argId)
			{
				var argument	 : AbstractNode	= node.getArgumentAt(argId);
				var argumentHash : uint			= argument.hash;
				
				if (_hashsToNodes[argumentHash] != undefined)
					node.setArgumentAt(argId, _hashsToNodes[argumentHash]);
				else
				{
					_hashsToNodes[argumentHash] = argument;
					visit(argument, true);
				}
			}
		}
		
		override protected function visitNonTraversable(node:AbstractNode, isVertexShader:Boolean):void
		{
		}
	}
}