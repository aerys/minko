package aerys.minko.render.shader.node
{
	import aerys.minko.render.shader.compiler.visitor.IShaderNodeVisitor;

	public class Dummy implements INode
	{
		protected var _node : INode;

		public function get name() : String
		{
			return 'Dummy';
		}

		public function get node() : INode
		{
			return _node;
		}

		public function get size() : uint
		{
			return _node.size;
		}

		public function Dummy(node : INode)
		{
			_node = node;
		}

		public function accept(v : IShaderNodeVisitor) : void
		{
			v.visit(_node);
		}

		public function isSame(node : INode) : Boolean
		{
			throw new Error('Dummy nodes are to be removed before folding the shader graph and therefor cannot be compared. Go fix your code.');
		}

		public function toString() : String
		{
			return name;
		}
	}
}
