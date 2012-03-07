package aerys.minko.type.enum
{
	/**
	 * The BlendingSource class is an enumeration of all the possible values for
	 * the blending source factor used in the blending equation.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 * @see aerys.minko.type.enum.Blending
	 * @see aerys.minko.render.shader.Shader
	 * @see aerys.minko.render.shader.part.BlendingShaderPart
	 * 
	 */
	public final class BlendingSource
	{
		public static const DESTINATION_ALPHA			: uint	= 0;
		public static const DESTINATION_COLOR			: uint	= 1;
		public static const ONE							: uint	= 2;
		public static const ONE_MINUS_DESTINATION_ALPHA	: uint	= 3;
		public static const ONE_MINUS_DESTINATION_COLOR	: uint	= 4;
		public static const ONE_MINUS_SOURCE_ALPHA		: uint	= 5;
		public static const SOURCE_ALPHA				: uint	= 6;
		public static const SOURCE_COLOR				: uint	= 7;
		public static const ZERO						: uint	= 8;

		public static const DEFAULT						: uint	= ONE;
	}
}