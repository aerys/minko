package aerys.minko.type.enum
{
	import aerys.minko.ns.minko_render;
	
	import flash.display3D.Context3DBlendFactor;

	/**
	 * The Blending class is an enumeration of the most common values possible
	 * for the Shader.blending property and the BlendingShaderPart.blend() method.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 * @see aerys.minko.type.enum.BlendingSource
	 * @see aerys.minko.type.enum.BlendingDestination
	 * @see aerys.minko.render.shader.Shader
	 * @see aerys.minko.render.shader.part.BlendingShaderPart
	 * 
	 */
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