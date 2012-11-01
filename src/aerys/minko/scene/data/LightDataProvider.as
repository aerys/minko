package aerys.minko.scene.data
{
	import aerys.minko.ns.minko_lighting;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.enum.DataProviderUsage;

	public class LightDataProvider extends DataProvider
	{
		private var _lightId	: int;
		
		public function LightDataProvider(lightId : int)
		{
			super(null, 'LightingDataProvider', DataProviderUsage.MANAGED);
			
			_lightId = lightId;
		}
		
		public function setLightProperty(name		: String,
										 newValue	: Object) : LightDataProvider
		{
			setProperty(getLightPropertyName(name, _lightId), newValue);
			
			return this;
		}
		
		public function getLightProperty(name	: String) : *
		{
			return getProperty(getLightPropertyName(name, _lightId));
		}
		
		public static function getLightPropertyName(propertyName 	: String,
													lightId			: int) : String
		{
			return 'light_' + lightId + '_' + propertyName;
		}
		
		public static function getPropertyName(lightPropertyName : String) : String
		{
			return lightPropertyName.substr(lightPropertyName.lastIndexOf('_') + 1);
		}
	}
}
