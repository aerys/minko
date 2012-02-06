package aerys.minko.type.enum
{
	public final class ColorMask
	{
		public static const NONE	: uint	= 0;

		public static const RED		: uint	= 1;
		public static const GREEN	: uint	= 2;
		public static const BLUE	: uint	= 4;
		public static const ALPHA	: uint	= 8;
		public static const RGB		: uint	= RED | GREEN | BLUE;
		public static const RGBA	: uint	= RGB | ALPHA;
	}
}