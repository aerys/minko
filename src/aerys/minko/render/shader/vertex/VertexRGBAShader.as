package aerys.minko.render.shader.vertex
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	
	public class VertexRGBAShader extends BasicShader
	{
		public function VertexRGBAShader(target		: RenderTarget	= null,
										 priority	: Number		= 0.)
		{
			super(target, priority);
		}
		
		override protected function getPixelColor() : SFloat
		{
			return float4(interpolate(vertexRGBAColor));
			//return float4(interpolate(vertexRGBAColor), 1);
		}
	}
}