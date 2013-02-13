package aerys.minko.render.shader.vertex
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.material.basic.BasicShader;
	import aerys.minko.render.shader.SFloat;
	
	public class VertexDepthShader extends BasicShader
	{
		private var _vertexPosition	: SFloat	= null;
		
		public function VertexDepthShader(target	: RenderTarget	= null,
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
			return pack(interpolate(divide(_vertexPosition.z, _vertexPosition.w)));
		}
	}
}