package aerys.minko.type.enum
{
	/**
	 * The BlendingDestination class is an enumeration of all the possible values for
	 * the blending destination factor used in the blending equation.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 * @see aerys.minko.type.enum.Blending
	 * @see aerys.minko.render.shader.Shader
	 * @see aerys.minko.render.shader.part.BlendingShaderPart
	 * 
	 */
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