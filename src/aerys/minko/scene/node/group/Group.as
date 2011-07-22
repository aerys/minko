package aerys.minko.scene.node.group
{
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.action.group.GroupAction;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.ISearchableScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.RenderingVisitor;
	
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	import flash.utils.getQualifiedClassName;

	/**
	 * The Group3D provides a basic support for scene building.
	 * A Group3D can contain any object implementing the IScene3D interface.
	 * 
	 * Group3D objects do not affect their children with any specific behaviour.
	 *
	 * @author Jean-Marc Le Roux
	 */
	public dynamic class Group extends Proxy implements IGroup
	{
		private static var _id		: uint				= 0;
		
		private var _name			: String			= null;
		
		private var _children		: Vector.<IScene>	= null;
		private var _numChildren	: int				= 0;
		
		private var _actions		: Vector.<IAction>	= Vector.<IAction>([GroupAction.groupAction]);
		
		public function get actions()	: Vector.<IAction>	{ return _actions; }
		public function get name()		: String			{ return _name; }
		
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
			if (!scene)
				throw new Error("Parameter child must be non-null.");
			
			_children.push(scene);
			++_numChildren;
			
			//scene.added(this);
			
			return this;
		}
		
		public function addChildAt(scene : IScene, position : uint) : IGroup
		{
			if (!scene)
				throw new Error("Parameter child must be non-null.");
			
			var numChildren : int = _children.length;
			
			if (position >= numChildren)
				return addChild(scene);
			
			for (var i : int = numChildren; i > position; --i)
				_children[i] = _children[int(i - 1)];
			_children[position] = scene;
			
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
			var numChildren : int = _children.length;
			var i : int	= 0;

			while (i < numChildren && _children[i] !== child)
				++i;
			
			if (i >= numChildren)
				return null;
			
			return removeChildAt(i);
		}
		
		public function removeChildAt(position : uint) : IGroup
		{
			var removed 	: IScene 	= null;
	
			if (position < _numChildren)
			{
				removed = _children[position];
				
				while (position < _numChildren - 1)
					_children[position] = _children[int(++position)];
				_children.length = --_numChildren;
			}
			
			return this;
		}
		
		public function removeAllChildren() : IGroup
		{
			_children.length = 0;
			_numChildren = 0;
			
			return this;
		}
		
		public function getChildAt(position : uint) : IScene
		{
			return position < _numChildren ? _children[position] : null;
		}
		
		public function swapChildren(child1	: IScene,
									 child2	: IScene) : IGroup
		{
			var id1	: int 	= getChildIndex(child1);
			var id2 : int	= getChildIndex(child2);
			
			if (id1 == -1 || id2 == -1)
				return this;
			
			var tmp : IScene = _children[id2];
			
			_children[id2] = _children[id1];
			_children[id1] = tmp;
			
			return this;
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
			
			if (index == name)
			{
				if (index < numChildren)
				{
					removeChildAt(index);
					addChildAt(value, index);
				}
			}
			else
			{
				var old : IScene = getChildByName(name);
				
				addChild(value);
				
				if (old)
				{
					swapChildren(value, old);
					_children.length = _children.length - 1;
				}
			}
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
	}
}