package aerys.minko.render.shader.node.operation.manipulation
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IAlignedOperation;

	public class Combine extends AbstractOperation implements IAlignedOperation
	{
		override public function get size() : uint
		{
			return _arg1.size + _arg2.size;
		}

		override public function get opCode() : uint
		{
			return 0x0;
		}

		override public function get instructionName() : String
		{
			return 'mov';
		}

		public function Combine(arg1	: INode,
								arg2	: INode)
		{
			super(arg1, arg2);

			if (arg1.size + arg2.size > 4)
				throw new Error('Cannot combine a ' + arg1.size.toString()
					+ ' and a ' + arg2.size.toString()
					+ ' sized node. The result would be larger than a registry');
		}

		override public function isSame(node : INode) : Boolean
		{
			var combineNode : Combine = node as Combine;
			return combineNode != null && combineNode._arg1.isSame(_arg1) && combineNode._arg2.isSame(_arg2);
		}

		override public function toString() : String
		{
			return "Combine";
		}
	}
}
