package aerys.minko.render.effect.basic
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.ShaderTemplate;
	import aerys.minko.type.enum.Blending;
	
	public class ColorShader extends ShaderTemplate
	{
		public function ColorShader()
		{
			super();
			
			drawCallTemplate.blending = Blending.ALPHA;
		}
		
		override protected function getClipspacePosition():SFloat
		{
			return localToScreen(vertexXYZ);
		}
		
		override protected function getPixelColor():SFloat
		{
			return getParameter("color", 4);
		}
	}
}