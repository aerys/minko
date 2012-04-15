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
			_vertexPosition = super.getVertexPosition();
			
			return _vertexPosition;
		}
		
		override protected function getPixelColor() : SFloat
		{
			var pos : SFloat = interpolate(_vertexPosition);
			
			pos = divide(add(pos, 1), 2);
			
			return float4(pos.xyz, 1);
		}
	}
}