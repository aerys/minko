package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IAlignedOperation;
	
	public class CrossProduct extends AbstractOperation implements IAlignedOperation
	{
		override public function get opCode() : uint
		{
			return 0x11;
		}
		
		override public function get instructionName() : String
		{
			return 'crs';
		}
		
		override public function get size() : uint
		{
			return 3;
		}
		
		public function CrossProduct(arg1 : INode, 
									 arg2 : INode)
		{
			super(arg1, arg2);
		}
		
		override public function isSame(node : INode) : Boolean
		{
			var castedNode : CrossProduct = node as CrossProduct;
			
			return castedNode != null && (
				(castedNode._arg1.isSame(_arg1) && castedNode._arg2.isSame(_arg2)) ||
				(castedNode._arg1.isSame(_arg2) && castedNode._arg2.isSame(_arg1))
			);
		}
	}
}