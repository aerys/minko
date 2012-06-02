package aerys.minko.render.shader.part
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
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
			var diffuseColor : SFloat	= null;
			
			if (meshBindings.propertyExists('diffuseMap'))
			{
				var diffuseMap	: SFloat	= meshBindings.getTextureParameter(
					'diffuseMap',
					meshBindings.getConstant('diffuseFiltering', SamplerFiltering.LINEAR),
					meshBindings.getConstant('diffuseMipMapping', SamplerMipMapping.LINEAR),
					meshBindings.getConstant('diffuseWrapping', SamplerWrapping.REPEAT)
				);
				
				diffuseColor = sampleTexture(diffuseMap,interpolate(vertexUV.xy));
			}
			else if (meshBindings.propertyExists('diffuseColor'))
			{
				diffuseColor = meshBindings.getParameter('diffuseColor', 4);
			}
			else
			{
				diffuseColor = float4(0., 0., 0., 1.);
			}
			
			// Apply HLSA modifiers
			if (meshBindings.propertyExists('diffuseColorMatrix'))
			{
				diffuseColor = multiply4x4(
					diffuseColor,
					meshBindings.getParameter('diffuseColorMatrix', 16)
				);
			}
			
			return diffuseColor;
		}
	}
}