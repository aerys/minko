package aerys.minko.render.shader
{
	import aerys.minko.render.effect.Style;

	/**
	 * The base class for post-processing shaders. 
	 * 
	 * To create a post-processing shader, one must simply extend this class
	 * and override the 'getFinalColor()' protected method. 
	 * 
	 * Example:
	 * 
	 * <pre>
	 * public final class GreyscalePostProcessingShader
	 * {
	 *   override protected function getFinalColor(outputColor : SValue) : SValue
	 *   {
	 *     return float4(
	 *       float3(divide(add(outputColor.r, outputColor.g, outputColor.b), 3.)),
	 *       1.
	 *     );
	 *   }
	 * }
	 * </pre>
	 * 
	 * It can then be set on the Viewport.postProcessingEffect property:
	 * 
	 * <pre>
	 * _viewport.postProcessingEffect = new SinglePassPostProcessingEffect(new );
	 * </pre>
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class PostProcessingActionScriptShader extends ActionScriptShader
	{
		public static const BACKBUFFER_STYLE_ID	: int	= Style.getStyleId("post processing backbuffer");
		
		public function PostProcessingActionScriptShader(name : String = null)
		{
			super(name);
		}
		
		protected final function sampleBackBuffer(uv 			: Object,
												  filtering		: uint	= 1,
												  mipMapping	: uint	= 2,
												  wrapping		: uint	= 1) : SValue
		{
			return sampleTexture(
				BACKBUFFER_STYLE_ID,
				uv,
				filtering,
				mipMapping,
				wrapping
			);
		}
		
		/**
		 * Returns the clipspace vertex position without modifications because
		 * this is what most of post-processing shaders will do. The method
		 * can still be overriden for any special use case.
		 * 
		 * @return The vertex position.
		 * 
		 */
		override protected function getOutputPosition() : SValue
		{
			return vertexPosition;
		}
		
		override protected function getOutputColor() : SValue
		{
			return getFinalColor(sampleBackBuffer(interpolate(vertexUV)));
		}
		
		/**
		 * The method that should be overriden to define the post-processing
		 * computations.
		 * 
		 * @param outputColor The input color read from the backbuffer.
		 * @return The final output color of the pixel on the screen.
		 * 
		 */
		protected function getFinalColor(outputColor : SValue) : SValue
		{
			throw new Error("The 'getFinalColor()' method should be overriden.");
		}
	}
}