package aerys.minko.render.effect.fog
{
	import aerys.minko.scene.visitor.data.Style;

	public final class FogStyle
	{
		public static const FOG_ENABLED	: int = Style.getStyleId("fogEnabled");
		public static const COLOR		: int = Style.getStyleId("fogColor");
		public static const START		: int = Style.getStyleId("fogStart");
		public static const DISTANCE	: int = Style.getStyleId("fogDistance");
	}
}