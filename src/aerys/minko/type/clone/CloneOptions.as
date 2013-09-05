package aerys.minko.type.clone
{
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.TransformController;
	import aerys.minko.scene.controller.animation.IAnimationController;
	import aerys.minko.scene.controller.camera.CameraController;
	import aerys.minko.scene.controller.light.DirectionalLightController;
	import aerys.minko.scene.controller.light.LightController;
	import aerys.minko.scene.controller.light.PointLightController;
	import aerys.minko.scene.controller.light.SpotLightController;
	import aerys.minko.scene.controller.mesh.MeshController;
	import aerys.minko.scene.controller.mesh.MeshVisibilityController;
	import aerys.minko.scene.controller.mesh.skinning.SkinningController;

	public final class CloneOptions
	{
		private var _clonedControllerTypes		: Vector.<Class>;
		private var _ignoredControllerTypes		: Vector.<Class>;
		private var _reassignedControllerTypes	: Vector.<Class>;
		private var _defaultControllerAction	: uint;
		
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
			initialize();
		}
		
		private function initialize() : void
		{
			_clonedControllerTypes = new <Class>[];
			_ignoredControllerTypes = new <Class>[];
			_reassignedControllerTypes = new <Class>[];
			_defaultControllerAction = ControllerCloneAction.REASSIGN;
		}
		
		public static function get defaultOptions() : CloneOptions
		{
			var cloneOptions : CloneOptions = new CloneOptions();
			
			cloneOptions._clonedControllerTypes.push(
				IAnimationController,
				SkinningController
			);
			cloneOptions._ignoredControllerTypes.push(
				MeshController,
				MeshVisibilityController,
				CameraController,
				TransformController,
                LightController,
                DirectionalLightController,
                SpotLightController,
                PointLightController
			);
			cloneOptions._defaultControllerAction = ControllerCloneAction.REASSIGN;
						
			return cloneOptions;
		}
		
		public static function get cloneAllOptions() : CloneOptions
		{
			var cloneOptions : CloneOptions = new CloneOptions();
			
			cloneOptions._ignoredControllerTypes.push(
				MeshController,
				MeshVisibilityController,
				CameraController,
				TransformController,
                LightController,
                DirectionalLightController,
                SpotLightController,
                PointLightController
			);
			cloneOptions._defaultControllerAction = ControllerCloneAction.CLONE;
			
			return cloneOptions;
		}
		
		public function setActionForControllerClass(controllerClass	: Class,
													action			: uint) : CloneOptions
		{
			var index : int = 0;
			
			if ((index = _clonedControllerTypes.indexOf(controllerClass)) >= 0)
				_clonedControllerTypes.splice(index, 1);
			else if ((index = _ignoredControllerTypes.indexOf(controllerClass)) >= 0)
				_ignoredControllerTypes.splice(index, 1);
			else if ((index = _reassignedControllerTypes.indexOf(controllerClass)) >= 0)
				_reassignedControllerTypes.splice(index, 1);
			
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
			
			return this;
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
