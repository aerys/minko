package aerys.minko.render.effect.fog
{
	import aerys.minko.scene.visitor.data.Style;

	public final class FogStyle
	{
		public static const FOG_ENABLED	: int = Style.getStyleId("fog enabled");
		public static const COLOR		: int = Style.getStyleId("fog color");
		public static const START		: int = Style.getStyleId("fog start");
		public static const DISTANCE	: int = Style.getStyleId("fog distance");
	}
}