package aerys.minko.render.effect.basic
{
	import aerys.minko.scene.visitor.data.Style;

	public final class BasicStyle
	{
		public static const DIFFUSE_MAP			: int	= Style.getStyleId("basicDiffuseMap");
		
		public static const BLENDING			: int	= Style.getStyleId("basicBlending");
		
		public static const FRUSTUM_CULLING		: int	= Style.getStyleId("basicFrustumCulling");
		public static const TRIANGLE_CULLING	: int	= Style.getStyleId("basicTriangleCulling");
		public static const NORMAL_MULTIPLIER	: int	= Style.getStyleId("basicTriangleCullingMultiplier");
		
	}
}