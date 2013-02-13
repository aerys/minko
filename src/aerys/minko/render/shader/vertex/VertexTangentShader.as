package aerys.minko.render.shader.vertex
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	
	public class VertexTangentShader extends BasicShader
	{
		private var _tangent	: SFloat;
		
		public function VertexTangentShader(renderTarget 	: RenderTarget	= null,
											priority		: Number		= 0.0)
		{
			super(renderTarget, priority);
		}
		
		override protected function getVertexPosition() : SFloat
		{
			_tangent = deltaLocalToWorld(vertexAnimation.getAnimatedVertexTangent());
			_tangent = normalize(_tangent);
			_tangent = divide(add(_tangent, 1), 2);
			
			return super.getVertexPosition();
		}
		
		override protected function getPixelColor() : SFloat
		{
			return float4(normalize(interpolate(_tangent.xyz)), 1);
		}
	}
}