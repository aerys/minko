package aerys.minko.render.renderer.state
{
	public final class Blending
	{
		public static const NORMAL		: uint		= BlendingSource.ONE
													  | BlendingDestination.ZERO;
		
		public static const ALPHA		: uint		= BlendingSource.SOURCE_ALPHA
													  | BlendingDestination.ONE_MINUS_SOURCE_ALPHA;
		
		public static const ADDITIVE	: uint		= BlendingSource.SOURCE_ALPHA
													  | BlendingDestination.ONE;
		
		public static const LIGHT		: uint		= BlendingSource.ZERO
													  | BlendingDestination.SOURCE_COLOR;
	}
}