package aerys.minko.render.effect.basic
{
	import aerys.minko.render.effect.Style;

	public final class BasicStyle
	{
		public static const DIFFUSE_MAP			: int	= Style.getStyleId("basic diffuse map");
		public static const NORMAL_MAP			: int	= Style.getStyleId("basic normal map");
		
		public static const BLENDING			: int	= Style.getStyleId("basic blending");
		
		public static const FRUSTUM_CULLING		: int	= Style.getStyleId("basic frustum culling");
		public static const TRIANGLE_CULLING	: int	= Style.getStyleId("basic triangle culling");
		public static const NORMAL_MULTIPLIER	: int	= Style.getStyleId("basic normal multiplier");
	}
}