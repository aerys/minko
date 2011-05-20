package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;
	
	public class SetIfGreaterEqual extends AbstractOperation implements IComponentWiseOperation
	{
		override public function get instructionName() : String
		{
			return 'sge';
		}
		
		override public function get opCode() : uint
		{
			return 0x29;
		}
		
		override public function get size() : uint
		{
			return Math.max(_arg1.size, _arg2.size);
		}
		
		public function SetIfGreaterEqual(arg1 : INode, arg2 : INode)
		{
			super(arg1, arg2);
		}
		
		override public function isSame(node : INode) : Boolean
		{
			var castedNode : SetIfGreaterEqual = node as SetIfGreaterEqual;
			return castedNode != null && castedNode._arg1.isSame(_arg1) && castedNode._arg2.isSame(_arg2);
		}
	}
}