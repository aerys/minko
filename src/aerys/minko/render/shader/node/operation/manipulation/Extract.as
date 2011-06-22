package aerys.minko.render.shader.node.operation.manipulation
{
	import aerys.minko.render.shader.compiler.register.RegisterSwizzling;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.operation.AbstractOperation;
	
	public class Extract extends AbstractOperation
	{
		protected var _swizzle		: uint;
		protected var _outputSize	: uint;
		
		public function get swizzle() : uint
		{
			return _swizzle;
		}
		
		override public function get instructionName() : String
		{
			throw new Error('This is a virtual operand');
		}
		
		override public function get opCode() : uint
		{
			throw new Error('This is a virtual operand');
		}
		
		override public function get size() : uint
		{
			return _outputSize;
		}
		
		public function Extract(arg1		: INode,
								subNode		: uint)
		{
			super(arg1, null);
			
			_swizzle	= Components.getSwizzle(subNode);
			_outputSize	= Components.getSize(subNode);
		}
		
		override public function isSame(node : INode) : Boolean
		{
			var swizzleNode : Extract = node as Extract;
			
			return swizzleNode != null 
				&& _swizzle == swizzleNode._swizzle
				&& _outputSize == swizzleNode._outputSize
				&& _arg1.isSame(swizzleNode._arg1);
		}
		
		override public function toString() : String
		{
			return "Extract\\nvalue=" + RegisterSwizzling.STRINGS[_swizzle];
		}
	}
}
