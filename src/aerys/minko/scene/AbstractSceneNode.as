package aerys.minko.scene
{
	import aerys.minko.type.Signal;
	
	import flash.utils.getQualifiedClassName;

	public class AbstractSceneNode implements ISceneNode
	{
		private static var _id			: uint		= 0;

		private var _name				: String	= null;
		private var _root				: Group		= null;
		private var _parent				: Group		= null;
		
		private var _added				: Signal	= new Signal();
		private var _removed			: Signal	= new Signal();
		private var _visited			: Signal	= new Signal();
		private var _addedToScene		: Signal	= new Signal();
		private var _removedFromScene	: Signal	= new Signal();

		public function get name() : String
		{
			return _name;
		}
		public function set name(value : String) : void
		{
			_name = value;
		}
		
		public function get parent() : Group
		{
			return _parent;
		}
		public function set parent(value : Group) : void
		{
			if (_parent)
			{
				var oldParent : Group = _parent;
				
				_parent = null;
				_removed.execute(this, oldParent);
			}
			
			_parent = value;
			
			if (_parent)
				_added.execute(this, _parent);
		}
		
		public function get root() : Group
		{
			return _root;
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
		
		public function get addedToScene() : Signal
		{
			return _addedToScene;
		}
		
		public function get removedFromScene() : Signal
		{
			return _removedFromScene;
		}
		
		public function AbstractSceneNode()
		{
			initialize();
		}
		
		private function initialize() : void
		{
			_name = getDefaultSceneName(this);
			
			_added.add(addedHandler);
			_removed.add(removedHandler);
			_addedToScene.add(addedToSceneHandler);
			_removedFromScene.add(removedFromSceneHandler);
		}

		protected function addedHandler(child : ISceneNode, parent : Group) : void
		{
			var oldRoot : Group = _root;
			
			_root = parent.root;
			if (_root is Scene)
				_addedToScene.execute(this, _root);
		}
		
		protected function removedHandler(child : ISceneNode, parent : Group) : void
		{
			_root = parent.root;
			_removedFromScene.execute(this, _root);
		}
		
		protected function addedToSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			// nothing
		}
		
		protected function removedFromSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			// nothing
		}
		
		public static function getDefaultSceneName(scene : ISceneNode) : String
		{
			var className : String = getQualifiedClassName(scene);

			return className.substr(className.lastIndexOf(":") + 1)
				   + "_" + (++_id);
		}
	}
}