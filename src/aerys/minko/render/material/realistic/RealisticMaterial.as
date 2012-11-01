package aerys.minko.render.material.realistic
{
	import aerys.minko.render.Effect;
	import aerys.minko.render.material.environment.EnvironmentMappingProperties;
	import aerys.minko.render.material.phong.PhongMaterial;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.scene.node.Scene;
	
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
		
		public function RealisticMaterial(scene			: Scene,
										  properties	: Object	= null,
										  effect		: Effect	= null,
										  name			: String	= DEFAULT_NAME)
		{
			effect ||= SCENE_TO_EFFECT[scene] || (SCENE_TO_EFFECT[scene] = new RealisticEffect(scene));
			
			super(scene, properties, effect, name);
		}
	}
}