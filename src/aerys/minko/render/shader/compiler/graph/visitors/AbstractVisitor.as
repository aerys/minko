package aerys.minko.render.shader.compiler.graph.visitors
{
	import aerys.minko.render.shader.compiler.graph.ShaderGraph;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
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
	public class AbstractVisitor
	{
		protected var _stack			: Vector.<INode>;
		
		protected var _visitedInVs		: Vector.<INode>;
		protected var _visitedInFs		: Vector.<INode>;
		
		protected var _shaderGraph		: ShaderGraph;
		
		public function AbstractVisitor(oneTimePerNode : Boolean)
		{
			_stack			= new Vector.<INode>();
			_visitedInVs	= oneTimePerNode ? new Vector.<INode>() : null;
			_visitedInFs	= oneTimePerNode ? new Vector.<INode>() : null;
		}
		
		public function process(shaderGraph : ShaderGraph) : void
		{
			_shaderGraph = shaderGraph;
			
			start();
			
			visit(_shaderGraph.position, true);
			visit(_shaderGraph.color, false);
			for each (var kill : INode in _shaderGraph.kills)
				visit(kill, false);
			
			finish();
		}
		
		protected function start() : void
		{
			_stack.length = 0;
			_visitedInVs.length = 0;
			_visitedInFs.length = 0;
		}
		
		protected function finish() : void
		{
			throw new Error('Must be overriden');
		}
		
		protected function visit(node : INode, isVertexShader : Boolean) : void
		{
			var visitationTable : Vector.<INode> = isVertexShader ? _visitedInVs : _visitedInFs;
			if (visitationTable != null)
			{
				if (visitationTable.indexOf(node) != -1)
					return;
				
				visitationTable.push(node);
			}
			
			_stack.push(node);
			
			if (node is Extract)
				visitExtract(Extract(node), isVertexShader);
			
			if (node is Attribute)
				visitAttribute(Attribute(node), isVertexShader);
			
			else if (node is Constant)
				visitConstant(Constant(node), isVertexShader);
			else if (node is BindableConstant)
				visitBindableConstant(BindableConstant(node), isVertexShader);
			else if (node is VariadicExtract)
				visitVariadicExtract(VariadicExtract(node), isVertexShader);
			
			else if (node is Sampler)
				visitSampler(Sampler(node), isVertexShader);
			else if (node is BindableSampler)
				visitBindableSampler(BindableSampler(node), isVertexShader);
			
			else if (node is Instruction)
				visitInstruction(Instruction(node), isVertexShader);
			else if (node is Interpolate)
				visitInterpolate(Interpolate(node), isVertexShader);
			else if (node is Overwriter)
				visitOverwriter(Overwriter(node), isVertexShader);
			
			_stack.pop();
		}
		
		protected function forgetVisiting(node : INode, isVertexShader : Boolean) : void
		{
			var visitationTable : Vector.<INode> = isVertexShader ? _visitedInVs : _visitedInFs;
			
			var index : int = visitationTable.indexOf(node);
			if (index == -1)
				throw new Error("No such node.");
			
			visitationTable.splice(index, 1);
		}
		
		protected function replaceInParentAndSwizzle(oldNode : INode, newNode : INode, modifier : uint) : void
		{
			swizzleParent(oldNode, modifier);
			replaceInParent(oldNode, newNode);
		}
		
		protected function replaceInParent(oldNode : INode, newNode : INode) : void
		{
			var workDone : Boolean = false;
			
			if (_stack.length < 2)
			{
				if (_shaderGraph.position === oldNode)
				{
					_shaderGraph.position	= newNode;
					workDone				= true;
				}
				
				if (_shaderGraph.color === oldNode)
				{
					_shaderGraph.color	= newNode;
					workDone			= true;
				}
				
				var numKills : uint = _shaderGraph.kills.length;
				for (var killId : uint = 0; killId < numKills; ++killId)
					if (_shaderGraph.kills[killId] === oldNode)
					{
						_shaderGraph.kills[killId]	= newNode;
						workDone					= true;
					}
			}
			else
			{
				var parent : INode = _stack[_stack.length - 2];
				
				if (parent is Instruction)
				{
					var instruction : Instruction = Instruction(parent);
					
					if (instruction.arg1 === oldNode)
					{
						instruction.arg1 = newNode;
						workDone = true;
					}
					
					if (instruction.arg2 === oldNode)
					{
						instruction.arg2 = newNode;
						workDone		 = true;
					}
				}
				else if (parent is Interpolate)
				{
					var interpolate : Interpolate = Interpolate(parent);
					
					if (interpolate.arg === oldNode)
					{
						interpolate.arg = newNode;
						workDone = true;
					}
				}
				else if (parent is Overwriter)
				{
					var overwriter			: Overwriter		= Overwriter(parent);
					var overwriterArgs		: Vector.<INode>	= overwriter.args;
					var overwriterNumArgs	: uint				= overwriterArgs.length;
					
					for (var argId : uint = 0; argId < overwriterNumArgs; ++argId)
						if (overwriterArgs[argId] === oldNode)
						{
							overwriterArgs[argId] = newNode;
							workDone = true;
						}
				}
				else if (parent is Extract)
				{
					var extract : Extract = Extract(parent);
					
					if (extract.child === oldNode)
					{
						extract.child = newNode;
						workDone = true;
					}
				}
				else if (parent is VariadicExtract)
				{
					var variadicExtract : VariadicExtract = VariadicExtract(parent);
					
					if (variadicExtract.index === oldNode)
					{
						variadicExtract.index = newNode;
						workDone = true;
					}
				}
				else
					throw new Error('Unknown shader graph vertex.');
			}
			
			if (!workDone)
				throw new Error('Invalid replace operation: the node was not found in parent');
		}
		
		protected function swizzleParent(node		: INode,
										 modifier	: uint) : void
		{
			var workDone : Boolean = false;
			
			if (_stack.length < 2)
			{
				
				if (_shaderGraph.position === node)
				{
					_shaderGraph.positionComponents = Components.applyCombination(modifier, _shaderGraph.positionComponents);
					workDone					= true;
				}
				
				if (_shaderGraph.color === node)
				{
					_shaderGraph.colorComponents = Components.applyCombination(modifier, _shaderGraph.colorComponents);
					workDone					= true;
				}
				
				var numKills : uint = _shaderGraph.kills.length;
				for (var killId : uint = 0; killId < numKills; ++killId)
					if (_shaderGraph.kills[killId] === node)
					{
						_shaderGraph.killComponents[killId]	= Components.applyCombination(modifier, _shaderGraph.killComponents[killId]);
						workDone					= true;
					}
			}
			else
			{
				var parent : INode = _stack[_stack.length - 2];
				
				if (parent is Instruction)
				{
					var instruction : Instruction = Instruction(parent);
					
					if (instruction.arg1 === node)
					{
						instruction.arg1Components	= Components.applyCombination(modifier, instruction.arg1Components);
						workDone					= true;
					}
					
					if (instruction.arg2 === node)
					{
						instruction.arg2Components	= Components.applyCombination(modifier, instruction.arg2Components);
						workDone					= true;
					}
				}
				else if (parent is Interpolate)
				{
					var interpolate : Interpolate = Interpolate(parent);
					
					if (interpolate.arg === node)
					{
						interpolate.components	= Components.applyCombination(modifier, interpolate.components);
						workDone				= true;
					}
				}
				else if (parent is Overwriter)
				{
					var overwriter			: Overwriter		= Overwriter(parent);
					var overwriterArgs		: Vector.<INode>	= overwriter.args;
					var overwriterComponents: Vector.<uint>		= overwriter.components;
					var overwriterNumArgs	: uint				= overwriterArgs.length;
					
					for (var argId : uint = 0; argId < overwriterNumArgs; ++argId)
						if (overwriterArgs[argId] === node)
						{
							overwriterComponents[argId]	= Components.applyCombination(modifier, overwriterComponents[argId]);
							workDone					= true;
						}
				}
				else if (parent is Extract)
				{
					var extract : Extract = Extract(parent);
					
					if (extract.child === node)
					{
						extract.components	= Components.applyCombination(modifier, extract.components);;
						workDone			= true;
					}
				}
				else if (parent is VariadicExtract)
				{
					var variadicExtract : VariadicExtract = VariadicExtract(parent);
					
					if (variadicExtract.index === node)
					{
						variadicExtract.indexComponentSelect += modifier & 0xff;
						workDone			= true;
					}
					if (variadicExtract.constant === node)
					{
						throw new Error("This node cannot be swizzled");
					}
					
				}
				else
					throw new Error('Unknown shader graph vertex.');
				
			}
			
			if (!workDone)
				throw new Error('Invalid swizzle operation: the node was not found in parent');
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
