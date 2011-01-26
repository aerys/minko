package aerys.minko.scene
{
	import aerys.minko.query.IScene3DQuery;
	
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
		
		public function accept(query : IScene3DQuery) : void
		{
			throw new Error();
		}
	}
}