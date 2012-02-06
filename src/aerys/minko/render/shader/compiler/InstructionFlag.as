package aerys.minko.render.shader.compiler
{
	public class InstructionFlag
	{
		public static const VERTEX_ONLY		: uint = 1;
		public static const FRAGMENT_ONLY	: uint = 2;
		public static const SPECIAL_MATRIX	: uint = 4;
		public static const COMMUTATIVE		: uint = 8;
		public static const SINGLE			: uint = 16;
		public static const COMPONENT_WISE	: uint = 32;
		public static const LINEAR			: uint = 64;
	}
}
