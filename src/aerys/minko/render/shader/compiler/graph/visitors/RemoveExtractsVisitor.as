package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.graph.nodes.ANode;
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
		
		override protected function start() : void
		{
			var extract : Extract;
			
			while (_shaderGraph.position is Extract)
			{
				extract = Extract(_shaderGraph.position);
				
				_shaderGraph.position = extract.argument;
				_shaderGraph.positionComponents = 
					Components.applyCombination(_shaderGraph.positionComponents, extract.component);
			}
			
			while (_shaderGraph.color is Extract)
			{
				extract = Extract(_shaderGraph.color);
				
				_shaderGraph.color = extract.argument;
				_shaderGraph.colorComponents = 
					Components.applyCombination(_shaderGraph.colorComponents, extract.component);
			}
			
			var numKills : uint = _shaderGraph.kills.length;
			for (var killId : uint = 0; killId < numKills; ++killId)
			{
				extract = Extract(_shaderGraph.color);
				
				_shaderGraph.kills[killId] = extract.argument;
				_shaderGraph.killComponents[killId] = 
					Components.applyCombination(_shaderGraph.killComponents[killId], extract.component);
			}
		}
		
		override protected function finish() : void
		{
		}
		
		override protected function visitTraversable(node:ANode, isVertexShader:Boolean):void
		{
			var numArguments : uint = node.numArguments;
			
			for (var argumentId : uint = 0; argumentId < numArguments; ++argumentId)
			{
				while (node.getArgumentAt(argumentId) is Extract)
				{
					var extract : Extract = Extract(node.getArgumentAt(argumentId));
					
					// remove extract
					node.setArgumentAt(argumentId, extract.argument);
					
					// change components
					node.setComponentAt(
						argumentId, 
						Components.applyCombination(extract.component, node.getComponentAt(argumentId))
					);
				}
				
				visit(node.getArgumentAt(argumentId), true);
			}
		}
		
		override protected function visitNonTraversable(node:ANode, isVertexShader:Boolean):void
		{
		}
	}
}
