package aerys.minko.scene.controller.light
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.light.AbstractLight;
	import aerys.minko.type.binding.DataBindings;
	
	import flash.display.BitmapData;

	public class LightController extends AbstractController
	{
		private var _light 		: AbstractLight;
		private var _lightData	: LightDataProvider;
		
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
			targetAdded.add(lightAddedHandler);
			targetRemoved.add(lightRemovedHandler);
		}
		
		protected function lightAddedHandler(ctrl	: LightController,
											 light	: AbstractLight) : void
		{
			_lightData = new LightDataProvider(-1);
			_lightData.setLightProperty('type', light.type);
			_lightData.setLightProperty('localToWorld', light.localToWorld);
			_lightData.setLightProperty('worldToLocal', light.worldToLocal);
			_lightData.setLightProperty('enabled', true);
			_lightData.changed.add(lightDataChangedHandler);
			
			light.addedToScene.add(lightAddedToSceneHandler);
			light.removedFromScene.add(lightRemovedFromSceneHandler);
			
			_light = light;
		}
		
		protected function lightRemovedHandler(ctrl		: LightController,
											   light	: AbstractLight) : void
		{
			throw new Error();
		}
		
		protected function lightAddedToSceneHandler(light	: AbstractLight,
													scene	: Scene) : void
		{
//			_lightData.changed.add(lightDataChangedHandler);
			
			sortLights(scene);
//			if (!scene.enterFrame.hasCallback(sceneEnterFrameHandler))
//				scene.enterFrame.add(sceneEnterFrameHandler);
		}
		
		protected function lightRemovedFromSceneHandler(light	: AbstractLight,
														scene	: Scene) : void
		{
//			_lightData.changed.remove(lightDataChangedHandler);
			
            sortLights(scene);
//			if (!scene.enterFrame.hasCallback(sceneEnterFrameHandler))
//				scene.enterFrame.add(sceneEnterFrameHandler);
		}
		
		protected function lightDataChangedHandler(lightData	: LightDataProvider,
												   propertyName	: String) : void
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
			
            if (_lightData.changed.hasCallback(lightDataChangedHandler))
            {
    			_lightData.changed.remove(lightDataChangedHandler);
    			_lightData = newDataProvider;
    			_lightData.changed.add(lightDataChangedHandler);
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
	}
}