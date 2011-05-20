package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;
	
	public class SetIfLessThan extends AbstractOperation implements IComponentWiseOperation
	{
		override public function get instructionName() : String
		{
			return 'slt';
		}
		
		override public function get opCode() : uint
		{
			return 0x2a;
		}
		
		override public function get size() : uint
		{
			return Math.max(_arg1.size, _arg2.size);
		}
		
		public function SetIfLessThan(arg1:INode, arg2:INode)
		{
			super(arg1, arg2);
		}
		
		override public function isSame(node : INode) : Boolean
		{
			var castedNode : SetIfLessThan = node as SetIfLessThan;
			return castedNode != null && castedNode._arg1.isSame(_arg1) && castedNode._arg2.isSame(_arg2);
		}
	}
}