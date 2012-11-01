package aerys.minko.render.material.phong
{
	import aerys.minko.ns.minko_lighting;

	public final class PhongProperties
	{
		public static const RECEPTION_MASK				: String = 'lightReceptionMask';
		
		public static const LIGHT_MAP					: String = 'lightMap';
		public static const SPECULAR_MAP				: String = 'specularMap';
		public static const NORMAL_MAP					: String = 'lightNormalMap';
		public static const HEIGHT_MAP					: String = 'lightHeightMap'; 
		
		public static const LIGHTMAP_MULTIPLIER			: String = 'lightMapMultiplier';
		public static const AMBIENT_MULTIPLIER			: String = 'lightAmbientMultiplier';
		public static const DIFFUSE_MULTIPLIER			: String = 'lightDiffuseMultiplier';
		public static const SPECULAR_MULTIPLIER			: String = 'lightSpecularMultiplier';
		public static const SHININESS_MULTIPLIER		: String = 'lightShininessMultiplier';
		
		public static const NORMAL_MAPPING_TYPE			: String = 'lightNormalMappingType';
		public static const PARALLAX_MAPPING_NBSTEPS	: String = 'lightParallaxMappingNbSteps';
		public static const PARALLAX_MAPPING_BUMP_SCALE	: String = 'lightParallaxMappingBumpScale';
		
		public static const SHADOW_BIAS					: String = 'lightShadowBias';
		public static const CAST_SHADOWS				: String = 'lightCastShadows';
		public static const RECEIVE_SHADOWS				: String = 'lightReceiveShadows';
		
		minko_lighting static function getNameFor(lightId		: uint,
												  propertyName	: String) : String
		{
			return 'light_' + propertyName + '_' + lightId;
		}
		
		minko_lighting static function getPropertyFor(name : String) : String
		{
			return name.substring('light_'.length, name.lastIndexOf('_'));
		}
	}
}
