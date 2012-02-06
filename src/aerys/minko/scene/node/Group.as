package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.Visitor;
	import aerys.minko.type.Signal;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.events.Event;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	import flash.utils.getQualifiedClassName;

	/**
	 * The Group provides a basic support for scene building.
	 * A Group can contain any object implementing the IScene interface.
	 *
	 * Group objects do not affect their children with any specific behaviour.
	 *
	 * @author Jean-Marc Le Roux
	 */
	public dynamic class Group extends Proxy implements IScene
	{
		use namespace minko_scene;
		
		private static var _id		: uint				= 0;
		
		minko_scene var _children	: Vector.<IScene>	= null;

		private var _name			: String			= null;
		private var _parent			: Group				= null;
		private var _numChildren	: int				= 0;
		
		private var _transform		: Matrix4x4			= new Matrix4x4();
		private var _localToWorld	: Matrix4x4			= new Matrix4x4();
		
		private var _added			: Signal			= new Signal();
		private var _removed		: Signal			= new Signal();

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

		/**
		 * The number of children.
		 */
		public function get numChildren() : uint
		{
			return _numChildren;
		}
		
		/**
		 * The Matrix4x4 object defining the transform of the object into world-space.
		 */
		public function get transform() : Matrix4x4
		{
			return _transform;
		}
		
		public function get localToWorld() : Matrix4x4
		{
			return _localToWorld;
		}
		
		public function get added() : Signal
		{
			return _added;
		}

		public function get removed() : Signal
		{
			return _removed;
		}
		
		public function Group(...children)
		{
			super();

			initialize(children);
		}

		private function initialize(children : Array) : void
		{
			_name = AbstractScene.getDefaultSceneName(this);
			_children = new Vector.<IScene>();

			while (children.length == 1 && children[0] is Array)
				children = children[0];

			var childrenNum : uint = children.length;
			
			for (var childrenIndex : uint = 0; childrenIndex < childrenNum; ++childrenIndex)
				addChild(IScene(children[childrenIndex]));
			
			_added.add(addedHandler);
			_removed.add(removedHandler);
			_transform.changed.add(transformChangedHandler);
		}
		
		private function addedHandler(child : IScene, parent : Group) : void
		{
			transformChangedHandler();
			parent.transform.changed.add(transformChangedHandler);
		}
		
		private function removedHandler(child : IScene, parent : Group) : void
		{
			transformChangedHandler();
			parent.transform.changed.remove(transformChangedHandler);
		}
		
		private function transformChangedHandler(transform 	: Matrix4x4	= null,
												 key 		: String	= null) : void
		{
			if (_parent)
				Matrix4x4.multiply(_transform, _parent.transform, _localToWorld);
			else
				Matrix4x4.copy(_transform, _localToWorld);
		}
		
		public function contains(scene : IScene) : Boolean
		{
			return getChildIndex(scene) >= 0;
		}

		public function getChildIndex(child : IScene) : int
		{
			if (child == null)
				throw new Error("The 'child' parameter cannot be null.");
			
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
		public function addChild(scene : IScene) : Group
		{
			return addChildAt(scene, _numChildren);
		}

		public function addChildAt(scene : IScene, position : uint) : Group
		{
			if (!scene)
				throw new Error("Parameter 'scene' must not be null.");

			if (position < _numChildren)
				for (var i : int = _numChildren; i > position; --i)
					_children[i] = _children[int(i - 1)];
			else
				position = _numChildren;

			_children[position] = scene;
			scene.parent = this;

			++_numChildren;

			return this;
		}

		/**
		 * Remove a child from the container.
		 *
		 * @param	myChild The child to remove.
		 * @return Whether the child was actually removed or not.
		 */
		public function removeChild(child : IScene) : Group
		{
			var childIndex : int = getChildIndex(child);

			if (childIndex == -1)
				throw new Error('The scene node is not a child of the caller.');

			return removeChildAt(childIndex);
		}

		public function removeChildAt(position : uint) : Group
		{
			if (position < _numChildren)
			{
				var scene	: IScene	= _children.splice(position, 1)[0];

				scene.parent = null;

				--_numChildren;
			}

			return this;
		}

		public function removeAllChildren() : Group
		{
			while (_numChildren)
				removeChildAt(0);

			return this;
		}

		public function getChildAt(position : uint) : IScene
		{
			return position < _numChildren ? _children[position] : null;
		}

		public function getDescendantsByName(name 			: String,
											 descendants 	: Vector.<IScene> = null) : Vector.<IScene>
		{
			descendants ||= new Vector.<IScene>();
			
			var descendant 	: IScene 	= getChildByName(name);
			var numChildren	: int 		= numChildren;
			
			if (descendants)
				descendants.push(descendants);

			for (var i : int = 0; i < numChildren && !descendant; ++i)
			{
				var group : Group = _children[i] as Group;

				if (group)
					descendant = group.getDescendantByName(name, descendants);
			}

			return descendants;
		}

		public function getDescendantsByType(type 			: Class,
											 descendants 	: Vector.<IScene> = null) : Vector.<IScene>
		{
			var numChildren	: int 	= numChildren;

			descendants ||= new Vector.<IScene>();

			for (var i : int = 0; i < numChildren; ++i)
			{
				var child	: IScene	= _children[i];
				var group 	: Group 	= child as Group;

				if (child is type)
					descendants.push(child);

				if (group)
					group.getDescendantsByType(type, descendants);
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
			return getDescendantsByName(name);
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
		
		public function visit(visitor : Visitor) : void
		{
			for (var i : int = 0; i < _numChildren; ++i)
				visitor.visit(_children[i]);
		}
	}
}