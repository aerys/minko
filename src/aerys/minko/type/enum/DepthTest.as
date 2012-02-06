package aerys.minko.type.enum
{
	public final class DepthTest
	{
		public static const NEVER			: uint	= 1;

		public static const EQUAL			: uint	= 2;
		public static const GREATER			: uint	= 4;
		public static const LESS			: uint	= 8;

		public static const NOT_EQUAL		: uint	= 16;
		
		public static const DISABLE_WRITE	: uint	= 32;

		public static const ALWAYS			: uint	= EQUAL | GREATER | LESS | NOT_EQUAL;
	}
}