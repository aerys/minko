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
	 */
	public class AbstractVisitor
	{
		private var _visited		: Vector.<AbstractNode>;
		protected var _shaderGraph	: ShaderGraph;
		
		public function AbstractVisitor()
		{
			_visited	= new Vector.<AbstractNode>();
		}
		
		public function process(shaderGraph : ShaderGraph) : void
		{
			_shaderGraph = shaderGraph;
			
			start();
			
			visit(_shaderGraph.position, true);
			visit(_shaderGraph.color, false);
			for each (var kill : AbstractNode in _shaderGraph.kills)
				visit(kill, false);
			
			finish();
		}
		
		protected function start() : void
		{
		}
		
		protected function finish() : void
		{
			_visited.length	= 0;
			_shaderGraph	= null;
		}
		
		protected function visit(node : AbstractNode, isVertexShader : Boolean) : void
		{
			if (_visited.indexOf(node) == -1)
			{
				_visited.push(node);
				
				if (node is Extract || node is Instruction || node is Interpolate || node is Overwriter || node is VariadicExtract)
					visitTraversable(node, isVertexShader);
				else
					visitNonTraversable(node, isVertexShader);
			}
		}
		
		protected function visitArguments(node : AbstractNode, isVertexShader : Boolean) : void
		{
			var numArguments : uint = node.numArguments;
			
			for (var argumentId : uint = 0; argumentId < numArguments; ++argumentId)
				visit(node.getArgumentAt(argumentId), isVertexShader);
		}
		
		protected function replaceInParents(oldNode : AbstractNode, newNode : AbstractNode) : void
		{
			var numParents	: uint					= oldNode.numParents;
			var parents		: Vector.<AbstractNode> = new Vector.<AbstractNode>(numParents, true);
			var parentId	: uint;
			
			for (parentId = 0; parentId < numParents; ++parentId)
				parents[parentId] = oldNode.getParentAt(parentId);
			
			for (parentId = 0; parentId < numParents; ++parentId)
			{
				var parent		: AbstractNode	= parents[parentId];
				var numArgument	: uint			= parent.numArguments;
				
				// loop backward, because we are removing elements from the parents array
				for (var argumentId : int = numArgument - 1; argumentId >= 0; --argumentId)
					if (parent.getArgumentAt(argumentId) === oldNode)
						parent.setArgumentAt(argumentId, newNode);
			}
			
			if (_shaderGraph.position === oldNode)
				_shaderGraph.position = newNode;
			
			if (_shaderGraph.color === oldNode)
				_shaderGraph.color = newNode;
			
			var kills		: Vector.<AbstractNode>	= _shaderGraph.kills;
			var numKills	: uint					= kills.length;
			
			for (var killId : uint = 0; killId < numKills; ++killId)
				if (kills[killId] === oldNode)
					kills[killId] = newNode;
		}
		
		protected function swizzleParents(node		: AbstractNode,
										  modifier	: uint) : void
		{
			var numParents		: uint			= node.numParents;
			var visitedParents	: Dictionary	= new Dictionary();
			
			for (var parentId : uint = 0; parentId < numParents; ++parentId)
			{
				var parent : AbstractNode = node.getParentAt(parentId);
				
				if (visitedParents[parent])
					continue;
				visitedParents[parent] = true;
				
				var numArgument	: uint	= parent.numArguments;
				
				// loop backward, because we are removing elements from the parents array
				for (var argumentId : int = numArgument - 1; argumentId >= 0; --argumentId)
					if (parent.getArgumentAt(argumentId) === node)
						parent.setComponentAt(argumentId, 
							Components.applyCombination(modifier, parent.getComponentAt(argumentId))
						);
			}
			
			if (_shaderGraph.position == node)
				_shaderGraph.positionComponents = Components.applyCombination(modifier, _shaderGraph.positionComponents);
			
			if (_shaderGraph.color == node)
				_shaderGraph.colorComponents = Components.applyCombination(modifier, _shaderGraph.colorComponents);
			
			var kills			: Vector.<AbstractNode>	= _shaderGraph.kills;
			var killComponents	: Vector.<uint>			= _shaderGraph.killComponents;
			var numKills		: uint = kills.length;
			
			for (var killId : uint = 0; killId < numKills; ++killId)
				if (kills[killId] === node)
					killComponents[killId] = Components.applyCombination(modifier, killComponents[killId]);
		}
		
		protected function replaceInParentsAndSwizzle(oldNode	: AbstractNode,
													  newNode	: AbstractNode,
													  modifier	: uint) : void
		{
			
			swizzleParents(oldNode, modifier);
			replaceInParents(oldNode, newNode);
		}
		
		protected function visitTraversable(node : AbstractNode, isVertexShader : Boolean) : void
		{
			if (node is Extract)
				visitExtract(Extract(node), isVertexShader);
			else if (node is Instruction)
				visitInstruction(Instruction(node), isVertexShader);
			else if (node is Interpolate)
				visitInterpolate(Interpolate(node), isVertexShader);
			else if (node is Overwriter)
				visitOverwriter(Overwriter(node), isVertexShader);
			else if (node is VariadicExtract)
				visitVariadicExtract(VariadicExtract(node), isVertexShader);
		}
		
		protected function visitNonTraversable(node : AbstractNode, isVertexShader : Boolean) : void
		{
			if (node is Attribute)
				visitAttribute(Attribute(node), isVertexShader);
			else if (node is Constant)
				visitConstant(Constant(node), isVertexShader);
			else if (node is BindableConstant)
				visitBindableConstant(BindableConstant(node), isVertexShader);
			else if (node is Sampler)
				visitSampler(Sampler(node), isVertexShader);
			else if (node is BindableSampler)
				visitBindableSampler(BindableSampler(node), isVertexShader);
		}
		
		protected function visitAttribute(attribute			: Attribute,
										  isVertexShader	: Boolean) : void
		{
		}
		
		protected function visitConstant(constant		: Constant,
										 isVertexShader : Boolean) : void
		{
		}
		
		protected function visitBindableConstant(bindableConstant	: BindableConstant,
												 isVertexShader		: Boolean) : void
		{
		}
		
		protected function visitSampler(sampler			: Sampler, 
										isVertexShader	: Boolean) : void
		{
		}
		
		protected function visitBindableSampler(bindableSampler	: BindableSampler,
												isVertexShader	: Boolean) : void
		{
		}
		
		protected function visitExtract(extract			: Extract,
										isVertexShader	: Boolean) : void
		{
		}
		
		protected function visitInstruction(instruction		: Instruction,
										    isVertexShader	: Boolean) : void
		{
		}
		
		protected function visitInterpolate(interpolate		: Interpolate,
											isVertexShader	: Boolean) : void
		{
		}
		
		protected function visitOverwriter(overwriter		: Overwriter,
										   isVertexShader	: Boolean) : void
		{
		}
		
		protected function visitVariadicExtract(variadicExtract : VariadicExtract,
												isVertexShader	: Boolean) : void
		{
		}
	}
}
