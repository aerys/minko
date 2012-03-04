package aerys.minko.render.shader.compiler
{
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class InstructionFlag
	{
		public static const AVAILABLE_VS	: uint = 1;
		public static const AVAILABLE_FS	: uint = 2;
		public static const AVAILABLE_CPU	: uint = 4;
		
		public static const AVAILABLE_ALL	: uint = 1 | 2 | 4;
		
		public static const SPECIAL_MATRIX	: uint = 8;
		public static const COMMUTATIVE		: uint = 16;
		public static const ASSOCIATIVE		: uint = 32;
		
		public static const SINGLE			: uint = 64;
		public static const COMPONENT_WISE	: uint = 128;
		public static const LINEAR			: uint = 256;
	}
}
