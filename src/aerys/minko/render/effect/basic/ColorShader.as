package aerys.minko.render.effect.basic
{
	import aerys.minko.render.shader.SValue;
	import aerys.minko.render.shader.ShaderTemplate;
	import aerys.minko.type.enum.Blending;
	
	public class ColorShader extends ShaderTemplate
	{
		public function ColorShader()
		{
			super();
			
			drawCallTemplate.blending = Blending.ALPHA;
		}
		
		override protected function getClipspacePosition():SValue
		{
			return localToScreen(vertexXYZ);
		}
		
		override protected function getPixelColor():SValue
		{
			return getParameter("color", 4);
		}
	}
}