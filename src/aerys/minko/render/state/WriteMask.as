package aerys.minko.render.state
{
	public final class WriteMask
	{
		public static const DISABLED	: uint	= 0;
		
		public static const COLOR_RED	: uint	= 1;
		public static const COLOR_GREEN	: uint	= 2;
		public static const COLOR_BLUE	: uint	= 4;
		public static const COLOR_ALPHA	: uint	= 8;
		public static const COLOR		: uint	= COLOR_RED | COLOR_GREEN | COLOR_BLUE
												  | COLOR_ALPHA;
		public static const DEPTH		: uint	= 16;
		public static const STENCIL		: uint	= 32;
		
		public static const ENABLED		: uint	= COLOR | STENCIL | DEPTH;
	}
}