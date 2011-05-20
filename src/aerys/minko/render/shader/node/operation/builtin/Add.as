package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.ICommutativeOperation;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;
	
	public class Add extends AbstractOperation implements IComponentWiseOperation, ICommutativeOperation
	{
		override public function get opCode():uint
		{
			return 0x01;
		}
		
		override public function get instructionName():String
		{
			return 'add';
		}
		
		override public function get size() : uint
		{
			return Math.max(_arg1.size, _arg2.size);
		}
		
		public function Add(arg1	: INode,
							arg2	: INode)
		{
			super(arg1, arg2);
		}
		
		override public function isSame(node : INode) : Boolean
		{
			var castedNode : Add = node as Add;
			
			return castedNode != null && (
				(castedNode._arg1.isSame(_arg1) && castedNode._arg2.isSame(_arg2)) || 
				(castedNode._arg1.isSame(_arg2) && castedNode._arg2.isSame(_arg1))
			);
		}

	}
}
