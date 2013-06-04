package aerys.minko.render.material.phong
{
	public final class PhongProperties
	{
		public static const RECEPTION_MASK				: String = 'lightReceptionMask';
		
		public static const LIGHT_MAP					: String = 'lightMap';
		public static const SPECULAR_MAP				: String = 'specularMap';
		
		public static const LIGHTMAP_MULTIPLIER			: String = 'lightMapMultiplier';
		public static const AMBIENT_MULTIPLIER			: String = 'ambientMultiplier';
		public static const DIFFUSE_MULTIPLIER			: String = 'diffuseMultiplier';
		public static const SPECULAR					: String = 'specular';
		public static const SHININESS					: String = 'shininess';
		
		public static const SPECULAR_MAP_FORMAT			: String = 'specularMapFormat';
		public static const LIGHT_MAP_FORMAT			: String = 'lightMapFormat';
		
		public static const HEIGHT_MAP					: String = 'heightMap'; 
		public static const HEIGHT_MAP_FORMAT   		: String = 'heightMapFormat';
		public static const HEIGHT_MAP_MIPMAPPING		: String = 'heightMapMipMapping';
		public static const HEIGHT_MAP_FILTERING		: String = 'heightMapFiltering';
		public static const HEIGHT_MAP_WRAPPING 		: String = 'heightMapWrapping';
		
		public static const NORMAL_MAPPING_TYPE			: String = 'normalMappingType';
		public static const NORMAL_MAP					: String = 'normalMap';
        public static const NORMAL_MAP_FORMAT   		: String = 'normalMapFormat';
		public static const NORMAL_MAP_MIPMAPPING		: String = 'normalMapMipMapping';
		public static const NORMAL_MAP_FILTERING		: String = 'normalMapFiltering';
        public static const NORMAL_MAP_WRAPPING 		: String = 'normalMapWrapping';

		public static const PARALLAX_MAPPING_NBSTEPS	: String = 'parallaxMappingNbSteps';
		public static const PARALLAX_MAPPING_BUMP_SCALE	: String = 'parallaxMappingBumpScale';
		
		public static const SHADOW_BIAS					: String = 'shadowBias';
		public static const CAST_SHADOWS				: String = 'castShadows';
		public static const RECEIVE_SHADOWS				: String = 'receiveShadows';
	}
}
