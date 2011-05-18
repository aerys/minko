package aerys.minko.render.shader.node.leaf
{
	public class AbstractConstant extends AbstractLeaf
	{
		protected var _name : String;
		
		public function AbstractConstant(name : String)
		{
		}
		
		public function clone() : AbstractConstant
		{
			throw new Error('Must be overriden');
		}
	}
}
