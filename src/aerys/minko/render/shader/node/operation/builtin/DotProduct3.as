package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IAlignedOperation;
	import aerys.minko.render.shader.node.operation.ICommutativeOperation;

	public class DotProduct3 extends AbstractOperation implements IAlignedOperation, ICommutativeOperation
	{
		override public function get opCode() : uint
		{
			return 0x12;
		}

		override public function get instructionName() : String
		{
			return 'dp3';
		}

		override public function get size() : uint
		{
			return 1;
		}

		public function DotProduct3(arg1 : INode,
									arg2 : INode)
		{
			super(arg1, arg2);

			if (arg1.size < 3 || arg2.size < 3)
				throw new Error('Both arguments must be at least of size 3');
		}

		override public function isSame(node : INode) : Boolean
		{
			var castedNode : DotProduct3 = node as DotProduct3;

			return castedNode != null && (
				(castedNode._arg1.isSame(_arg1) && castedNode._arg2.isSame(_arg2)) ||
				(castedNode._arg1.isSame(_arg2) && castedNode._arg2.isSame(_arg1))
			);
		}
	}
}
