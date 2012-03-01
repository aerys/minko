package aerys.minko.render.shader.parts
{
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.ShaderPart;
	
	public class PixelColorShaderPart extends ShaderPart
	{
		public function PixelColorShaderPart(main:ActionScriptShader)
		{
			super(main);
		}
		
		public function getPixelColor() : SFloat
		{
			if (meshBindings.propertyExists("diffuse map"))
			{
				var diffuseMap	: SFloat	= meshBindings.getTextureParameter("diffuse map");
				var uv			: SFloat	= interpolate(vertexUV);
				
				return sampleTexture(diffuseMap, uv);
			}
			else if (meshBindings.propertyExists("diffuse color"))
			{
				return meshBindings.getParameter("diffuse color", 4);
			}
			
			throw new Error(
				"Local parameter 'diffuse color' or 'diffuse map' must be set."
			);
		}
	}
}