package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;

	public class Logarithm extends AbstractOperation implements IComponentWiseOperation
	{
		override public function get opCode() : uint
		{
			return 0x0c;
		}

		override public function get instructionName() : String
		{
			return 'log';
		}

		override public function get size() : uint
		{
			return _arg1.size;
		}

		public function Logarithm(arg1 : INode)
		{
			super(arg1, null);
		}

		override public function isSame(node : INode) : Boolean
		{
			var castedNode : Logarithm = node as Logarithm;
			return castedNode != null && castedNode._arg1.isSame(_arg1);
		}

	}
}
