package aerys.minko.render.shader.node.operation.math
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;
	import aerys.minko.render.shader.node.operation.builtin.Add;

	public class Sum extends Dummy implements IComponentWiseOperation
	{
		protected static const NEUTRAL : INode = new Constant(0);

		public static function fromVector(v : Vector.<INode>) : INode
		{
			var s : Sum = new Sum();
			var length : uint = v.length;
			for each (var node : INode in v)
				s.addTerm(node);
			return s;
		}

		public function Sum(...terms)
		{
			super(NEUTRAL);

			for each (var node : INode in terms)
				addTerm(node);
		}

		public function addTerm(node : INode) : void
		{
			_node = _node === NEUTRAL ? node : new Add(_node, node);
		}
	}
}
