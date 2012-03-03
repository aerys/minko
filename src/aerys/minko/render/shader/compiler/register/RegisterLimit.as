package aerys.minko.render.shader.compiler.register
{
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public final class RegisterLimit
	{
		public static const MAX_VARYING			: uint = 8;

		public static const VS_MAX_ATTRIBUTE	: uint = 8;
		public static const VS_MAX_CONSTANT		: uint = 128;
		public static const VS_MAX_TEMPORARY	: uint = 8;

		public static const FS_MAX_SAMPLER		: uint = 8;
		public static const FS_MAX_CONSTANT		: uint = 28;
		public static const FS_MAX_TEMPORARY	: uint = 8;
	}
}
