package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.type.Signal;
	import aerys.minko.type.math.Matrix4x4;
	
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
	public dynamic class Group extends Proxy implements ISceneNode
	{
		use namespace minko_scene;
		
		private static var _id			: uint							= 0;
		
		minko_scene var _children		: Vector.<ISceneNode>			= null;
		minko_scene var _numChildren	: uint							= 0;

		private var _name				: String						= null;
		private var _root				: ISceneNode					= null;
		private var _parent				: Group							= null;
		
		private var _numDescendants		: uint							= 0;
		private var _transform			: Matrix4x4						= new Matrix4x4();
		private var _localToWorld		: Matrix4x4						= new Matrix4x4();
		
		private var _controllers		: Vector.<AbstractController>	= new <AbstractController>[];
		
		private var _added				: Signal						= new Signal();
		private var _removed			: Signal						= new Signal();
		private var _addedToScene		: Signal						= new Signal();
		private var _removedFromScene	: Signal						= new Signal();
		private var _childAdded			: Signal						= new Signal();
		private var _childRemoved		: Signal						= new Signal();
		private var _controllerAdded	: Signal						= new Signal();
		private var _controllerRemoved	: Signal						= new Signal();

		public function get name() : String
		{
			return _name;
		}
		public function set name(value : String) : void
		{
			_name = value;
		}
		
		public function get root() : ISceneNode
		{
			return _root;
		}
		
		public function get parent() : Group
		{
			return _parent; 
		}
		public function set parent(value : Group) : void
		{
			// remove child
			if (_parent)
			{
				var oldParent : Group = _parent;
				
				oldParent._children.splice(
					oldParent.getChildIndex(this),
					1
				);
				parent._numChildren--;
				oldParent._childRemoved.execute(oldParent, this);
				
				_parent = null;
				_removed.execute(this, oldParent);
			}
			
			// set parent
			_parent = value;
			
			// add child
			if (_parent)
			{
				_parent._children[parent._numChildren] = this;
				_parent._numChildren++;
				_parent._childAdded.execute(_parent, this);
				
				_added.execute(this, _parent);
			}
		}

		/**
		 * The number of children.
		 */
		public function get numChildren() : uint
		{
			return _numChildren;
		}
		
		public function get numDescendants() : uint
		{
			return _numDescendants;
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
		
		public function get addedToScene() : Signal
		{
			return _addedToScene;
		}
		
		public function get removedFromScene() : Signal
		{
			return _removedFromScene;
		}
		
		public function get childAdded() : Signal
		{
			return _childAdded;
		}
		
		public function get childRemoved() : Signal
		{
			return _childRemoved;
		}
		
		public function get numControllers() : uint
		{
			return _controllers.length;
		}
		
		public function get controllerAdded() : Signal
		{
			return _controllerAdded;
		}
		
		public function get controllerRemoved() : Signal
		{
			return _controllerRemoved;
		}
		
		public function Group(...children)
		{
			super();

			initialize();
			initializeChildren(children);
		}

		protected function initialize() : void
		{
			_root = this;
			_name = AbstractSceneNode.getDefaultSceneName(this);
			_children = new <ISceneNode>[];

			_added.add(addedHandler);
			_removed.add(removedHandler);
			_transform.changed.add(transformChangedHandler);
			_childAdded.add(childAddedHandler);
			_childRemoved.add(childRemovedHandler);
		}
		
		private function initializeChildren(children : Array) : void
		{
			while (children.length == 1 && children[0] is Array)
				children = children[0];
			
			var childrenNum : uint = children.length;
			
			for (var childrenIndex : uint = 0; childrenIndex < childrenNum; ++childrenIndex)
				addChild(ISceneNode(children[childrenIndex]));
		}
		
		private function addedHandler(child : ISceneNode, parent : Group) : void
		{
			var oldRoot : ISceneNode = _root;
			
			_root = _parent ? _parent.root : this;
			if (_root is Scene)
				_addedToScene.execute(this, _root);
					
			if (child == this)
			{
				transformChangedHandler();
				parent.localToWorld.changed.add(transformChangedHandler);
			}
			
			for (var childIndex : int = 0; childIndex < _numChildren; ++childIndex)
				_children[childIndex].added.execute(child, parent);
		}
		
		private function removedHandler(child : ISceneNode, parent : Group) : void
		{
			if (child == this)
			{
				_root = this;
				transformChangedHandler();
				parent.localToWorld.changed.remove(transformChangedHandler);
			}
			else
			{
				var oldRoot : ISceneNode = _root;
				
				_root = _parent ? _parent.root : this;
				if (oldRoot is Scene)
					_removedFromScene.execute(this, oldRoot);
			}
			
			for (var childIndex : int = 0; childIndex < _numChildren; ++childIndex)
				_children[childIndex].removed.execute(child, parent);
		}
		
		private function childAddedHandler(group : Group, child : ISceneNode) : void
		{
			if (group == this)
			{
				var childGroup : Group = child as Group;
				
				if (childGroup)
				{
					childGroup.childAdded.add(_childAdded.execute);
					childGroup.childRemoved.add(_childRemoved.execute);
				}
			}
			
			_numDescendants += (child is Group) ? (child as Group)._numDescendants + 1 : 1;
		}
		
		private function childRemovedHandler(group : Group, child : ISceneNode) : void
		{
			if (group == this)
			{
				var childGroup : Group = child as Group;
				
				if (childGroup)
				{
					childGroup.childAdded.remove(_childAdded.execute);
					childGroup.childRemoved.remove(_childRemoved.execute);
				}
			}
			
			_numDescendants -= (child is Group) ? (child as Group)._numDescendants + 1 : 1;
		}
		
		private function transformChangedHandler(transform 	: Matrix4x4	= null,
												 key 		: String	= null) : void
		{
			if (_parent)
				Matrix4x4.multiply(_parent.localToWorld, _transform, _localToWorld);
			else
				Matrix4x4.copy(_transform, _localToWorld);
		}
		
		public function contains(scene : ISceneNode) : Boolean
		{
			return getChildIndex(scene) >= 0;
		}

		public function getChildIndex(child : ISceneNode) : int
		{
			if (child == null)
				throw new Error("The 'child' parameter cannot be null.");
			
			for (var i : int = 0; i < _numChildren; i++)
				if (_children[i] === child)
					return i;

			return -1;
		}

		public function getChildByName(name : String) : ISceneNode
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
		public function addChild(node : ISceneNode) : Group
		{
			return addChildAt(node, _numChildren);
		}

		public function addChildAt(node : ISceneNode, position : uint) : Group
		{
			if (!node)
				throw new Error("Parameter 'scene' must not be null.");

			node.parent = this;

			for (var i : int = _numChildren - 1; i > position; --i)
				_children[i] = _children[int(i - 1)];
			
			_children[position] = node;
			
			return this;
		}

		/**
		 * Remove a child from the container.
		 *
		 * @param	myChild The child to remove.
		 * @return Whether the child was actually removed or not.
		 */
		public function removeChild(child : ISceneNode) : Group
		{
			return removeChildAt(getChildIndex(child));
		}

		public function removeChildAt(position : uint) : Group
		{
			if (position >= _numChildren || position < 0)
				throw new Error('The scene node is not a child of the caller.');
			
			(_children[position] as ISceneNode).parent = null;

			return this;
		}

		public function removeAllChildren() : Group
		{
			while (_numChildren)
				removeChildAt(0);

			return this;
		}

		public function getChildAt(position : uint) : ISceneNode
		{
			return position < _numChildren ? _children[position] : null;
		}

		public function getDescendantsByName(name 			: String,
											 descendants 	: Vector.<ISceneNode> = null) : Vector.<ISceneNode>
		{
			descendants ||= new Vector.<ISceneNode>();
			
			var numChildren	: int 	= numChildren;
			
			for (var i : int = 0; i < numChildren; ++i)
			{
				var child : ISceneNode = _children[i];
				var group : Group = child as Group;
				
				if (child.name == name)
					descendants.push(child);

				if (group)
					group.getDescendantsByName(name, descendants);
			}

			return descendants;
		}

		public function getDescendantsByType(type 			: Class,
											 descendants 	: Vector.<ISceneNode> = null) : Vector.<ISceneNode>
		{
			descendants ||= new Vector.<ISceneNode>();

			for (var i : int = 0; i < _numChildren; ++i)
			{
				var child	: ISceneNode	= _children[i];
				var group 	: Group 		= child as Group;

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
		
		public function addController(controller : AbstractController) : void
		{
			_controllers.push(controller);

			controller.addTarget(this);
			_controllerAdded.execute(this, controller);
		}
		
		public function removeController(controller : AbstractController) : void
		{
			var numControllers	: uint = _controllers.length - 1;
			
			_controllers[_controllers.indexOf(controller)] = _controllers[numControllers];
			_controllers.length = numControllers;
			
			controller.removeTarget(this);
			_controllerRemoved.execute(this, controller);
		}
		
		public function getController(index : uint) : AbstractController
		{
			return _controllers[index];
		}
	}
}