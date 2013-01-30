package aerys.minko.scene.controller.light
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.TransformController;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.light.AbstractLight;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.display.BitmapData;

	public class LightController extends AbstractController
	{
		private var _light 		    : AbstractLight;
		private var _lightData	    : LightDataProvider;
        private var _localToWorld   : Matrix4x4;
        private var _worldToLocal   : Matrix4x4;
		
		public function get lightData() : LightDataProvider
		{
			return _lightData;
		}
		
		protected function get light() : AbstractLight
		{
			return _light;
		}
		
		public function LightController(lightClass : Class)
		{
			super(lightClass);
			
			initialize();
		}
		
		private function initialize() : void
		{
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		protected function targetAddedHandler(ctrl	: LightController,
											 light	: AbstractLight) : void
		{
            _localToWorld = new Matrix4x4();
            _worldToLocal = new Matrix4x4();
            
			_lightData = new LightDataProvider(-1);
			_lightData.setLightProperty('type', light.type);
			_lightData.setLightProperty('enabled', true);
			_lightData.propertyChanged.add(lightDataChangedHandler);
			
			light.added.add(addedHandler);
			light.removed.add(removedHandler);
			
			_light = light;
		}
		
		protected function targetRemovedHandler(ctrl	: LightController,
											   light	: AbstractLight) : void
		{
			throw new Error();
		}
		
		protected function addedHandler(light		: AbstractLight,
										ancestor	: Group) : void
		{
			var scene : Scene = light.scene;
			
			if (scene)
				lightAddedToScene(scene);
		}
		
		protected function lightAddedToScene(scene : Scene) : void
		{
            var transformController : TransformController = scene.getControllersByType(TransformController)[0]
                as TransformController;
            
            _localToWorld = transformController.getLocalToWorldTransform(_light, true);
            _worldToLocal = transformController.getWorldToLocalTransform(_light, true);
            transformController.synchronizeTransforms(_light, true);
            transformController.lockTransformsBeforeUpdate(_light, true);
            
            _lightData.setLightProperty('localToWorld', _localToWorld);
            _lightData.setLightProperty('worldToLocal', _worldToLocal);
            
			sortLights(scene);
		}
		
		protected function removedHandler(light		: AbstractLight,
										  ancestor	: Group) : void
		{
			var scene : Scene = ancestor.scene;
			
			if (scene)
				lightRemovedFromScene(scene);
		}
		
		protected function lightRemovedFromScene(scene : Scene) : void
		{
            _localToWorld = null;
            _worldToLocal = null;
            
			sortLights(scene);
		}
		
		protected function lightDataChangedHandler(lightData		: LightDataProvider,
												   propertyName		: String,
												   bindingName		: String,
												   value			: Object) : void
		{
			// nothing
		}
		
		private function updateDataProvider(lightId : int) : void
		{
			var newDataProvider : LightDataProvider = new LightDataProvider(lightId);
			
			for (var propertyName : String in _lightData.dataDescriptor)
			{
				propertyName = LightDataProvider.getPropertyName(propertyName);
				
				newDataProvider.setLightProperty(
					propertyName,
					_lightData.getLightProperty(propertyName)
				);
			}
			
            if (_lightData.propertyChanged.hasCallback(lightDataChangedHandler))
            {
    			_lightData.propertyChanged.remove(lightDataChangedHandler);
    			_lightData = newDataProvider;
    			_lightData.propertyChanged.add(lightDataChangedHandler);
            }
		}
        
		private static function sceneEnterFrameHandler(scene		: Scene,
													   viewport		: Viewport,
													   destination	: BitmapData,
													   time			: uint) : void
		{
			scene.enterFrame.remove(sceneEnterFrameHandler);
			
			sortLights(scene);
		}
		
		private static function sortLights(scene : Scene) : void
		{
			var sceneBindings	: DataBindings			= scene.bindings;
			var lights			: Vector.<ISceneNode>	= scene.getDescendantsByType(AbstractLight);
			var numLights		: uint					= lights.length;
			var lightId			: uint;
			
			// remove all lights from scene bindings.
			var numProviders	: uint					= sceneBindings.numProviders;
			for (var providerId : int = numProviders - 1; providerId >= 0; --providerId)
			{
				var provider : LightDataProvider = sceneBindings.getProviderAt(providerId)
					as LightDataProvider;
				
				if (provider)
					sceneBindings.removeProvider(provider);
			}
			
			// sorting allows to limit the number of shaders that will be generated
			// if (add|remov)ing many lights all the time (add order won't matter anymore).
			lights.sort(compare);
			
			// update all descriptors.
			for (lightId = 0; lightId < numLights; ++lightId)
			{
				var light 	: AbstractLight 	= lights[lightId] as AbstractLight;
				var ctrl 	: LightController 	= light.getControllersByType(LightController)[0]
					as LightController;
				
				ctrl.updateDataProvider(lightId);
				sceneBindings.addProvider(ctrl._lightData);
			}
		}
		
		private static function compare(light1 : AbstractLight, light2 : AbstractLight) : int
		{
			return light1.type - light2.type;
		}
		
		override public function clone() : AbstractController
		{
			return new LightController(null);
		}
	}
}