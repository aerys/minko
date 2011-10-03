package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;

	public class ReciprocalRoot extends AbstractOperation implements IComponentWiseOperation
	{
		override public function get opCode() : uint
		{
			return 0x0a;
		}

		override public function get instructionName() : String
		{
			return 'rsq';
		}

		override public function get size() : uint
		{
			return _arg1.size;
		}

		public function ReciprocalRoot(arg1 : INode)
		{
			super(arg1, null);
		}

		override public function isSame(node : INode) : Boolean
		{
			var castedNode : ReciprocalRoot = node as ReciprocalRoot;

			return castedNode != null && castedNode._arg1.isSame(_arg1);
		}
	}
}
