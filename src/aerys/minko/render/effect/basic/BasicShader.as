package aerys.minko.render.effect.basic
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	
	public class BasicShader extends Shader
	{
		override protected function getVertexPosition() : SFloat
		{
			return localToScreen(vertexXYZ);
		}
		
		override protected function getPixelColor() : SFloat
		{
			if (localBindings.countProperty("diffuse color"))
			{
				return localBindings.getParameter("diffuse color", 4);
			}
			else if (localBindings.countProperty("diffuse map"))
			{
				 var diffuseMap	: SFloat	= localBindings.getTextureParameter("diffuse map");
				 var uv			: SFloat	= interpolate(vertexUV);
				 
				 return sampleTexture(diffuseMap, uv);
			}
			
			throw new Error("Local parameter 'diffuse color' or 'diffuse map' must be set.");
		}
	}
}