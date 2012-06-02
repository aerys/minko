package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.data.TransformDataProvider;
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
		
		private var _transformData		: TransformDataProvider			= new TransformDataProvider();
		private var _transform			: Matrix4x4						= new Matrix4x4();
		
		private var _controllers		: Vector.<AbstractController>	= new <AbstractController>[];
		
		private var _added				: Signal						= new Signal('AbstractSceneNode.added');
		private var _removed			: Signal						= new Signal('AbstractSceneNode.removed');
		private var _addedToScene		: Signal						= new Signal('AbstractSceneNode.addedToScene');
		private var _removedFromScene	: Signal						= new Signal('AbstractSceneNode.removedFromScene');
		private var _controllerAdded	: Signal						= new Signal('AbstractSceneNode.controllerAdded');
		private var _controllerRemoved	: Signal						= new Signal('AbstractSceneNode.controllerRemoved');

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
				oldParent.descendantRemoved.execute(oldParent, this);
				
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
				_parent.descendantAdded.execute(_parent, this);
				
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
			return _transformData.localToWorld;
		}
		
		public function get worldToLocal() : Matrix4x4
		{
			return _transformData.worldToLocal;
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
		
		protected function get transformData() : TransformDataProvider
		{
			return _transformData;
		}
		
		public function AbstractSceneNode()
		{
			initialize();
		}
		
		private function initialize() : void
		{
			_name = getDefaultSceneName(this);
			_root = this;
			
			_added.add(addedHandler);
			_removed.add(removedHandler);
			_addedToScene.add(addedToSceneHandler);
			_removedFromScene.add(removedFromSceneHandler);
			
			_transform.changed.add(transformChangedHandler);
		}
		
		protected function addedHandler(child : ISceneNode, parent : Group) : void
		{
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
		
		protected function transformChangedHandler(transform	: Matrix4x4,
												   propertyName	: String) : void
		{
			if (_parent)
			{
				localToWorld.lock()
					.copyFrom(_transform)
					.append(_parent.localToWorld)
					.unlock();
			}
			else
				localToWorld.copyFrom(_transform);
			
			worldToLocal.lock()
				.copyFrom(localToWorld)
				.invert()
				.unlock();
		}
		
		public function addController(controller : AbstractController) : ISceneNode
		{
			_controllers.push(controller);
			
			controller.addTarget(this);
			_controllerAdded.execute(this, controller);
			
			return this;
		}
		
		public function removeController(controller : AbstractController) : ISceneNode
		{
			var numControllers	: uint = _controllers.length - 1;
			
			_controllers[_controllers.indexOf(controller)] = _controllers[numControllers];
			_controllers.length = numControllers;
			
			controller.removeTarget(this);
			_controllerRemoved.execute(this, controller);
			
			return this;
		}
		
		public function removeAllControllers() : ISceneNode
		{
			while (numControllers)
				removeController(getController(0));
			
			return this;
		}
		
		public function getController(index : uint) : AbstractController
		{
			return _controllers[index];
		}
		
		public function getControllersByType(type			: Class,
											 controllers	: Vector.<AbstractController> = null) : Vector.<AbstractController>
		{
			controllers ||= new Vector.<AbstractController>();
			
			var nbControllers : uint = numControllers;
			
			for (var i : int = 0; i < nbControllers; ++i)
			{
				var ctrl 	: AbstractController	= getController(i);
				
				if (ctrl is type)
					controllers.push(ctrl);
			}
			
			return controllers;
		}
		
		public static function getDefaultSceneName(scene : ISceneNode) : String
		{
			var className : String = getQualifiedClassName(scene);

			return className.substr(className.lastIndexOf(':') + 1)
				   + '_' + (++_id);
		}
		
		public function clone(cloneControllers : Boolean = false) : ISceneNode
		{
			throw new Error('The method AbstractSceneNod.clone() must be overriden.');
		}
		
		protected function copyControllersFrom(source 			: ISceneNode,
											   target			: ISceneNode,
											   cloneControllers	: Boolean) : void
		{
			var numControllers : uint = target.numControllers;
			
			while (numControllers)
				target.removeController(target.getController(--numControllers));
			
			numControllers = source.numControllers;
			for (var controllerId : uint = 0; controllerId < numControllers; ++controllerId)
			{
				var controller : AbstractController = source.getController(controllerId);
				
				if (cloneControllers)
					controller = controller.clone();
				
				target.addController(controller);
			}
		}
	}
}