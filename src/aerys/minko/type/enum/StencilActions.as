package aerys.minko.type.enum 
{
	import aerys.minko.ns.minko_render;
	import flash.display3D.Context3DStencilAction;

	public class StencilActions 
	{
		public static const DECREMENT_SATURATE		: int = 0;
		public static const DECREMENT_WRAP			: int = 1;
		public static const INCREMENT_SATURATE		: int = 2;
		public static const INCREMENT_WRAP			: int = 3;		
		public static const INVERT					: int = 4;
		public static const KEEP					: int = 5;
		public static const SET						: int = 6;
		public static const ZERO					: int = 7;

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