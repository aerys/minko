package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;

	public class Reciprocal extends AbstractOperation implements IComponentWiseOperation
	{
		override public function get opCode() : uint
		{
			return 0x05;
		}

		override public function get instructionName() : String
		{
			return 'rcp';
		}

		override public function get size() : uint
		{
			return _arg1.size;
		}

		public function Reciprocal(arg1 : INode)
		{
			super(arg1, null);

			if (arg1.size > 4)
				throw new Error('Max argument length is 4');
		}

		override public function isSame(node : INode) : Boolean
		{
			var castedNode : Reciprocal = node as Reciprocal;
			return castedNode != null && castedNode._arg1 == _arg1;
		}
	}
}
