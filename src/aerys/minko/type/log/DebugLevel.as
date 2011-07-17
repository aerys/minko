package aerys.minko.type.log
{
	public final class DebugLevel
	{
		public static const DISABLED					: uint	= 0;
		
		public static const SHADER_AGAL					: uint	= 1 << 0;
		public static const SHADER_ANTECOMPILE_DOTTY	: uint	= 1 << 1;
		public static const SHADER_POSTCOMPILE_DOTTY	: uint	= 1 << 2;
		public static const SHADER_CONST_ALLOC			: uint	= 1 << 3;
		public static const SHADER_ATTR_ALLOC			: uint	= 1 << 4;
	}
}
