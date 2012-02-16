package aerys.minko.type.enum
{
	import aerys.minko.ns.minko_render;
	
	import flash.display3D.Context3DBlendFactor;

	public final class Blending
	{
		public static const NORMAL		: uint		= BlendingSource.ONE
													  | BlendingDestination.ZERO;

		public static const ALPHA		: uint		= BlendingSource.SOURCE_ALPHA
													  | BlendingDestination.ONE_MINUS_SOURCE_ALPHA;

		public static const ADDITIVE	: uint		= BlendingSource.SOURCE_ALPHA
													  | BlendingDestination.ONE;

		public static const LIGHT		: uint		= BlendingSource.ZERO
													  | BlendingDestination.SOURCE_COLOR;
		
		minko_render static const STRINGS : Vector.<String> = new <String>[
			Context3DBlendFactor.DESTINATION_ALPHA,
			Context3DBlendFactor.DESTINATION_COLOR,
			Context3DBlendFactor.ONE,
			Context3DBlendFactor.ONE_MINUS_DESTINATION_ALPHA,
			Context3DBlendFactor.ONE_MINUS_DESTINATION_COLOR,
			Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA,
			Context3DBlendFactor.SOURCE_ALPHA,
			Context3DBlendFactor.SOURCE_COLOR,
			Context3DBlendFactor.ZERO
		];
	}
}