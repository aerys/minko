package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IAlignedOperation;

	public class Multiply4x4 extends AbstractOperation implements IAlignedOperation
	{
		override public function get opCode():uint
		{
			return 0x18;
		}

		override public function get instructionName():String
		{
			return 'm44';
		}

		override public function get size() : uint
		{
			return 4;
		}

		public function Multiply4x4(arg1:INode, arg2:INode)
		{
			super(arg1, arg2);

			if (arg1.size < 3)
				throw new Error('First argument must be of size 3 or 4');

			if (arg2.size !== 16)
				throw new Error('Second argument must be of size 16');
		}

		override public function isSame(node : INode) : Boolean
		{
			var castedNode : Multiply4x4 = node as Multiply4x4;

			return castedNode != null && castedNode._arg1.isSame(_arg1) && castedNode._arg2.isSame(_arg2);
		}
	}
}
