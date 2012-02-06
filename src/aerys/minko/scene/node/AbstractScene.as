package aerys.minko.scene.node
{
	import aerys.minko.type.Signal;
	
	import flash.utils.getQualifiedClassName;

	public class AbstractScene implements IScene
	{
		private static var _id	: uint		= 0;

		private var _name		: String	= null;
		private var _parent		: Group		= null;
		
		private var _added		: Signal	= new Signal();
		private var _removed	: Signal	= new Signal();
		private var _visited	: Signal	= new Signal();

		public function get parent() : Group
		{
			return _parent;
		}
		public function set parent(value : Group) : void
		{
			if (_parent)
				_removed.execute(this, _parent);
			
			_parent = value;
			
			if (_parent)
				_added.execute(this, _parent);
		}
		
		public function get name() : String
		{
			return _name;
		}
		public function set name(value : String) : void
		{
			_name = value;
		}
		
		public function get added() : Signal
		{
			return _added;
		}
		
		public function get removed() : Signal
		{
			return _removed;
		}

		public function get visited() : Signal
		{
			return _visited;
		}
		
		public function AbstractScene()
		{
			initialize();
		}
		
		private function initialize() : void
		{
			_name = getDefaultSceneName(this);
			
			_added.add(addedHandler);
			_removed.add(removedHandler);
		}

		protected function addedHandler(child : IScene, parent : Group) : void
		{
			// nothing
		}
		
		protected function removedHandler(child : IScene, parent : Group) : void
		{
			// nothing
		}
		
		public static function getDefaultSceneName(scene : IScene) : String
		{
			var className : String = getQualifiedClassName(scene);

			return className.substr(className.lastIndexOf(":") + 1)
				   + "_" + (++_id);
		}
	}
}