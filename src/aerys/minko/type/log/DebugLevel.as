package aerys.minko.type.log
{
	public final class DebugLevel
	{
		public static const DISABLED					: uint	= 0;

		public static const LOAD_ERROR					: uint	= 1 << 0;
		
		public static const SHADER_AGAL					: uint	= 1 << 1;
		public static const SHADER_WARNING				: uint	= 1 << 2;
		public static const SHADER_DOTTY				: uint	= 1 << 3;
		public static const SHADER_CONST_ALLOC			: uint	= 1 << 4;
		public static const SHADER_ATTR_ALLOC			: uint	= 1 << 5;

		public static const CONTEXT						: uint	= 1 << 6;
		
		public static const PLUGIN_NOTICE				: uint	= 1 << 7;
		public static const PLUGIN_WARNING				: uint	= 1 << 8;
		public static const PLUGIN_ERROR				: uint	= 1 << 9;
		
		public static const ALL							: uint 	= 0xffffffff;
		
	}
}
