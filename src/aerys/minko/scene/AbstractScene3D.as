package aerys.minko.scene
{
	import aerys.minko.query.IScene3DQuery;
//	import aerys.minko.query.RenderingQueryOld;
	
	import flash.utils.getQualifiedClassName;
	import aerys.minko.scene.interfaces.IScene3D;
	
	public class AbstractScene3D implements IScene3D
	{
		private static var _id	: uint			= 0;
		
		private var _name		: String		= null;
		
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
			_name = getDefaultSceneName(this);
		}
		
		public static function getDefaultSceneName(scene : IScene3D) : String
		{
			var className : String = getQualifiedClassName(scene);
			
			return className.substr(className.lastIndexOf(":") + 1)
				   + "_" + (++_id);
		}
		
//		public function accept(query : IScene3DQuery) : void
//		{
//			if (query is RenderingQueryOld)
//				acceptRenderingQuery(query as RenderingQueryOld);
//		}
		
//		protected function acceptRenderingQuery(query : RenderingQueryOld) : void
//		{
//			// NOTHING
//		}
	}
}