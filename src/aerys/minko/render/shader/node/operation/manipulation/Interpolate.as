package aerys.minko.render.shader.node.operation.manipulation
{
	import aerys.minko.render.shader.node.IFragmentNode;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	
	public class Interpolate extends AbstractOperation implements IFragmentNode
	{
		override public function get opCode() : uint
		{
			throw new Error('This is a virtual operand');
		}
		
		override public function get instructionName() : String
		{
			throw new Error('This is a virtual operand');
		}
		
		override public function get size() : uint
		{
			return _arg1.size;
		}
		
		public function Interpolate(arg1 : INode)
		{
			super(arg1, null);
		}
		
		override public function isSame(node : INode) : Boolean
		{
			var interpolateNode : Interpolate = node as Interpolate;
			
			return interpolateNode != null && _arg1.isSame(interpolateNode._arg1);
		}
	}
}
