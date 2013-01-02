package aerys.minko.render.material.phong
{
	import aerys.minko.render.Effect;
	import aerys.minko.render.material.basic.BasicMaterial;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.binding.IDataProvider;
	import aerys.minko.type.math.Vector4;
	
	import flash.utils.Dictionary;
	
	public class PhongMaterial extends BasicMaterial
	{
		public static const DEFAULT_NAME	: String		= 'PhongMaterial';
		
		private static const EFFECTS		: Dictionary	= new Dictionary(true);
		
		public function get receptionMask() : uint
		{
			return getProperty(PhongProperties.RECEPTION_MASK) as uint;
		}
		public function set receptionMask(value : uint) : void
		{
			setProperty(PhongProperties.RECEPTION_MASK, value);
		}
		
		public function get lightmap() : TextureResource
		{
			return getProperty(PhongProperties.LIGHT_MAP) as TextureResource;
		}
		public function set lightmap(value : TextureResource) : void
		{
			setProperty(PhongProperties.LIGHT_MAP, value);
		}
		
		public function get specularMap() : TextureResource
		{
			return getProperty(PhongProperties.SPECULAR_MAP) as TextureResource;
		}
		public function set specularMap(value : TextureResource) : void
		{
			setProperty(PhongProperties.SPECULAR_MAP, value);
		}
		
		public function get lightmapMultiplier() : Number
		{
			return getProperty(PhongProperties.LIGHTMAP_MULTIPLIER) as Number;
		}
		public function set lightmapMultiplier(value : Number) : void
		{
			setProperty(PhongProperties.LIGHTMAP_MULTIPLIER, value);
		}
		
		public function get ambientMultiplier() : Number
		{
			return getProperty(PhongProperties.AMBIENT_MULTIPLIER) as Number;
		}
		public function set ambientMultiplier(value : Number) : void
		{
			setProperty(PhongProperties.AMBIENT_MULTIPLIER, value);
		}
		
		public function get diffuseMultiplier() : Number
		{
			return getProperty(PhongProperties.DIFFUSE_MULTIPLIER) as Number;
		}
		public function set diffuseMultiplier(value : Number) : void
		{
			setProperty(PhongProperties.DIFFUSE_MULTIPLIER, value);
		}
		
		public function get specular() : Vector4
		{
			return getProperty(PhongProperties.SPECULAR) as Vector4;
		}
		
		public function set specular(value : Vector4) : void
		{
			setProperty(PhongProperties.SPECULAR, value);
		}
		
		public function get shininess() : Number
		{
			return getProperty(PhongProperties.SHININESS) as Number;
		}
		public function set shininess(value : Number) : void
		{
			setProperty(PhongProperties.SHININESS, value);
		}
		
		public function get normalMappingType() : uint
		{
			return getProperty(PhongProperties.NORMAL_MAPPING_TYPE) as uint;
		}
		public function set normalMappingType(value : uint) : void
		{
			setProperty(PhongProperties.NORMAL_MAPPING_TYPE, value);
		}
		
		public function get normalMap() : TextureResource
		{
			return getProperty(PhongProperties.NORMAL_MAP) as TextureResource;
		}
		public function set normalMap(value : TextureResource) : void
		{
			setProperty(PhongProperties.NORMAL_MAP, value);
		}
		
		public function get heightMap() : TextureResource
		{
			return getProperty(PhongProperties.HEIGHT_MAP) as TextureResource;
		}
		public function set heightMap(value : TextureResource) : void
		{
			setProperty(PhongProperties.HEIGHT_MAP, value);
		}
		
		public function get numParallaxMappingSteps() : uint
		{
			return getProperty(PhongProperties.PARALLAX_MAPPING_NBSTEPS) as uint;
		}
		public function set numParallaxMappingSteps(value : uint) : void
		{
			setProperty(PhongProperties.PARALLAX_MAPPING_NBSTEPS, value);
		}
		
		public function get parallaxMappingBumpScale() : Number
		{
			return getProperty(PhongProperties.PARALLAX_MAPPING_BUMP_SCALE) as Number;
		}
		public function set parallaxMappingBumpScale(value : Number) : void
		{
			setProperty(PhongProperties.PARALLAX_MAPPING_BUMP_SCALE, value);
		}
		
		public function get shadowBias() : Number
		{
			return getProperty(PhongProperties.SHADOW_BIAS) as Number;
		}
		public function set shadowBias(value : Number) : void
		{
			setProperty(PhongProperties.SHADOW_BIAS, value);
		}
		
		public function get castShadows() : Boolean
		{
			return getProperty(PhongProperties.CAST_SHADOWS) as Boolean;
		}
		public function set castShadows(value : Boolean) : void
		{
			setProperty(PhongProperties.CAST_SHADOWS, value);
		}
		
		public function get receiveShadows() : Boolean
		{
			return getProperty(PhongProperties.RECEIVE_SHADOWS) as Boolean;
		}
		public function set receiveShadows(value : Boolean) : void
		{
			setProperty(PhongProperties.RECEIVE_SHADOWS, value);
		}
		
		public function PhongMaterial(scene 		: Scene,
									  properties 	: Object 	= null,
									  effect		: Effect	= null,
									  name 			: String 	= DEFAULT_NAME)
		{
			super(
				properties,
				effect || (EFFECTS[scene] || (EFFECTS[scene] = new PhongEffect(scene))),
				name
			);
		}
		
		override public function clone() : IDataProvider
		{
			return new PhongMaterial((effect as PhongEffect).scene, this, effect, name);
		}
	}
}