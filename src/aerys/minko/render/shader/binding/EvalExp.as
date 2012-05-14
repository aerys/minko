package aerys.minko.render.shader.binding
{
	import aerys.minko.render.shader.compiler.Evaluator;
	import aerys.minko.render.shader.compiler.Serializer;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.BindableConstant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	
	import flash.utils.Dictionary;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class EvalExp
	{
		private var _tree				: AbstractNode;
		
		private var _bindableConstants	: Vector.<BindableConstant>;
		private var _binderNames		: Vector.<String>;
		private var _constantSizes		: Object;
		
		public function get bindableConstants() : Vector.<BindableConstant>
		{
			return _bindableConstants;
		}
		
		public function EvalExp(tree : AbstractNode)
		{
			_tree				= tree;
			_bindableConstants	= getBindableConstants(tree);
			_binderNames		= new Vector.<String>();
			_constantSizes		= new Object();
			
			for each (var constant : BindableConstant in _bindableConstants)
			{
				_binderNames.push(constant.bindingName);
				_constantSizes[constant.bindingName] = constant.size;
			}
		}
		
		private function getBindableConstants(tree : AbstractNode) : Vector.<BindableConstant>
		{
			var constants : Vector.<BindableConstant> = new Vector.<BindableConstant>();
			
			getBindableConstantsRec(tree, constants);
			
			return constants;
		}
		
		private function getBindableConstantsRec(tree				: AbstractNode,
												 bindableConstants	: Vector.<BindableConstant>) : void
		{
			if (tree is BindableConstant)
			{
				bindableConstants.push(BindableConstant(tree));
			}
			else
			{
				var numArguments : uint = tree.numArguments;
				
				for (var argumentId : uint = 0; argumentId < numArguments; ++argumentId)
					getBindableConstantsRec(tree.getArgumentAt(argumentId), bindableConstants);
			}
		}
		
		public function compute(dataStore : Dictionary) : Vector.<Number>
		{
			var numBinders	: uint		= _binderNames.length;
			var allSet		: Boolean	= true;
			
			for (var binderId : uint = 0; binderId < numBinders; ++binderId)
				if (!dataStore[_binderNames[binderId]])
				{
					allSet = false;
					break;
				}
			
			return allSet ? visit(_tree, dataStore) : null;
		}
		
		private function visit(node	: AbstractNode, dataStore : Dictionary) : Vector.<Number>
		{
			if (node is Constant)
				return Constant(node).value;
			
			else if (node is BindableConstant)
			{
				return dataStore[BindableConstant(node).bindingName];
			}
			
			else if (node is Instruction)
			{
				var instruction : Instruction = Instruction(node);
				var arg1 : Vector.<Number>;
				var arg2 : Vector.<Number>;
				
				arg1 = visit(instruction.argument1, dataStore);
				if (arg1.length <= 4)
					arg1 = Evaluator.evaluateComponents(instruction.component1, arg1);
				
				if (!instruction.isSingle)
				{
					arg2 = visit(instruction.argument2, dataStore);
					if (arg2.length <= 4)
						arg2 = Evaluator.evaluateComponents(instruction.component2, arg2);
					
					return Evaluator.EVALUATION_FUNCTIONS[instruction.id](arg1, arg2);
				}
				
				return Evaluator.EVALUATION_FUNCTIONS[instruction.id](arg1);
			}
			
			else if (node is Overwriter)
			{
				var tmpVec					: Vector.<Number>	= new Vector.<Number>();
				var overwriter				: Overwriter		= Overwriter(node);
				var overwriterNumArgs		: uint				= overwriter.numArguments;
				var overwriterSize			: uint				= overwriter.size;
				
				var result					: Vector.<Number>	= new Vector.<Number>(overwriterSize, true);
				for (var argId : uint = 0; argId < overwriterNumArgs; ++argId)
				{
					var argument	: AbstractNode	= overwriter.getArgumentAt(argId);
					var component	: uint			= overwriter.getComponentAt(argId);
					
					tmpVec = visit(argument, dataStore);
					tmpVec = Evaluator.evaluateComponentWithHoles(component, tmpVec);
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