package aerys.minko.scene
{
	import aerys.minko.render.IScene3DVisitor;
	
	public class AbstractScene3D implements IScene3D
	{
		protected var _name		: String		= null;
		
		public function get name() : String
		{
			return _name;
		}
		
		public function set name(value : String) : void
		{
			_name = value;
		}
		
		public function visited(visitor : IScene3DVisitor) : void
		{
			throw new Error();
		}
	}
}