package aerys.minko.type.enum
{
	import aerys.minko.ns.minko;

	import flash.display3D.Context3DCompareMode;

	public final class CompareMode
	{
		public static const NEVER		: uint	= 1;

		public static const EQUAL		: uint	= 2;
		public static const GREATER		: uint	= 4;
		public static const LESS		: uint	= 8;

		public static const NOT_EQUAL	: uint	= 16;

		public static const ALWAYS		: uint	= EQUAL | GREATER | LESS | NOT_EQUAL;
	}
}