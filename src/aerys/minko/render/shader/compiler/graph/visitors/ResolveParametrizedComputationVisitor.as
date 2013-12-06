package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.Minko;
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
	import aerys.minko.type.log.DebugLevel;
	
	import flash.utils.Dictionary;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class ResolveParametrizedComputationVisitor extends AbstractVisitor
	{
		private var _isComputable			: Dictionary;
		private var _computableConstantId	: uint;
		
		public function ResolveParametrizedComputationVisitor()
		{
		}
		
		override protected function start() : void
		{
			super.start();
			
			_computableConstantId	= 0;
			_isComputable			= new Dictionary();
		}
		
		override protected function finish() : void
		{
			for (var objNode : Object in _isComputable)
			{
				var node : AbstractNode = AbstractNode(objNode);
				
				if (_isComputable[node] && !isConstant(node))
				{
					var numParents	: uint = node.numParents;
					var parentId	: int;
					
					for (parentId = 0; parentId < numParents; ++parentId)
						if (!_isComputable[node.getParentAt(parentId)])
							break;
					
					// if at least one parent is not computable, we have to replace.
					if (parentId != numParents)
					{
						// copy parents.
						var parents : Vector.<AbstractNode> = new Vector.<AbstractNode>(numParents, true);
						for (parentId = 0; parentId < numParents; ++parentId)
							parents[parentId] = node.getParentAt(parentId);
						
						// for nodes inside the graph, replace only if parent is not computable.
						var computableConstant : BindableConstant = createComputableConstant(node);
						for (parentId = 0; parentId < numParents; ++parentId)
							if (!_isComputable[parents[parentId]])
								parents[parentId].replaceArgument(node, computableConstant);
						
						// for root nodes, replace all the time.
						if (_shaderGraph.position === node)
							_shaderGraph.position = computableConstant;
						
						if (_shaderGraph.color === node)
							_shaderGraph.color = computableConstant;
						
						var kills		: Vector.<AbstractNode>	= _shaderGraph.kills;
						var numKills	: uint					= kills.length;
						
						for (var killId : uint = 0; killId < numKills; ++killId)
							if (kills[killId] === node)
								kills[killId] = computableConstant;
					}
				}
			}
			
			_isComputable			= null;
			_computableConstantId	= 0;
			
			super.finish();
		}
		
		override protected function visitInterpolate(interpolate	: Interpolate, 
													 isVertexShader	: Boolean) : void
		{
			visitArguments(interpolate, true);
			
			_isComputable[interpolate] = _isComputable[interpolate.argument];
		}
		
		override protected function visitVariadicExtract(variadicExtract	: VariadicExtract, 
														 isVertexShader		: Boolean) : void
		{
			visit(variadicExtract.index, true);
			
			_isComputable[variadicExtract] = _isComputable[variadicExtract.index];
		}
		
		override protected function visitInstruction(instruction : Instruction, isVertexShader : Boolean) : void
		{
			visitArguments(instruction, isVertexShader);
			
			if (instruction.isSingle)
				_isComputable[instruction] = _isComputable[instruction.argument1];
			else
				_isComputable[instruction] = _isComputable[instruction.argument1] && _isComputable[instruction.argument2];

		}
		
		override protected function visitOverwriter(overwriter	   : Overwriter, 
													isVertexShader : Boolean) : void
		{
			visitArguments(overwriter, isVertexShader);
			
			var numArguments : uint = overwriter.numArguments;
			
			for (var argumentId : uint = 0; argumentId < numArguments; ++argumentId)
				if (!_isComputable[overwriter.getArgumentAt(argumentId)])
					break;
			
			_isComputable[overwriter] = argumentId == numArguments;
		}
		
		override protected function visitAttribute(attribute	  : Attribute, 
												   isVertexShader : Boolean) : void
		{
			_isComputable[attribute] = false;
		}
		
		override protected function visitConstant(constant		 : Constant, 
												  isVertexShader : Boolean) : void
		{
			_isComputable[constant] = true;
		}
		
		override protected function visitBindableConstant(bindableConstant	: BindableConstant,
														  isVertexShader	: Boolean):void
		{
			_isComputable[bindableConstant] = true;
		}
		
		override protected function visitSampler(sampler		: Sampler, 
												 isVertexShader	: Boolean) : void
		{
			_isComputable[sampler] = false;
		}
		
		override protected function visitBindableSampler(bindableSampler	: BindableSampler, 
														 isVertexShader		: Boolean) : void
		{
			_isComputable[bindableSampler] = false;
		}
		
		override protected function visitExtract(extract		: Extract, 
												 isVertexShader	: Boolean) : void
		{
			throw new Error('Found invalid node: ' + extract.toString());
		}
		
		private function isConstant(node : AbstractNode) : Boolean
		{
			return node is BindableConstant || node is Constant;
		}
		
		private function createComputableConstant(computableNode : AbstractNode) : BindableConstant
		{
			var constantName : String = BindableConstant.COMPUTABLE_CONSTANT_PREFIX + (_computableConstantId++);
			_shaderGraph.computableConstants[constantName] = computableNode;
			
			return new BindableConstant(constantName, computableNode.size);
		}
	}
}