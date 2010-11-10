package aerys.minko.render.state
{
	public final class BlendingDestination
	{
		private static const DST_OFFSET					: uint	= 16;
		
		public static const DESTINATION_ALPHA			: uint	= BlendingSource.DESTINATION_ALPHA << DST_OFFSET;
		public static const DESTINATION_COLOR			: uint	= BlendingSource.DESTINATION_COLOR << DST_OFFSET;
		public static const ONE							: uint	= BlendingSource.ONE << DST_OFFSET;
		public static const ONE_MINUS_DESTINATION_ALPHA	: uint	= BlendingSource.ONE_MINUS_DESTINATION_ALPHA << DST_OFFSET;
		public static const ONE_MINUS_DESTINATION_COLOR	: uint	= BlendingSource.ONE_MINUS_DESTINATION_COLOR << DST_OFFSET;
		public static const ONE_MINUS_SOURCE_ALPHA		: uint	= BlendingSource.ONE_MINUS_SOURCE_ALPHA << DST_OFFSET;
		public static const SOURCE_ALPHA				: uint	= BlendingSource.SOURCE_ALPHA << DST_OFFSET;
		public static const SOURCE_COLOR				: uint	= BlendingSource.SOURCE_COLOR << DST_OFFSET;
		public static const ZERO						: uint	= BlendingSource.ZERO << DST_OFFSET;
		
		public static const DEFAULT						: uint	= ZERO;
	}
}