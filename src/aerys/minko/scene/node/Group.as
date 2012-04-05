package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.SceneIterator;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.type.Signal;
	import aerys.minko.type.loader.ILoader;
	import aerys.minko.type.loader.SceneLoader;
	import aerys.minko.type.loader.parser.ParserOptions;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	import flash.utils.getQualifiedClassName;

	/**
	 * Group objects can contain other scene nodes and applies a 3D
	 * transformation to its descendants.
	 *
	 * @author Jean-Marc Le Roux
	 */
	public dynamic class Group extends AbstractSceneNode
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
		private var _worldToLocal		: Matrix4x4						= new Matrix4x4();
		
		private var _controllers		: Vector.<AbstractController>	= new <AbstractController>[];
		
		private var _added				: Signal						= new Signal('Group.added');
		private var _removed			: Signal						= new Signal('Group.removed');
		private var _addedToScene		: Signal						= new Signal('Group.addedToScene');
		private var _removedFromScene	: Signal						= new Signal('Group.removedFromScene');
		private var _descendantAdded	: Signal						= new Signal('Group.descendantAdded');
		private var _descendantRemoved	: Signal						= new Signal('Group.descendantRemoved');
		private var _controllerAdded	: Signal						= new Signal('Group.controllerAdded');
		private var _controllerRemoved	: Signal						= new Signal('Group.controllerRemoved');

		override public function set parent(value : Group) : void
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
				oldParent._descendantRemoved.execute(oldParent, this);
				
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
				_parent._descendantAdded.execute(_parent, this);
				
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
		
		public function get descendantAdded() : Signal
		{
			return _descendantAdded
		}
		
		public function get descendantRemoved() : Signal
		{
			return _descendantRemoved;
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

			added.add(addedHandler);
			removed.add(removedHandler);
			transform.changed.add(transformChangedHandler);
			descendantAdded.add(descendantAddedHandler);
			descendantRemoved.add(descendantRemovedHandler);
		}
		
		protected function initializeChildren(children : Array) : void
		{
			while (children.length == 1 && children[0] is Array)
				children = children[0];
			
			var childrenNum : uint = children.length;
			
			for (var childrenIndex : uint = 0; childrenIndex < childrenNum; ++childrenIndex)
				addChild(ISceneNode(children[childrenIndex]));
		}
		
		override protected function addedHandler(child : ISceneNode, parent : Group) : void
		{
			super.addedHandler(child, parent);
			
			for (var childIndex : int = 0; childIndex < _numChildren; ++childIndex)
				_children[childIndex].added.execute(child, parent);
		}
		
		override protected function removedHandler(child : ISceneNode, parent : Group) : void
		{
			super.removedHandler(child, parent);
			
			for (var childIndex : int = 0; childIndex < _numChildren; ++childIndex)
				_children[childIndex].removed.execute(child, parent);
		}
		
		private function descendantAddedHandler(group : Group, child : ISceneNode) : void
		{
			if (group == this)
			{
				var childGroup : Group = child as Group;
				
				if (childGroup)
				{
					childGroup.descendantAdded.add(_descendantAdded.execute);
					childGroup.descendantRemoved.add(_descendantRemoved.execute);
				}
			}
			
			_numDescendants += (child is Group) ? (child as Group)._numDescendants + 1 : 1;
		}
		
		private function descendantRemovedHandler(group : Group, child : ISceneNode) : void
		{
			if (group == this)
			{
				var childGroup : Group = child as Group;
				
				if (childGroup)
				{
					childGroup.descendantAdded.remove(_descendantAdded.execute);
					childGroup.descendantRemoved.remove(_descendantRemoved.execute);
				}
			}
			
			_numDescendants -= (child is Group) ? (child as Group)._numDescendants + 1 : 1;
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

		public function load(request	: URLRequest,
							 options	: ParserOptions) : ILoader
		{
			var loader	: SceneLoader	= new SceneLoader(options);
			
			loader.complete.add(loaderCompleteHandler);
			loader.load(request);
			
			return loader;
		}
		
		public function loadClass(classObject	: Class,
								  options		: ParserOptions) : ILoader
		{
			var loader	: SceneLoader	= new SceneLoader(options);
			
			loader.complete.add(loaderCompleteHandler);
			loader.loadClass(classObject);
			
			return loader;
		}
		
		public function loadBytes(bytes		: ByteArray,
								  options	: ParserOptions) : ILoader
		{
			var loader	: SceneLoader	= new SceneLoader(options);
			
			loader.complete.add(loaderCompleteHandler);
			loader.loadBytes(bytes);
			
			return loader;
		}
		
		public function get(path : String) : SceneIterator
		{
			return new SceneIterator(path, new <ISceneNode>[this]);
		}
		
		private function loaderCompleteHandler(loader	: ILoader,
											   scene	: ISceneNode) : void
		{
			addChild(scene);
		}
		
		override public function clone(cloneControllers : Boolean = false) : ISceneNode
		{
			var cloned : Group = new Group();
			
			cloned.name = name;
			cloned.transform.copyFrom(transform);
			
			for (var childId : uint = 0; childId < _numChildren; ++childId)
				cloned.addChildAt(getChildAt(childId).clone(), childId);
			
			copyControllersFrom(this, cloned, cloneControllers);
			
			return cloned;
		}
	}
}