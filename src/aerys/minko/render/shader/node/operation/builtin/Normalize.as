package aerys.minko.render.shader.node.operation.builtin
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;
	
	public class Normalize extends AbstractOperation implements IComponentWiseOperation
	{
		override public function get opCode():uint
		{
			return 0x0e;
		}
		
		override public function get instructionName():String
		{
			return 'nrm';
		}
		
		override public function get size() : uint
		{
			return Math.min(arg1.size, 3);
		}
		
		public function Normalize(arg1:INode)
		{
			super(arg1, null);
			
			if (arg1.size > 4)
				throw new Error('Max argument length is 4');
		}
		
		override public function isSame(node : INode) : Boolean
		{
			var castedNode : Normalize = node as Normalize;
			
			return castedNode != null && castedNode._arg1.isSame(_arg1); 
		}
	}
}
