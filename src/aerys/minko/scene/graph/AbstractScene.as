package aerys.minko.scene.graph
{
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	
	import flash.utils.getQualifiedClassName;
	
	public class AbstractScene implements IScene
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
		
		public function AbstractScene()
		{
			_name = getDefaultSceneName(this);
		}
		
		public static function getDefaultSceneName(scene : IScene) : String
		{
			var className : String = getQualifiedClassName(scene);
			
			return className.substr(className.lastIndexOf(":") + 1)
				   + "_" + (++_id);
		}
		
		public function visited(visitor : ISceneVisitor) : void
		{
			if (visitor is RenderingVisitor)
				visitedByRenderingVisitor(visitor as RenderingVisitor);
		}
		
		protected function visitedByRenderingVisitor(visitor : RenderingVisitor) : void
		{
			// NOTHING
		}
	}
}