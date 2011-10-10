package aerys.minko.type.enum
{
	public final class ColorMask
	{
		public static const NONE		: uint	= 0;

		public static const COLOR_RED	: uint	= 1;
		public static const COLOR_GREEN	: uint	= 2;
		public static const COLOR_BLUE	: uint	= 4;
		public static const COLOR_ALPHA	: uint	= 8;
		public static const COLOR_RGB	: uint	= COLOR_RED | COLOR_GREEN | COLOR_BLUE;
		public static const COLOR		: uint	= COLOR_RGB | COLOR_ALPHA;
	}
}