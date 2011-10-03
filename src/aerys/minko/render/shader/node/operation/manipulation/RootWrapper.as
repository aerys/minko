package aerys.minko.render.shader.node.operation.manipulation
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;

	public class RootWrapper extends AbstractOperation implements IComponentWiseOperation
	{
		override public function isSame(node : INode) : Boolean
		{
			var rootWrapNode : RootWrapper = node as RootWrapper;
			return rootWrapNode != null && _arg1.isSame(rootWrapNode._arg1);
		}

		override public function get opCode() : uint
		{
			return 0x0;
		}

		override public function get size() : uint
		{
			return _arg1.size;
		}

		override public function get name() : String
		{
			return 'RootWrapper';
		}

		override public function get instructionName() : String
		{
			return 'mov';
		}

		public function RootWrapper(arg1 : INode)
		{
			super(arg1, null);
		}
	}
}
