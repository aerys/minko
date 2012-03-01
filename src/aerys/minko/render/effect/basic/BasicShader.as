package aerys.minko.render.effect.basic
{
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.parts.PixelColorShaderPart;
	import aerys.minko.render.shader.parts.animation.SkinningShaderPart;
	import aerys.minko.render.shader.parts.animation.VertexAnimationShaderPart;
	
	public class BasicShader extends ActionScriptShader
	{
		private var _vertexAnimationPart	: VertexAnimationShaderPart;
		private var _pixelColorPart			: PixelColorShaderPart;
		
		public function BasicShader()
		{
			super();
			
			_vertexAnimationPart	= new VertexAnimationShaderPart(this);
			_pixelColorPart			= new PixelColorShaderPart(this);
		}
		
		override protected function getVertexPosition() : SFloat
		{
			var vertexPosition	: SFloat = _vertexAnimationPart.getAnimatedVertexPosition();
			
			return localToScreen(vertexPosition);
		}
		
		override protected function getPixelColor() : SFloat
		{
			return _pixelColorPart.getPixelColor();
		}
	}
}