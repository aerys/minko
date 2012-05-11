package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.graph.ShaderGraph;
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
	 * 
	 */
	public class WriteDot extends AbstractVisitor
	{
		private var _nodeIds	: Dictionary;
		private var _nodes		: String;
		private var _links		: String;
		private var _result		: String;
		
		private var _nodeId		: uint;
		
		public function get result() : String
		{
			return _result;
		}
		
		public function WriteDot()
		{
		}
		
		override public function process(shaderGraph : ShaderGraph) : void
		{
			_shaderGraph = shaderGraph;
			
			var outputPosition	: Object = { id: 'outputPosition' };
			var outputColor		: Object = { id: 'outputColor' };
			var kills			: Object = { id: 'kills' };
			
			start();
			
			appendNode(outputPosition, 'orange', 'Output Position');
			appendNode(outputColor, 'orange', 'Output Color');
			
			if (_shaderGraph.kills.length != 0)
				appendNode(kills, 'orange', 'Kills');
			
			visit(_shaderGraph.position, true);
			appendLink(outputPosition, _shaderGraph.position);
			
			visit(_shaderGraph.color, false);
			appendLink(outputColor, _shaderGraph.color);
			
			for each (var kill : AbstractNode in _shaderGraph.kills)
			{
				visit(kill, false);
				appendLink(kills, kill);
			}
				
			finish();
		}
		
		override protected function start() : void
		{
			super.start();
			
			_nodes		= "";
			_links		= "";
			_nodeIds	= new Dictionary();
		}
		
		override protected function finish() : void
		{
			_result = "digraph shader {\n" +
				"\tnode [ style = filled ];\n" + 
				_nodes + _links + 
			"}";
			
			_nodeIds	= null;
			_nodeId		= 0;
			
			super.finish();
		}
		
		public function clear() : void
		{
			_result = null;
		}
		
		override protected function visitInstruction(instruction	: Instruction, 
													 isVertexShader	: Boolean) : void
		{
			appendNode(instruction, 'cadetblue1', instruction.name);
			
			visit(instruction.argument1, isVertexShader);
			appendLink(instruction, instruction.argument1, instruction.component1);
			
			if (!instruction.isSingle)
			{
				visit(instruction.argument2, isVertexShader);
				appendLink(instruction, instruction.argument2, instruction.component2);
			}
		}
		
		override protected function visitOverwriter(overwriter		: Overwriter, 
													isVertexShader	: Boolean) : void
		{
			var numArguments	: uint = overwriter.numArguments;
			
			appendNode(overwriter, 'darkslategray1', 'Overwriter');
			
			for (var argumentId : uint = 0; argumentId < numArguments; ++argumentId)
			{
				var argument 	: AbstractNode	= overwriter.getArgumentAt(argumentId);
				var component	: uint			= overwriter.getComponentAt(argumentId);
				
				visit(argument, isVertexShader);
				appendLink(overwriter, argument, component);
			}
		}
		
		override protected function visitExtract(extract		: Extract, 
												 isVertexShader	: Boolean) : void
		{
			visit(extract.argument, true);
			
			appendNode(extract, 'moccasin', 'Extract');
			appendLink(extract, extract.argument, extract.component);
		}
		
		override protected function visitInterpolate(interpolate	: Interpolate, 
													 isVertexShader	: Boolean) : void
		{
			visit(interpolate.argument, true);
			
			appendNode(interpolate, 'lemonchiffon', 'Interpolate');
			appendLink(interpolate, interpolate.argument, interpolate.component);
		}
		
		override protected function visitAttribute(attribute		: Attribute, 
												   isVertexShader	: Boolean) : void
		{
			appendNode(attribute, 'yellowgreen', 'Attribute', attribute.component.fields.join());
		}
		
		override protected function visitConstant(constant			: Constant, 
												  isVertexShader	: Boolean) : void
		{
			appendNode(constant, 'plum1', 'Constant (' + constant.size + ')', constant.value.join(',\\n'));
		}
		
		override protected function visitBindableConstant(bindableConstant	: BindableConstant, 
														  isVertexShader	: Boolean) : void
		{
			appendNode(bindableConstant, 'palegreen1', 'Parameter (' + bindableConstant.size + ')', bindableConstant.bindingName);
			
			var subGraph : AbstractNode = _shaderGraph.computableConstants[bindableConstant.bindingName] as AbstractNode;
			
			if (subGraph != null)
			{
				visit(subGraph, false);
				appendLink(bindableConstant, subGraph);
			}
		}
		
		override protected function visitSampler(sampler		: Sampler, 
												 isVertexShader	: Boolean) : void
		{
			appendNode(sampler, 'darkolivegreen3', 'Sampler');
		}
		
		override protected function visitBindableSampler(bindableSampler	: BindableSampler, 
														 isVertexShader		: Boolean) : void
		{
			appendNode(bindableSampler, 'darkolivegreen3', 'BindableSampler', bindableSampler.bindingName);
		}
		
		override protected function visitVariadicExtract(variadicExtract	: VariadicExtract,
														 isVertexShader		: Boolean) : void
		{
			appendNode(variadicExtract, 'cadetblue1', 'VariadicExtract');
			
			visit(variadicExtract.index, isVertexShader);
			appendLink(variadicExtract, variadicExtract.index);
			
			visit(variadicExtract.constant, isVertexShader);
			appendLink(variadicExtract, variadicExtract.constant);
		}
		
		private function appendNode(node		: Object,
									color		: String, 
									labelLine1	: String, 
									labelLine2	: String = null) : void
		{
			var nodeId : uint = _nodeId++;
			
			_nodeIds[node]	= nodeId;
			_nodes			+= labelLine2 == null ? 
				"\tnode" + nodeId + " [color=" + color + ", label=\"" + labelLine1 + "\"]\n" :
				"\tnode" + nodeId + " [color=" + color + ", label=\"" + labelLine1 + "\\n" + labelLine2 + "\"]\n";
		}
		
		private function appendLink(parent : Object, child : Object, components : int = -1) : void
		{
			var parentId	: uint = _nodeIds[parent];
			var childId		: uint = _nodeIds[child];
			
			if (components != -1)
				_links += "\tnode" + parentId + ' -> ' + 'node' + childId + " [label=\"" + Components.componentToString(components) + "\"]\n";
			else
				_links += "\tnode" + parentId + ' -> ' + 'node' + childId + "\n";
		}
	}
}
