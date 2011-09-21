package aerys.minko.scene.node.group
{
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.action.group.GroupAction;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.ISearchableScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.RenderingVisitor;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.net.getClassByAlias;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	import flash.utils.getDefinitionByName;
	import flash.utils.getQualifiedClassName;

	/**
	 * The Group provides a basic support for scene building.
	 * A Group can contain any object implementing the IScene interface.
	 * 
	 * Group objects do not affect their children with any specific behaviour.
	 *
	 * @author Jean-Marc Le Roux
	 */
	public dynamic class Group extends Proxy implements IGroup
	{
		private static var _id		: uint				= 0;
		
		private var _name			: String			= null;
		private var _parents		: Vector.<IScene>	= new Vector.<IScene>();
		private var _actions		: Vector.<IAction>	= Vector.<IAction>([GroupAction.groupAction]);
		
		private var _children		: Vector.<IScene>	= null;
		private var _numChildren	: int				= 0;
		
		private var _dispatcher		: EventDispatcher	= null;
		
		public function get name()		: String			{ return _name; }
		public function get parents()	: Vector.<IScene>	{ return _parents; }
		public function get actions()	: Vector.<IAction>	{ return _actions; }
		
		public function set name(value : String) : void
		{
			_name = value;
		}
		
		protected function get rawChildren()	: Vector.<IScene> 	{ return _children; }
		
		protected function set rawChildren(value : Vector.<IScene>) : void
		{
			_children = value;
			_numChildren = _children.length;
		}
		
		/**
		 * The number of children.
		 */
		public function get numChildren() : uint
		{
			return _numChildren;
		}
		
		public function Group(...children)
		{
			super();
			
			_dispatcher = new EventDispatcher(this);
			_name = AbstractScene.getDefaultSceneName(this);

			initialize(children);
		}
		
		private function initialize(children : Array) : void
		{
			_children = new Vector.<IScene>();

			while (children.length == 1 && children[0] is Array)
				children = children[0];

			var childrenNum : uint = children.length;
			for (var childrenIndex : uint = 0; childrenIndex < childrenNum; ++childrenIndex)
			{
				var child : IScene = children[childrenIndex] as IScene;

				if (!child)
					throw new Error("Constructor parameters must be IScene objects.");

				addChild(child);
			}
		}
		
		public function contains(scene : IScene) : Boolean
		{
			return getChildIndex(scene) >= 0;
		}
		
		public function getChildIndex(child : IScene) : int
		{
			for (var i : int = 0; i < _numChildren; i++)
				if (_children[i] === child)
					return i;
			
			return -1;
		}
		
		public function getChildByName(name : String) : IScene
		{
			for (var i : int = 0; i < _numChildren; i++)
				if (_children[i].name === name)
					return _children[i];
			
			return null;
		}
		
		/**
		 * Add a child to the container.
		 *
		 * @param	scene The child to add.
		 */
		public function addChild(scene : IScene) : IGroup
		{
			return addChildAt(scene, _numChildren);
		}
		
		public function addChildAt(scene : IScene, position : uint) : IGroup
		{
			if (!scene)
				throw new Error("Parameter child must be non-null.");
			
			if (position < _numChildren)
				for (var i : int = _numChildren; i > position; --i)
					_children[i] = _children[int(i - 1)];
			else
				position = _numChildren;
			
			_children[position] = scene;
			scene.parents.push(this);
			scene.dispatchEvent(new Event(Event.ADDED));
			
			++_numChildren;
			
			return this;
		}
		
		/**
		 * Remove a child from the container.
		 *
		 * @param	myChild The child to remove.
		 * @return Whether the child was actually removed or not.
		 */
		public function removeChild(child : IScene) : IGroup
		{
			var childIndex : int = getChildIndex(child);
			
			if (childIndex == -1)
				throw new Error('The scene node is not a child of the caller');
			
			return removeChildAt(childIndex);
		}
		
		public function removeChildAt(position : uint) : IGroup
		{
			if (position < _numChildren)
			{
				var removedParents 	: Vector.<IScene> 	= _children[position].parents;
				var scene			: IScene			= _children.splice(position, 1)[0];
				
				removedParents.splice(removedParents.indexOf(this), 1);

				scene.dispatchEvent(new Event(Event.REMOVED));
				
				--_numChildren;
			}
			
			return this;
		}
		
		public function removeAllChildren() : IGroup
		{
			while (_numChildren)
				removeChildAt(0);
				
			return this;
		}
		
		public function getChildAt(position : uint) : IScene
		{
			return position < _numChildren ? _children[position] : null;
		}
		
		public function getDescendantByName(name : String) : IScene
		{
			var descendant 	: IScene 	= getChildByName(name);
			var numChildren	: int 		= numChildren;
			
			for (var i : int = 0; i < numChildren && !descendant; ++i)
			{
				var searchable : ISearchableScene = _children[i] as ISearchableScene;
				
				if (searchable)
					descendant = searchable.getDescendantByName(name);
			}
			
			return descendant;
		}
		
		public function getDescendantsByType(type : Class, descendants : Vector.<IScene> = null) : Vector.<IScene>
		{		
			var numChildren	: int 		= numChildren;
			
			descendants ||= new Vector.<IScene>();
			
			for (var i : int = 0; i < numChildren; ++i)
			{
				var child		: IScene			= _children[i];
				var searchable 	: ISearchableScene 	= child as ISearchableScene;

				if (child is type)
					descendants.push(child);
				
				if (searchable)
					searchable.getDescendantsByType(type, descendants);
			}
			
			return descendants;
		}

		public function toString() : String
		{
			return "[" + getQualifiedClassName(this) + " " + name + "]";
		}
		
		override flash_proxy function getProperty(name : *) : *
		{			
			var index : int = parseInt(name);
			
			return index == name ? getChildAt(index) : getChildByName(name as String);
		}
		
		override flash_proxy function setProperty(name : *, value : *) : void
		{
			var index : int = parseInt(name);
			
			if (index != name || index > numChildren)
				throw new Error("'" + name + "' is not a valid child index.");
			
			if (index != numChildren)
				removeChildAt(index);
			
			addChildAt(value, index);
		}
		
		override flash_proxy function getDescendants(name : *) : *
		{
			return getDescendantByName(name);
		}
		
		override flash_proxy function nextNameIndex(index : int) : int
		{
			return index < numChildren ? index + 1 : 0;
		}
		
		override flash_proxy function nextName(index : int) : String
		{
			return String(index - 1);
		}
		
		override flash_proxy function nextValue(index : int) : *
		{
			return _children[int(index - 1)];
		}
		
		public function addEventListener(type				: String,
										 listener			: Function,
										 useCapture 		: Boolean	= false,
										 priority			: int		= 0,
										 useWeakReference	: Boolean	= false) : void
		{
			_dispatcher.addEventListener(type, listener, useCapture, priority, useWeakReference);
		}
		
		public function removeEventListener(type		: String,
											listener	: Function,
											useCapture	: Boolean	= false) : void
		{
			_dispatcher.removeEventListener(type, listener, useCapture);
		}
		
		public function dispatchEvent(event : Event) : Boolean
		{
			return willTrigger(event.type) && dispatchEvent(event);
		}
		
		public function hasEventListener(type : String) : Boolean
		{
			return _dispatcher.hasEventListener(type);
		}
		
		public function willTrigger(type : String) : Boolean
		{
			return _dispatcher.willTrigger(type);
		}
	}
}