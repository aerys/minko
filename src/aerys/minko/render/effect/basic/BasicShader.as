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