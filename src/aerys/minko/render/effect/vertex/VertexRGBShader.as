package aerys.minko.render.effect.vertex
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	
	public class VertexRGBShader extends BasicShader
	{
		public function VertexRGBShader(target		: RenderTarget	= null,
										priority	: Number		= 0.)
		{
			super(target, priority);
		}
		
		override protected function getPixelColor() : SFloat
		{
			return float4(interpolate(vertexRGBColor), 1);
		}
	}
}