package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.type.Signal;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.utils.getQualifiedClassName;

	/**
	 * The base class to extend in order to create new scene node types.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class AbstractSceneNode implements ISceneNode
	{
		use namespace minko_scene;
		
		private static var _id			: uint							= 0;

		private var _name				: String						= null;
		private var _root				: ISceneNode					= null;
		private var _parent				: Group							= null;
		
		private var _transform			: Matrix4x4						= new Matrix4x4();
		private var _localToWorld		: Matrix4x4						= new Matrix4x4();
		private var _worldToLocal		: Matrix4x4						= new Matrix4x4();
		
		private var _controllers		: Vector.<AbstractController>	= new <AbstractController>[];
		
		private var _added				: Signal						= new Signal();
		private var _removed			: Signal						= new Signal();
		private var _addedToScene		: Signal						= new Signal();
		private var _removedFromScene	: Signal						= new Signal();
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
				oldParent.childRemoved.execute(oldParent, this);
				
				_parent = null;
				_removed.execute(this, oldParent);
			}
			
			// set parent
			_parent = value;
			
			// add child
			if (_parent)
			{
				_parent._children[_parent.numChildren] = this;
				_parent._numChildren++;
				_parent.childAdded.execute(_parent, this);
				
				_added.execute(this, _parent);
			}
		}
		
		public function get root() : ISceneNode
		{
			return _root;
		}
		
		public function get transform() : Matrix4x4
		{
			return _transform;
		}
		
		public function get localToWorld() : Matrix4x4
		{
			return _localToWorld;
		}
		
		public function get worldToLocal() : Matrix4x4
		{
			return _worldToLocal;
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
			
			_transform.changed.add(transformChangedHandler);
		}

		protected function addedHandler(child : ISceneNode, parent : Group) : void
		{
			// update root
			var oldRoot : ISceneNode = _root;
			
			_root = _parent ? _parent.root : this;
			if (_root is Scene)
				_addedToScene.execute(this, _root);
			
			if (child === this)
			{
				_parent.localToWorld.changed.add(transformChangedHandler);
				transformChangedHandler(_parent.transform, null);
			}
		}
		
		protected function removedHandler(child : ISceneNode, parent : Group) : void
		{
			// update root
			var oldRoot : ISceneNode = _root;
			
			_root = _parent ? _parent.root : this;
			if (oldRoot is Scene)
				_removedFromScene.execute(this, oldRoot);
			
			if (child === this)
				parent.localToWorld.changed.remove(transformChangedHandler);
		}
		
		protected function addedToSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			// nothing
		}
		
		protected function removedFromSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			// nothing
		}
		
		private function transformChangedHandler(transform		: Matrix4x4,
												 propertyName	: String) : void
		{
			if (_parent)
				Matrix4x4.multiply(_parent.localToWorld, _transform, _localToWorld);
			else
				Matrix4x4.copy(_transform, _localToWorld);
			
			_worldToLocal.copyFrom(_localToWorld).invert();
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
		
		public static function getDefaultSceneName(scene : ISceneNode) : String
		{
			var className : String = getQualifiedClassName(scene);

			return className.substr(className.lastIndexOf(":") + 1)
				   + "_" + (++_id);
		}
	}
}