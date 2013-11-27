package aerys.minko.render.material.basic
{
	public class BasicProperties
	{
		public static const DIFFUSE_COLOR							: String	= 'diffuseColor';
		public static const DIFFUSE_TRANSFORM						: String	= 'diffuseTransform';
		public static const DIFFUSE_MAP								: String	= 'diffuseMap';
		public static const DIFFUSE_MAP_FILTERING					: String	= 'diffuseMapFiltering';
		public static const DIFFUSE_MAP_MIPMAPPING					: String	= 'diffuseMapMipMapping';
		public static const DIFFUSE_MAP_WRAPPING					: String	= 'diffuseMapWrapping';
        public static const DIFFUSE_MAP_FORMAT  					: String	= 'diffuseMapFormat';
		public static const UV_SCALE								: String	= 'uvScale';
		public static const UV_OFFSET								: String	= 'diffuseMapUvOffset';
		
		public static const ALPHA_THRESHOLD							: String	= 'alphaThreshold';
		public static const ALPHA_MAP								: String	= 'alphaMap';
		public static const ALPHA_MAP_FILTERING						: String	= 'alphaMapFiltering';
		public static const ALPHA_MAP_MIPMAPPING					: String	= 'alphaMapMipMapping';
		public static const ALPHA_MAP_WRAPPING						: String	= 'alphaMapWrapping';
		public static const ALPHA_MAP_FORMAT  						: String	= 'alphaMapFormat';
		public static const ALPHA_MAP_UV_SCALE						: String	= 'alphaMapUvScale';
		public static const ALPHA_MAP_UV_OFFSET						: String	= 'alphaMapUvOffset';
		
		public static const BLENDING								: String	= 'blending';
		public static const DEPTH_TEST								: String	= 'depthTest';
		public static const TRIANGLE_CULLING						: String	= 'triangleCulling';
		public static const DEPTH_WRITE_ENABLED						: String	= 'depthWriteEnabled';		
		
		public static const STENCIL_TRIANGLE_FACE					: String	= 'stencilTriangleFace';
		public static const STENCIL_COMPARE_MODE					: String	= 'stencilCompareMode';
		public static const STENCIL_ACTION_BOTH_PASS				: String	= 'stencilActionOnBothPass';
		public static const STENCIL_ACTION_DEPTH_FAIL				: String	= 'stencilActionOnDepthFail';
		public static const STENCIL_ACTION_DEPTH_PASS_STENCIL_FAIL	: String	= 'stencilActionOnDepthPassStencilFail';
		public static const STENCIL_REFERENCE_VALUE					: String	= 'stencilReferenceValue';
		public static const STENCIL_READ_MASK						: String	= 'stencilReadMask';
		public static const STENCIL_WRITE_MASK						: String	= 'stencilWriteMask';
	}
}