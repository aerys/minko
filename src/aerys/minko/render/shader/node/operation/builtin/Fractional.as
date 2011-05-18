package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;
	
	public class Fractional extends AbstractOperation implements IComponentWiseOperation
	{
		override public function get opCode():uint
		{
			return 0x08;
		}
		
		override public function get instructionName():String
		{
			return 'frc';
		}
		
		override public function get size() : uint
		{
			return _arg1.size;
		}
		
		public function Fractional(arg1 : INode)
		{
			super(arg1, null);
			
			if (arg1.size > 4)
				throw new Error('Max argument length is 4');
		}
		
		override public function isSame(node : INode) : Boolean
		{
			var castedNode : Fractional = node as Fractional;
			
			return castedNode != null && castedNode._arg1.isSame(_arg1); 
		}
	}
}
