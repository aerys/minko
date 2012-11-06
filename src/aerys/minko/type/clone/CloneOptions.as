package aerys.minko.type.clone
{
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.AnimationController;
	import aerys.minko.scene.controller.TransformController;
	import aerys.minko.scene.controller.camera.CameraController;
	import aerys.minko.scene.controller.mesh.MeshVisibilityController;
	import aerys.minko.scene.controller.mesh.skinning.SkinningController;

	public class CloneOptions
	{
		private var _clonedControllerTypes		: Vector.<Class>	= new <Class>[];
		private var _ignoredControllerTypes		: Vector.<Class>	= new <Class>[];
		private var _reassignedControllerTypes	: Vector.<Class>	= new <Class>[];
		private var _defaultControllerAction	: uint				= ControllerCloneAction.REASSIGN;
		
		public function get defaultControllerAction() : uint
		{
			return _defaultControllerAction;
		}
		public function set defaultControllerAction(v : uint) : void
		{
			if (v != ControllerCloneAction.CLONE 
				&& v != ControllerCloneAction.IGNORE 
				&& v != ControllerCloneAction.REASSIGN)
				throw new Error('Unknown action type.');
			
			_defaultControllerAction = v;
		}
		
		public function CloneOptions()
		{
		}
		
		public static function get defaultOptions() : CloneOptions
		{
			var cloneOptions : CloneOptions = new CloneOptions();
			
			cloneOptions._clonedControllerTypes.push(
				AnimationController,
				SkinningController
			);
			cloneOptions._ignoredControllerTypes.push(
				MeshVisibilityController,
				CameraController,
				TransformController
			);
			cloneOptions._defaultControllerAction = ControllerCloneAction.REASSIGN;
			
			return cloneOptions;
		}
		
		public static function get cloneAllOptions() : CloneOptions
		{
			var cloneOptions : CloneOptions = new CloneOptions();
			
			cloneOptions._ignoredControllerTypes.push(
				MeshVisibilityController,
				CameraController,
				TransformController
			);
			cloneOptions._defaultControllerAction = ControllerCloneAction.CLONE;
			
			return cloneOptions;
		}
		
		public function addControllerAction(controllerClass : Class, action : uint) : void
		{
			switch (action)
			{
				case ControllerCloneAction.CLONE:
					_clonedControllerTypes.push(controllerClass);
					break;
				
				case ControllerCloneAction.IGNORE:
					_ignoredControllerTypes.push(controllerClass);
					break;
				
				case ControllerCloneAction.REASSIGN:
					_reassignedControllerTypes.push(controllerClass);
					break;
				
				default:
					throw new Error('Unknown action type.');
			}
		}
		
		public function removeControllerAction(controllerClass : Class) : void
		{
			throw new Error('Implement me.');
		}
		
		public function getActionForController(controller : AbstractController) : uint
		{
			var numControllersToClone		: uint = _clonedControllerTypes.length;
			var numControllersToIgnore		: uint = _ignoredControllerTypes.length;
			var numControllersToReassign	: uint = _reassignedControllerTypes.length;
			var controllerId				: uint;
			
			for (controllerId = 0; controllerId < numControllersToClone; ++controllerId)
				if (controller is _clonedControllerTypes[controllerId])
					return ControllerCloneAction.CLONE;
			
			for (controllerId = 0; controllerId < numControllersToIgnore; ++controllerId)
				if (controller is _ignoredControllerTypes[controllerId])
					return ControllerCloneAction.IGNORE;
			
			for (controllerId = 0; controllerId < numControllersToReassign; ++controllerId)
				if (controller is _reassignedControllerTypes[controllerId])
					return ControllerCloneAction.REASSIGN;
			
			return _defaultControllerAction;
		}
		
	}
}
