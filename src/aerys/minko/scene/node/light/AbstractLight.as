package aerys.minko.scene.node.light
{
	import aerys.minko.ns.minko_lighting;
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.DataProvider;
	
	use namespace minko_lighting;
	
	public class AbstractLight extends AbstractSceneNode
	{
		private static const TYPE_STRINGS : Vector.<String> = 
			new <String>['AmbientLight', 'DirectionalLight', 'PointLight', 'SpotLight'];
		
		private var _dataProvider	: DataProvider;
		private var _lightId		: int;
		
		public function get color() : uint
		{
			return getProperty('color') as uint;
		}
		
		public function get emissionMask() : uint
		{
			return getProperty('emissionMask') as uint;
		}
		
		public function get shadowCastingType() : uint
		{
			return getProperty('shadowCastingType') as uint; 
		}
		
		public function get enabled() : Boolean
		{
			return getProperty('enabled') as Boolean; 
		}
		
		public function set color(v : uint)	: void
		{
			setProperty('color', v);
		}
		
		public function set emissionMask(v : uint) : void
		{
			setProperty('emissionMask', v);
		}
		
		public function set shadowCastingType(v : uint) : void
		{
			throw new Error('Must be overriden');
		}
		
		public function set enabled(v : Boolean) : void
		{
			setProperty('enabled', v);
		}
		
		private function set lightId(v : int) : void
		{
			var numProperties	: uint				= 0;
			var propertyNames	: Vector.<String>	= new Vector.<String>();
			var propertyValues	: Vector.<Object>	= new Vector.<Object>();
			
			for (var propertyName : String in _dataProvider.dataDescriptor)
			{
				propertyNames.push(PhongProperties.getPropertyFor(propertyName));
				propertyValues.push(_dataProvider.getProperty(propertyName));
				++numProperties;
			}
			
			_dataProvider.removeAllProperties();
			_lightId = v;
			
			for (var propertyId : uint = 0; propertyId < numProperties; ++propertyId)
				_dataProvider.setProperty(
					PhongProperties.getNameFor(v, propertyNames[propertyId]), 
					propertyValues[propertyId]
				);
		}
		
		public function AbstractLight(color				: uint,
									  emissionMask		: uint,
									  shadowCastingType	: uint,
									  type				: uint)
		{
			_dataProvider			= new LightDataProvider();
			_lightId				= -1;
			
			this.color				= color;
			this.emissionMask		= emissionMask;
			this.shadowCastingType	= shadowCastingType;
			
			setProperty('type', type);
			setProperty('localToWorld', localToWorld);
			setProperty('worldToLocal', worldToLocal);
			setProperty('enabled', true);
		}
		
		protected final function getProperty(name : String) : *
		{
			var propertyName : String = PhongProperties.getNameFor(_lightId, name);
			
			return _dataProvider.getProperty(propertyName);
		}
		
		protected final function setProperty(name : String, value : Object) : void
		{
			var propertyName : String = PhongProperties.getNameFor(_lightId, name);
			
			_dataProvider.setProperty(propertyName, value);
		}
		
		protected final function removeProperty(name : String) : void
		{
			if (_dataProvider.getProperty(name) !== null)
				_dataProvider.removeProperty(name);
		}
		
		override protected function addedToSceneHandler(child : ISceneNode, scene : Scene):void
		{
			// this happens AFTER being added to scene
			super.addedToSceneHandler(child, scene);
			sortLights(scene);
		}
		
		override protected function removedFromSceneHandler(child : ISceneNode, scene : Scene):void
		{
			// This happens AFTER being removed from scene.
			super.removedFromSceneHandler(child, scene);
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
				var provider : DataProvider = sceneBindings.getProviderAt(providerId) as DataProvider;
				if (provider is LightDataProvider)
					sceneBindings.removeProvider(provider);
			}
			
			// sorting allows to limit the number of shaders that will be generated
			// if (add|remov)ing many lights all the time (add order won't matter anymore).
			lights.sort(compare);
			
			// update all descriptors.
			for (lightId = 0; lightId < numLights; ++lightId)
				AbstractLight(lights[lightId]).lightId = lightId;
			
			// put back all lights into scene bindings.
			for (lightId = 0; lightId < numLights; ++lightId)
				sceneBindings.addProvider(AbstractLight(lights[lightId])._dataProvider);
		}
		
		private static function compare(light1 : AbstractLight, light2 : AbstractLight) : int
		{
			return Object(light1).constructor.TYPE - Object(light2).constructor.TYPE;
		}
	}
}
