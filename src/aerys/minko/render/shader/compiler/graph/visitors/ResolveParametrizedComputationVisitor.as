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
			if (_isComputable[_shaderGraph.position] && !isConstant(_shaderGraph.position))
				_shaderGraph.position = createComputableConstant(_shaderGraph.position);
			
			if (_isComputable[_shaderGraph.color] && !isConstant(_shaderGraph.color))
				_shaderGraph.color = createComputableConstant(_shaderGraph.color);
			
			var kills		: Vector.<AbstractNode>	= _shaderGraph.kills;
			var numKills	: uint					= kills.length;
			
			for (var killId : uint = 0; killId < numKills; ++killId)
			{
				var kill : AbstractNode = kills[killId];
				if (_isComputable[kill] && !isConstant(kill))
					kills[killId] = createComputableConstant(kill);
			}
			
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
			
			var isComputable1 : Boolean = _isComputable[instruction.argument1];
			var isComputable2 : Boolean = !instruction.isSingle ? _isComputable[instruction.argument2] : null;
			
			if (instruction.isSingle)
			{
				_isComputable[instruction] = isComputable1;
			}
			else
			{
				_isComputable[instruction] = isComputable1 && isComputable2;
				
				if (!_isComputable[instruction])
				{
					// check if either arg1 or 2 is computable, and useful to compute
					if (isComputable1 && !isConstant(instruction.argument1))
						instruction.argument1 = createComputableConstant(instruction.argument1);
					
					if (isComputable2 && !isConstant(instruction.argument2))
						instruction.argument2 = createComputableConstant(instruction.argument2);
				}
			}
		}
		
		override protected function visitOverwriter(overwriter	   : Overwriter, 
													isVertexShader : Boolean) : void
		{
			visitArguments(overwriter, isVertexShader);
			
			var argumentId		: int;
			var numArguments	: uint				= overwriter.numArguments;
			
			var computableArgs	: Vector.<AbstractNode>	= new Vector.<AbstractNode>();
			var computableComps	: Vector.<uint>			= new Vector.<uint>();
			
			// which arguments are computable?
			// remove them from the overwriter
			for (argumentId = numArguments - 1; argumentId >= 0; --argumentId)
			{
				var argument : AbstractNode = overwriter.getArgumentAt(argumentId)
				if (_isComputable[argument])
				{
					computableArgs.push(argument);
					computableComps.push(overwriter.getComponentAt(argumentId));
					
					overwriter.removeArgumentAt(argumentId);
					--numArguments;
				}
			}
			
			var computableArgsCount : uint = computableArgs.length;
			
			// all arguments are computable!
			if (numArguments == 0)
			{
				// put the arguments and components back
				numArguments = computableArgs.length;
				for (argumentId = 0; argumentId < numArguments; ++argumentId)
					overwriter.addArgumentAt(argumentId, 
						computableArgs[argumentId], computableComps[argumentId]);
				
				// tell the parent we are computable
				_isComputable[overwriter] = true;
			}
			// no arguments are computable
			else if (computableArgsCount == 0)
			{
				_isComputable[overwriter] = false;
			}
			// only one argument is computable. 
			else if (computableArgsCount == 1)
			{
				// there is no need to replace it if it's already a parameter or a contant.
				if (isConstant(computableArgs[0]))
				{
					overwriter.addArgumentAt(0, computableArgs[0], computableComps[0]);
				}
				// we must replace it otherwise
				else
				{
					overwriter.addArgumentAt(0,
						createComputableConstant(computableArgs[0]),
						computableComps[0]
					);
				}
			}
			// more than one argument is computable in CPU, 
			// we are going to decompose this overwriter into 2 overwriters. One on CPU, one on GPU
			else
			{
				
				Minko.log(DebugLevel.SHADER_WARNING, 'Could not split overwriter. Shader code was not fully optimized');
				
				// put everything back to normal
				numArguments = computableArgs.length;
				for (argumentId = 0; argumentId < numArguments; ++argumentId)
					overwriter.addArgumentAt(argumentId, 
						computableArgs[argumentId], computableComps[argumentId]);
				
			}
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