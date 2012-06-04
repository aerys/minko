package aerys.minko.render.effect.vertex
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	
	public class VertexNormalShader extends BasicShader
	{
		private var _normal	: SFloat	= null;
		
		public function VertexNormalShader(target	: RenderTarget	= null,
										   priority	: Number		= 0.)
		{
			super(target, priority);
		}
		
		override protected function getVertexPosition() : SFloat
		{
			_normal = deltaLocalToWorld(vertexAnimation.getAnimatedVertexNormal());
			_normal = normalize(_normal);
			_normal = divide(add(_normal, 1), 2);
			
			return super.getVertexPosition();
		}
		
		override protected function getPixelColor() : SFloat
		{
			return float4(normalize(interpolate(_normal.xyz)), 1);
		}
	}
}