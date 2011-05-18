package aerys.minko.render.shader.node.operation
{
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	import aerys.minko.render.shader.node.INode;
	
	import flash.utils.getQualifiedClassName;
	
	public class AbstractOperation implements INode
	{
		protected var _arg1 : INode;
		protected var _arg2 : INode;
		
		public function get arg1() : INode
		{
			return _arg1;
		}
		
		public function get arg2() : INode
		{
			return _arg2;
		}
		
		public function get opCode() : uint
		{
			throw new Error('Must be overriden');
		}
		
		public function get instructionName() : String
		{
			throw new Error('Must be overriden');
		}
		
		public function get name() : String
		{
			return getQualifiedClassName(this);
		}
		
		public function get size() : uint
		{
			throw new Error('Must be overriden');
		}
		
		public function AbstractOperation(arg1	: INode, 
										  arg2	: INode)
		{
			_arg1 = arg1;
			_arg2 = arg2;
		}
		
		public function swapChildren(from	: INode,
									 to		: INode) : void
		{
			if (_arg1 == from)
				_arg1 = to;
			
			if (_arg2 == from)
				_arg2 = to;
		}
		
		public function accept(v : IShaderNodeVisitor) : void
		{
			_arg1 && v.visit(_arg1);
			_arg2 && v.visit(_arg2);
		}
		
		public function isSame(node : INode) : Boolean
		{
			throw new Error('Must be overriden');
		}
		
		public function toString() : String
		{
			return name;
		}
	}
}
