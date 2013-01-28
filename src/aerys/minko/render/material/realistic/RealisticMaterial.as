package aerys.minko.render.material.realistic
{
	import aerys.minko.render.Effect;
	import aerys.minko.render.material.environment.EnvironmentMappingProperties;
	import aerys.minko.render.material.phong.PhongMaterial;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.binding.IDataProvider;
	
	import flash.utils.Dictionary;
	
	public class RealisticMaterial extends PhongMaterial
	{
		private static const DEFAULT_NAME		: String		= 'RealisticMaterial';
		private static const SCENE_TO_EFFECT	: Dictionary	= new Dictionary(true);
		
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
			effect ||= new RealisticEffect();
			
			super(properties, effect, name);
		}
        
        override public function clone() : IDataProvider
        {
            return new RealisticMaterial(this, effect, name);
        }
	}
}