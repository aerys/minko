package aerys.minko.render.shader.binding
{
	import aerys.minko.render.shader.compiler.Evaluator;
	import aerys.minko.render.shader.compiler.Serializer;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableConstant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	
	import flash.utils.Dictionary;

	public class EvalExp
	{
		private var _tree				: INode;
		
		private var _bindableConstants	: Vector.<BindableConstant>;
		private var _binderNames		: Vector.<String>;
		private var _constantSizes		: Object;
		private var _constantValues		: Dictionary;
		
		private var _dirty				: Boolean;
		private var _allSet				: Boolean;
		private var _result				: Vector.<Number>
		
		public function get bindableConstants() : Vector.<BindableConstant>
		{
			return _bindableConstants;
		}
		
		public function get result() : Vector.<Number>
		{
			if (_dirty && _allSet)
			{
				_result = visit(_tree);
				_dirty	= false;
			}
			
			return _result;
		}
		
		public function EvalExp(tree : INode)
		{
			_dirty				= true;
			_allSet				= false;
			_tree				= tree;
			
			_constantValues		= new Dictionary();
			_bindableConstants	= getBindableConstants(tree);
			
			_binderNames		= new Vector.<String>();
			_constantSizes		= new Object();
			
			for each (var constant : BindableConstant in _bindableConstants)
			{
				_binderNames.push(constant.bindingName);
				_constantSizes[constant.bindingName] = constant.size;
			}
		}
		
		private function getBindableConstants(tree : INode) : Vector.<BindableConstant>
		{
			var constants : Vector.<BindableConstant> = new Vector.<BindableConstant>();
			
			getBindableConstantsRec(tree, constants);
			
			return constants;
		}
		
		private function getBindableConstantsRec(tree : INode, bindableConstants : Vector.<BindableConstant>) : void
		{
			if (tree is Instruction)
			{
				var instruction : Instruction = Instruction(tree);
				getBindableConstantsRec(instruction.arg1, bindableConstants);
				if (!instruction.isSingle)
					getBindableConstantsRec(instruction.arg2, bindableConstants);
			}
			else if (tree is Overwriter)
			{
				var overwriter : Overwriter = Overwriter(tree);
				for each (var arg : INode in overwriter.args)
				getBindableConstantsRec(arg, bindableConstants);
			}
			else if (tree is BindableConstant)
			{
				bindableConstants.push(BindableConstant(tree));
			}
		}
		
		public function changeBindedConstant(name : String, value : Object) : void
		{
			if (!_constantValues[name])
				_constantValues[name] = new Vector.<Number>();
			
			Serializer.serializeKnownLength(value, _constantValues[name], 0, _constantSizes[name]);
			
			if (!_allSet)
			{
				var numBinders : uint = _binderNames.length;
				
				_allSet	= true;
				for (var binderId : uint = 0; binderId < numBinders; ++binderId)
					if (!_constantValues[_binderNames[binderId]])
					{
						_allSet = false;
						break;
					}
			}
			
			_dirty = true;
		}
		
		private function visit(node	: INode) : Vector.<Number>
		{
			if (node is Constant)
				return Constant(node).value;
			
			else if (node is BindableConstant)
			{
				return _constantValues[BindableConstant(node).bindingName];
			}
			
			else if (node is Instruction)
			{
				var instruction : Instruction = Instruction(node);
				var arg1 : Vector.<Number>;
				var arg2 : Vector.<Number>;
				
				arg1 = visit(instruction.arg1);
				arg1 = Evaluator.evaluateComponents(instruction.arg1Components, arg1);
				
				arg2 = visit(instruction.arg2);
				arg2 = Evaluator.evaluateComponents(instruction.arg2Components, arg2);
				
				return Evaluator.EVALUTION_FUNCTIONS[instruction.id](arg1, arg2);
			}
			
			else if (node is Overwriter)
			{
				var tmpVec					: Vector.<Number>	= new Vector.<Number>();
				var overwriter				: Overwriter		= Overwriter(node);
				var overwriterArgs			: Vector.<INode>	= overwriter.args;
				var overwriterComponents	: Vector.<uint>		= overwriter.components;
				var overwriterNumArgs		: uint				= overwriterArgs.length;
				var overwriterSize			: uint				= overwriter.size;
				
				var result					: Vector.<Number>	= new Vector.<Number>(overwriterSize, true);
				for (var argId : uint = 0; argId < overwriterNumArgs; ++argId)
				{
					tmpVec = visit(overwriterArgs[argId]);
					tmpVec = Evaluator.evaluateComponentWithHoles(overwriterComponents[argId], tmpVec);
					for (var i : uint = 0; i < overwriterSize; ++i)
						if (!isNaN(tmpVec[i]))
							result[i] = tmpVec[i];
				}
				
				return result;
			}
			
			else throw new Error("Unknown node type");
		}
		
	}
}