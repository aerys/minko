package aerys.minko.type.enum
{
	import aerys.minko.ns.minko_render;
	
	import flash.display3D.Context3DCompareMode;

	public final class DepthTest
	{
		public static const NEVER			: uint				= 1;

		public static const EQUAL			: uint				= 2;
		public static const GREATER			: uint				= 4;
		public static const LESS			: uint				= 8;

		public static const NOT_EQUAL		: uint				= 16;
		
		public static const DISABLE_WRITE	: uint				= 32;

		public static const ALWAYS			: uint				= EQUAL | GREATER | LESS | NOT_EQUAL;
		
		minko_render static const STRINGS 	: Vector.<String> 	= new <String>[
			Context3DCompareMode.NEVER,
			Context3DCompareMode.GREATER,
			Context3DCompareMode.GREATER_EQUAL,
			Context3DCompareMode.EQUAL,
			Context3DCompareMode.LESS_EQUAL,
			Context3DCompareMode.LESS,
			Context3DCompareMode.NOT_EQUAL,
			Context3DCompareMode.ALWAYS
		];
		
		minko_render static const FLAGS		: Vector.<uint>		= Vector.<uint>([
			DepthTest.NEVER,
			DepthTest.GREATER,
			DepthTest.GREATER | DepthTest.EQUAL,
			DepthTest.EQUAL,
			DepthTest.LESS | DepthTest.EQUAL,
			DepthTest.LESS,
			DepthTest.NOT_EQUAL,
			DepthTest.ALWAYS
		]);
	}
}