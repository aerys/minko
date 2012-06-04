package aerys.minko.render.effect.vertex
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	
	public class VertexPositionShader extends BasicShader
	{
		private var _vertexPosition	: SFloat	= null;
		
		public function VertexPositionShader(target		: RenderTarget	= null,
											 priority	: Number		= 0.)
		{
			super(target, priority);
		}
		
		override protected function getVertexPosition() : SFloat
		{
			var screenPosition : SFloat = super.getVertexPosition();
			
			_vertexPosition = screenPosition;
			_vertexPosition = divide(add(_vertexPosition, 1), 2);
			
			return screenPosition;
		}
		
		override protected function getPixelColor() : SFloat
		{
			return float4(interpolate(_vertexPosition.xyz), 1);
		}
	}
}