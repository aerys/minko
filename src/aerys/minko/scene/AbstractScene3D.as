package aerys.minko.scene
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.RenderingQuery;
	
	import flash.utils.getQualifiedClassName;
	
	public class AbstractScene3D implements IScene3D
	{
		private static var _id	: uint			= 0;
		
		protected var _name		: String		= null;
		
		public function get name() : String
		{
			return _name;
		}
		
		public function set name(value : String) : void
		{
			_name = value;
		}
		
		public function AbstractScene3D()
		{
			var className : String = getQualifiedClassName(this);
			
			_name = className.substr(className.lastIndexOf(":") + 1) + "_" + (++_id);
		}
		
		public function accept(query : IScene3DQuery) : void
		{
			if (query is RenderingQuery)
				acceptRenderingQuery(query as RenderingQuery);
		}
		
		protected function acceptRenderingQuery(query : RenderingQuery) : void
		{
			// NOTHING
		}
	}
}