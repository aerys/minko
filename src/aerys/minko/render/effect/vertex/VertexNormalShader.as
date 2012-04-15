package aerys.minko.render.effect.vertex
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	
	public final class VertexNormalShader extends BasicShader
	{
		public function VertexNormalShader(target	: RenderTarget	= null,
									  priority	: Number		= 0.)
		{
			super(target, priority);
		}
		
		override protected function getPixelColor() : SFloat
		{
			var normal : SFloat = interpolate(
				vertexAnimation.getAnimatedVertexNormal()
			);
			
			normal = normalize(normal);
			normal = divide(add(normal, 1), 2);
			
			return float4(normal.xyz, 1);
		}
	}
}