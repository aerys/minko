package aerys.minko.render.shader.part
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.type.enum.SamplerFilter;
	import aerys.minko.type.enum.SamplerMipmap;
	import aerys.minko.type.enum.SamplerWrapping;
	import aerys.minko.type.stream.format.VertexComponent;
	
	public class DiffuseShaderPart extends ShaderPart
	{
		/**
		 * The shader part to use a diffuse map or fallback and use a solid color.
		 *  
		 * @param main
		 * 
		 */
		public function DiffuseShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function getDiffuse() : SFloat
		{
			if (meshBindings.propertyExists("diffuseMap"))
			{
				var diffuseMap	: SFloat	= meshBindings.getTextureParameter(
					"diffuseMap",
					meshBindings.getConstant("diffuseFiltering", SamplerFilter.LINEAR),
					meshBindings.getConstant("diffuseMipMapping", SamplerMipmap.LINEAR),
					meshBindings.getConstant("diffuseWrapping", SamplerWrapping.REPEAT)
				);
				
				return sampleTexture(
					diffuseMap,
					interpolate(getVertexAttribute(VertexComponent.UV))
				);
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