package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.graph.ShaderGraph;
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
	
	import flash.utils.Dictionary;

	/**
	 * @private
	 * @author Romain Gilliotte
	 */
	public class AbstractVisitor
	{
		protected var _stack			: Vector.<ANode>;
		
		protected var _visitedInVs		: Vector.<ANode>;
		protected var _visitedInFs		: Vector.<ANode>;
		
		protected var _shaderGraph		: ShaderGraph;
		
		public function AbstractVisitor()
		{
			_stack			= new Vector.<ANode>();
			_visitedInVs	= new Vector.<ANode>();
			_visitedInFs	= new Vector.<ANode>();
		}
		
		public function process(shaderGraph : ShaderGraph) : void
		{
			_shaderGraph = shaderGraph;
			
			start();
			
			visit(_shaderGraph.position, true);
			visit(_shaderGraph.color, false);
			for each (var kill : ANode in _shaderGraph.kills)
				visit(kill, false);
			
			finish();
		}
		
		protected function start() : void
		{
			_stack.length		= 0;
			_visitedInVs.length	= 0;
			_visitedInFs.length	= 0;
		}
		
		protected function finish() : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function visit(node : ANode, isVertexShader : Boolean) : void
		{
			var visitationTable : Vector.<ANode> = isVertexShader ? _visitedInVs : _visitedInFs;
			if (visitationTable != null)
			{
				if (visitationTable.indexOf(node) != -1)
					return;
				
				visitationTable.push(node);
			}
			
			_stack.push(node);
			
			if (node is Extract || node is Instruction || node is Interpolate || node is Overwriter
				|| node is VariadicExtract)
				visitTraversable(node, isVertexShader);
			else
				visitNonTraversable(node, isVertexShader);
			
			_stack.pop();
		}
		
		protected function visitArguments(node : ANode, isVertexShader : Boolean) : void
		{
			var numArguments : uint = node.numArguments;
			
			for (var argumentId : uint = 0; argumentId < numArguments; ++argumentId)
				visit(node.getArgumentAt(argumentId), isVertexShader);
		}
		
		protected function replaceInParents(oldNode : ANode, newNode : ANode) : void
		{
			var numParents : uint = oldNode.numParents;
			
			for (var parentId : uint = 0; parentId < numParents; ++parentId)
			{
				var parent		: ANode = oldNode.getParentAt(parentId);
				var numArgument	: uint	= parent.numArguments;
				
				// loop backward, because we are removing elements from the parents array
				for (var argumentId : int = numArgument - 1; argumentId >= 0; --argumentId)
					if (parent.getArgumentAt(argumentId) === oldNode)
					{
						// replace only the first occurence.
						// if more are present, they will be removed by another instance of the parent.
						parent.setArgumentAt(argumentId, newNode);
						break;
					}
			}
			
			if (_shaderGraph.position == oldNode)
				_shaderGraph.position = newNode;
			
			if (_shaderGraph.color == oldNode)
				_shaderGraph.color = newNode;
			
			var kills		: Vector.<ANode> = _shaderGraph.kills;
			var numKills	: uint = kills.length;
			
			for (var killId : uint = 0; killId < numKills; ++killId)
				if (kills[killId] === oldNode)
					kills[killId] = newNode;
		}
		
		protected function swizzleParents(node		: ANode,
										  modifier	: uint) : void
		{
			var numParents		: uint = node.numParents;
			var visitedParents	: Dictionary = new Dictionary();
			
			for (var parentId : uint = 0; parentId < numParents; ++parentId)
			{
				var parent		: ANode = node.getParentAt(parentId);
				
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
			
			var kills			: Vector.<ANode> = _shaderGraph.kills;
			var killComponents	: Vector.<uint>  = _shaderGraph.killComponents;
			var numKills		: uint = kills.length;
			
			for (var killId : uint = 0; killId < numKills; ++killId)
				if (kills[killId] === node)
					killComponents[killId] = Components.applyCombination(modifier, killComponents[killId]);
		}
		
		protected function replaceInParentsAndSwizzle(oldNode	: ANode,
													  newNode	: ANode,
													  modifier	: uint) : void
		{
			
			swizzleParents(oldNode, modifier);
			replaceInParents(oldNode, newNode);
		}
		
		protected function visitTraversable(node : ANode, isVertexShader : Boolean) : void
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
		
		protected function visitNonTraversable(node : ANode, isVertexShader : Boolean) : void
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
			throw new Error('Must be overriden');
		}
		
		protected function visitConstant(constant		: Constant,
										 isVertexShader : Boolean) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function visitBindableConstant(bindableConstant	: BindableConstant,
												 isVertexShader		: Boolean) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function visitSampler(sampler			: Sampler, 
										isVertexShader	: Boolean) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function visitBindableSampler(bindableSampler	: BindableSampler,
												isVertexShader	: Boolean) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function visitExtract(extract			: Extract,
										isVertexShader	: Boolean) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function visitInstruction(instruction		: Instruction,
										    isVertexShader	: Boolean) : void
		{
			throw new Error('Must be overriden');	
		}
		
		protected function visitInterpolate(interpolate		: Interpolate,
											isVertexShader	: Boolean) : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function visitOverwriter(overwriter		: Overwriter,
										   isVertexShader	: Boolean) : void
		{
			throw new Error('Must be overriden');	
		}
		
		protected function visitVariadicExtract(variadicExtract : VariadicExtract,
												isVertexShader	: Boolean) : void
		{
			throw new Error('Must be overriden');
		}
	}
}
