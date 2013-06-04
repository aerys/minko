package aerys.minko.render.material.realistic
{
	import aerys.minko.render.Effect;
	import aerys.minko.render.material.environment.EnvironmentMappingProperties;
	import aerys.minko.render.material.phong.PhongEffect;
	import aerys.minko.render.material.phong.PhongMaterial;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.type.binding.IDataProvider;
	
	public class RealisticMaterial extends PhongMaterial
	{
		private static const DEFAULT_NAME		: String		= 'RealisticMaterial';
        private static const DEFAULT_EFFECT     : Effect        = new PhongEffect(
            null,
            new RealisticSinglePassShader(null, 0.),
            new RealisticBaseShader(null, .5)
        );
		
		public function get environmentMap() : ITextureResource
		{
			return getProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP) as ITextureResource;
		}
		public function set environmentMap(value : ITextureResource) : void
		{
			setProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP, value);
		}
        
        public function get environmentMapFiltering() : uint
        {
            return getProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP_FILTERING);
        }
        public function set environmentMapFiltering(value : uint) : void
        {
            setProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP_FILTERING, value);
        }
        
        public function get environmentMapMipMapping() : uint
        {
            return getProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP_MIPMAPPING);
        }
        public function set environmentMapMipMapping(value : uint) : void
        {
            setProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP_MIPMAPPING, value);
        }
        
        public function get environmentMapWrapping() : uint
        {
            return getProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP_WRAPPING);
        }
        public function set environmentMapWrapping(value : uint) : void
        {
            setProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP_WRAPPING, value);
        }
        
        public function get environmentMapFormat() : uint
        {
            return getProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP_FORMAT);
        }
        public function set environmentMapFormat(value : uint) : void
        {
            setProperty(EnvironmentMappingProperties.ENVIRONMENT_MAP_FORMAT, value);
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
		
		public function get environmentBlending() : uint
		{
			return getProperty(EnvironmentMappingProperties.ENVIRONMENT_BLENDING) as uint;
		}
		public function set environmentBlending(value : uint) : void
		{
			setProperty(EnvironmentMappingProperties.ENVIRONMENT_BLENDING, value);
		}
		
		public function RealisticMaterial(properties	: Object	= null,
										  effect		: Effect	= null,
										  name			: String	= DEFAULT_NAME)
		{
			super(properties, effect || DEFAULT_EFFECT, name);
		}
        
        override public function clone() : IDataProvider
        {
            return new RealisticMaterial(this, effect, name);
        }
	}
}