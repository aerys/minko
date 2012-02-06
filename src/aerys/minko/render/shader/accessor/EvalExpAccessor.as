package aerys.minko.render.shader.accessor
{
	import aerys.minko.render.shader.compiler.Evaluator;
	import aerys.minko.render.shader.compiler.Serializer;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Constant;
	import aerys.minko.render.shader.compiler.graph.nodes.leaf.Parameter;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Instruction;
	import aerys.minko.render.shader.compiler.graph.nodes.vertex.Overwriter;
	
	import flash.utils.Dictionary;
	
	public class EvalExpAccessor implements IParameterAccessor
	{
		private var _tree			: INode;
		private var _visitMethods	: Dictionary;
		
		public function get hash() : uint
		{
			return 0;
		}
		
		public function get tree() : INode
		{
			return _tree;
		}
		
		public function EvalExpAccessor(tree : INode)
		{
			_visitMethods				= new Dictionary();
			_visitMethods[Constant]		= visitConstant;
			_visitMethods[Parameter]	= visitParameter;
			_visitMethods[Overwriter]	= visitOverwriter;
			_visitMethods[Instruction]	= visitInstruction;
			
			_tree = tree;
		}
		
		public function getParameter() : Object
		{
			return visit(_tree);
		}
		
		private function visit(node	: INode) : Vector.<Number>
		{
			var method : Function = _visitMethods[Object(node).constructor];
			
			if (method == null)
				throw new Error('Only Parameters, Constants, Instructions and Overwriters are allowed in an EvalExp');
			
			return method(node);
		}
		
		private function visitParameter(param			: Parameter) : Vector.<Number>
		{
			var data	: Object			= param.accessor.getParameter();
			var result	: Vector.<Number>	= new Vector.<Number>();
			Serializer.serializeKnownLength(data, result, 0, param.size);
			
			return result;
		}
		
		private function visitConstant(constant			: Constant) : Vector.<Number>
		{
			return constant.value;
		}
		
		private function visitInstruction(instruction	: Instruction) : Vector.<Number>
		{
			var arg1 : Vector.<Number>;
			var arg2 : Vector.<Number>;
			
			arg1 = visit(instruction.arg1);
			arg1 = Evaluator.evaluateComponents(instruction.arg1Components, arg1);
			
			arg2 = visit(instruction.arg2);
			arg2 = Evaluator.evaluateComponents(instruction.arg2Components, arg2);
			
			return Evaluator.EVALUTION_FUNCTIONS[instruction.id](arg1, arg2);
		}
		
		private function visitOverwriter(overwriter		: Overwriter,
										 worldData		: Dictionary) : Vector.<Number>
		{
			var tmpVec		: Vector.<Number>	= new Vector.<Number>();
			var args		: Vector.<INode>	= overwriter.args;
			var components	: Vector.<uint>		= overwriter.components;
			var numArgs		: uint				= args.length;
			var size		: uint				= overwriter.size;
			
			var result		: Vector.<Number>	= new Vector.<Number>(size, true);
			for (var argId : uint = 0; argId < numArgs; ++argId)
			{
				tmpVec = visit(args[argId]);
				tmpVec = Evaluator.evaluateComponentWithHoles(components[argId], tmpVec);
				for (var i : uint = 0; i < size; ++i)
					if (!isNaN(tmpVec[i]))
						result[i] = tmpVec[i];
			}
			
			return result;
		}
	}
}
