package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;
	import aerys.minko.render.shader.node.INode;
	
	import flash.utils.getQualifiedClassName;
	
	public class AbstractLeaf implements INode
	{
		public function get name() : String
		{
			return getQualifiedClassName(this);
		}
		
		public function get size() : uint
		{
			throw new Error('Must be overriden');
		}

		public function AbstractLeaf()
		{
		}
		
		public function accept(v : IShaderNodeVisitor) : void
		{
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
