package aerys.minko.render.shader.part
{
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.type.enum.BlendingSource;
	
	public class BlendingShaderPart extends ShaderPart
	{
		public function BlendingShaderPart(main:ActionScriptShader)
		{
			super(main);
		}
		
		public function blend(sourceColor : SFloat, destColor : SFloat, blendingMode : uint) : SFloat
		{
			
			var sourceBlendingMode	: uint		= blendingMode & 0x0000FFFF;
			var source				: SFloat;
			
			if (sourceBlendingMode == BlendingSource.ONE)		source = sourceColor;
			else if (sourceBlendingMode == BlendingSource.ZERO)	source = null;
			else source = multiply(sourceColor, blendFactor(sourceColor, destColor, sourceBlendingMode));
			
			var destBlendingMode	: uint		= (blendingMode & 0xFFFF0000) >>> 16;
			var dest				: SFloat;
			
			if (destBlendingMode == BlendingSource.ONE)			dest = destColor;
			else if (destBlendingMode == BlendingSource.ZERO)	dest = null;
			else dest = multiply(destColor, blendFactor(sourceColor, destColor, destBlendingMode));
			
			var result	: SFloat;
			if (dest == null && source == null)	return float4(0, 0, 0, 0);
			else if (dest == null)				return source;
			else if (source == null)			return dest;
			else								return add(source, dest);
			
			return float4(result.xyz, destColor.w);
		}
		
		private function blendFactor(sourceColor	: SFloat,
									 destColor		: SFloat,
									 blendingMode	: uint) : SFloat
		{
			switch (blendingMode)
			{
				case BlendingSource.DESTINATION_ALPHA:
					return destColor.w;
					
				case BlendingSource.DESTINATION_COLOR:
					return destColor;
					
				case BlendingSource.ONE:
					return float(1);
					
				case BlendingSource.ONE_MINUS_DESTINATION_ALPHA:
					return subtract(1, destColor.w);
					
				case BlendingSource.ONE_MINUS_DESTINATION_COLOR:
					return subtract(1., destColor);
					
				case BlendingSource.ONE_MINUS_SOURCE_ALPHA:
					return subtract(1, sourceColor.w);
					
				case BlendingSource.SOURCE_ALPHA:
					return sourceColor.w;
					
				case BlendingSource.SOURCE_COLOR:
					return sourceColor;
					
				case BlendingSource.ZERO:
					return float(0);
					
				default:
					throw new Error('Invalid BlendingMode.');
			}
		}
	}
}