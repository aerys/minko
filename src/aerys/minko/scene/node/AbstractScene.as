package aerys.minko.scene.node
{
	import aerys.minko.scene.Visitor;
	import aerys.minko.type.Signal;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.utils.getQualifiedClassName;

	public class AbstractScene extends EventDispatcher implements IScene
	{
		private static var _id	: uint		= 0;

		private var _name		: String	= null;
		private var _parent		: Group		= null;
		
		private var _added		: Signal	= new Signal();
		private var _removed	: Signal	= new Signal();

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

		public function AbstractScene()
		{
			_name = getDefaultSceneName(this);
		}

		public function visit(visitor : Visitor) : void
		{
			// nothing
		}
		
		override public function dispatchEvent(event : Event) : Boolean
		{
			if (willTrigger(event.type))
				return super.dispatchEvent(event);
			
			return false;
		}
		
		public static function getDefaultSceneName(scene : IScene) : String
		{
			var className : String = getQualifiedClassName(scene);

			return className.substr(className.lastIndexOf(":") + 1)
				   + "_" + (++_id);
		}
	}
}