package aerys.minko.render.shader.node.operation.math
{
	import aerys.minko.render.shader.node.Dummy;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.operation.IComponentWiseOperation;
	import aerys.minko.render.shader.node.operation.builtin.Multiply;
	
	public class Product extends Dummy implements IComponentWiseOperation
	{
		protected static const NEUTRAL : INode = new Constant(1);
		
		public static function fromVector(v : Vector.<INode>) : INode
		{
			var p : Product = new Product();
			for each (var node : INode in v)
				p.addTerm(node);
			
			return p;
		}
		
		public function Product(...terms)
		{
			super(NEUTRAL);
			
			for each (var node : INode in terms)
				addTerm(node);
		}
		
		public function addTerm(node : INode) : void
		{
			_node = _node === NEUTRAL ? node : new Multiply(_node, node);
		}
	}
}
