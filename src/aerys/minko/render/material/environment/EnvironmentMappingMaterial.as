package aerys.minko.render.material.environment
{
	import aerys.minko.render.Effect;
	import aerys.minko.render.material.basic.BasicMaterial;
	import aerys.minko.render.resource.texture.ITextureResource;
	
	public class EnvironmentMappingMaterial extends BasicMaterial
	{
		private static const DEFAULT_NAME	: String	= 'EnvironmentMappingMaterial';
		private static const DEFAULT_EFFECT	: Effect	= new Effect(new EnvironmentMappingShader());
		
		public function get environmentMap() : ITextureResource
		{
			return getProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP) as ITextureResource;
		}
		public function set environmentMap(value : ITextureResource) : void
		{
			setProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP, value);
		}
		
		public function get reflectivity() : Number
		{
			return getProperty(EnvironmentMappingProperties.REFLECTIVITY) as Number;
		}
		public function set reflectivity(value : Number) : void
		{
			setProperty(EnvironmentMappingProperties.REFLECTIVITY, value);
		}
		
		public function get environmentMappingType() : uint
		{
			return getProperty(EnvironmentMappingProperties.ENVIRONMENT_MAPPING_TYPE) as uint;
		}
		public function set environmentMappingType(value : uint) : void
		{
			setProperty(EnvironmentMappingProperties.ENVIRONMENT_MAPPING_TYPE, value);
		}
		
		public function EnvironmentMappingMaterial(properties	: Object	= null,
												   effect		: Effect	= null,
												   name			: String	= DEFAULT_NAME)
		{
			super(properties, effect || DEFAULT_EFFECT, name);
		}
	}
}