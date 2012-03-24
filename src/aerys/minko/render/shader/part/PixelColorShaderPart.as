package aerys.minko.render.shader.part
{
	import aerys.minko.render.shader.PassTemplate;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.type.stream.format.VertexComponent;
	
	public class PixelColorShaderPart extends ShaderPart
	{
		/**
		 * The shader part to use a diffuse map or fallback and use a solid color.
		 *  
		 * @param main
		 * 
		 */
		public function PixelColorShaderPart(main : PassTemplate)
		{
			super(main);
		}
		
		public function getPixelColor() : SFloat
		{
			if (meshBindings.propertyExists("diffuseMap"))
			{
				var diffuseMap	: SFloat	= meshBindings.getTextureParameter("diffuseMap");
				var uv			: SFloat	= interpolate(getVertexAttribute(VertexComponent.UV));
				
				return sampleTexture(diffuseMap, uv);
			}
			else if (meshBindings.propertyExists("diffuseColor"))
			{
				return meshBindings.getParameter("diffuseColor", 4);
			}
			
			throw new Error(
				"Local parameter 'diffuseColor' or 'diffuseMap' must be set."
			);
		}
	}
}