package aerys.minko.type.enum
{
	import aerys.minko.ns.minko_render;
	
	import flash.display.TriangleCulling;
	import flash.display3D.Context3DTriangleFace;

	public final class TriangleCulling
	{
		public static const NONE	: uint = 0;
		public static const BACK	: uint = 1;
		public static const FRONT	: uint = 2;
		public static const BOTH	: uint = 3;
		
		minko_render static const STRINGS : Vector.<String> = new <String>[
			Context3DTriangleFace.NONE,
			Context3DTriangleFace.FRONT,
			Context3DTriangleFace.BACK,
			Context3DTriangleFace.FRONT_AND_BACK
		];
	}
}
