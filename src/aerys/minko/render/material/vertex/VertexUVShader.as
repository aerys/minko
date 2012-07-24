package aerys.minko.render.material.vertex
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.type.stream.format.VertexComponent;
	
	public class VertexUVShader extends BasicShader
	{
		
		public function VertexUVShader(target 	: RenderTarget 	= null, 
									   priority	: Number		= 0)
		{
			super(target, priority);
		}
		
		override protected function getPixelColor() : SFloat
		{
			var uv 				: SFloat = getVertexAttribute(VertexComponent.UV);
			var interpolatedUv 	: SFloat = normalize(interpolate(uv));
			
			
			return float4(interpolatedUv.x, interpolatedUv.y, 0, 1);
		}
		
	}
}