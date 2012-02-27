package aerys.minko.render.effect.basic
{
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.parts.SkinningShaderPart;
	
	public class BasicShader extends ActionScriptShader
	{
		private var _skinning	: SkinningShaderPart	= null;
		
		public function BasicShader()
		{
			super();
			
			_skinning = new SkinningShaderPart(this);
		}
		
		override protected function getVertexPosition() : SFloat
		{
			var vertexPosition	: SFloat = vertexXYZ;
			
			if (meshBindings.propertyExists("skinningMethod"))
				vertexPosition = _skinning.skinPosition(vertexPosition);
			
			return localToScreen(vertexPosition);
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