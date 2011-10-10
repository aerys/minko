package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IAlignedOperation;
	import aerys.minko.render.shader.node.operation.ICommutativeOperation;

	public class DotProduct4 extends AbstractOperation implements IAlignedOperation, ICommutativeOperation
	{
		override public function get opCode() : uint
		{
			return 0x13;
		}

		override public function get instructionName() : String
		{
			return 'dp4';
		}

		override public function get size() : uint
		{
			return 1;
		}

		public function DotProduct4(arg1 : INode,
									arg2 : INode)
		{
			super(arg1, arg2);

			if (arg1.size != 4 || arg2.size != 4)
				throw new Error('Both arguments must be of size 4');
		}

		override public function isSame(node : INode) : Boolean
		{
			var castedNode : DotProduct4 = node as DotProduct4;

			return castedNode != null && (
				(castedNode._arg1.isSame(_arg1) && castedNode._arg2.isSame(_arg2)) ||
				(castedNode._arg1.isSame(_arg2) && castedNode._arg2.isSame(_arg1))
			);
		}
	}
}
