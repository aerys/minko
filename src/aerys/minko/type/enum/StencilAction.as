package aerys.minko.type.enum 
{
	import aerys.minko.ns.minko_render;
	import flash.display3D.Context3DStencilAction;

	public final class StencilAction 
	{
		public static const DECREMENT_SATURATE		: uint = 0;
		public static const DECREMENT_WRAP			: uint = 1;
		public static const INCREMENT_SATURATE		: uint = 2;
		public static const INCREMENT_WRAP			: uint = 3;		
		public static const INVERT					: uint = 4;
		public static const KEEP					: uint = 5;
		public static const SET						: uint = 6;
		public static const ZERO					: uint = 7;

		minko_render static const STRINGS : Vector.<String> = new <String>[
			Context3DStencilAction.DECREMENT_SATURATE,
			Context3DStencilAction.DECREMENT_WRAP,
			Context3DStencilAction.INCREMENT_SATURATE,
			Context3DStencilAction.INCREMENT_WRAP,
			Context3DStencilAction.INVERT,
			Context3DStencilAction.KEEP,
			Context3DStencilAction.SET,
			Context3DStencilAction.ZERO
		];		
	}
}