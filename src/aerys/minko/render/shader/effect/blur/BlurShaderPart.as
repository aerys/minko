package aerys.minko.render.shader.effect.blur
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.ShaderPart;
	
	public class BlurShaderPart extends ShaderPart
	{
		private static const OFFSETS	: Vector.<Number>	= new <Number>[0.0, 1.3846153846, 3.2307692308];
		private static const WEIGHTS	: Vector.<Number>	= new <Number>[0.2270270270, 0.3162162162, 0.0702702703];
		
		/**
		 * Efficient Gaussian blur with linear sampling using a 3x3 kernel.
		 * 
		 * http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
		 *  
		 * @param main
		 * 
		 */
		public function BlurShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function linearGaussianBlurX(texture		: SFloat,
											outputSize	: SFloat,
                                            spread      : SFloat    = null) : SFloat
		{
			var fragmentCoord 	: SFloat = interpolate(vertexUV.xy);
			var color 			: SFloat = multiply(sampleTexture(texture, fragmentCoord), WEIGHTS[0]);
			
			for (var i : uint = 1; i < 3; ++i)
			{
                var offset : SFloat    = divide(float2(OFFSETS[i], 0), outputSize.xx);
                
                if (spread)
                    offset.scaleBy(spread);
                
				color.incrementBy(multiply(
					WEIGHTS[i],
					sampleTexture(texture, add(fragmentCoord, offset))
				));
				
				color.incrementBy(multiply(
					WEIGHTS[i],
					sampleTexture(texture, subtract(fragmentCoord, offset))
				));
			}
			
			return color;
		}
		
		public function linearGaussianBlurY(texture		: SFloat,
											outputSize	: SFloat,
                                            spread      : SFloat    = null) : SFloat
		{
			var fragmentCoord 	: SFloat = interpolate(vertexUV.xy);
			var color 			: SFloat = multiply(sampleTexture(texture, fragmentCoord), WEIGHTS[0]);
			
			for (var i : uint = 1; i < 3; ++i)
			{
                var offset : SFloat    = divide(float2(0, OFFSETS[i]), outputSize.yy);
                
                if (spread)
                    offset.scaleBy(spread);
                
				color.incrementBy(multiply(
					WEIGHTS[i],
					sampleTexture(texture, add(fragmentCoord, offset))
				));
				
				color.incrementBy(multiply(
					WEIGHTS[i],
					sampleTexture(texture, subtract(fragmentCoord, offset))
				));
			}
			
			return color;
		}
	}
}