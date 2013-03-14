package aerys.minko.render.shader.part.phong
{
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerFormat;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
	public class LightAwareDiffuseShaderPart extends LightAwareShaderPart
	{
		public function LightAwareDiffuseShaderPart(main:Shader)
		{
			super(main);
		}
		
		public function getDiffuseColor() : SFloat
		{
			var diffuseColor : SFloat	= null;
			var uv			 : SFloat	= fsUV;
			
			if (meshBindings.propertyExists(BasicProperties.DIFFUSE_MAP))
			{
				var diffuseMap	: SFloat	= meshBindings.getTextureParameter(
					BasicProperties.DIFFUSE_MAP,
					meshBindings.getProperty(BasicProperties.DIFFUSE_MAP_FILTERING, SamplerFiltering.LINEAR),
					meshBindings.getProperty(BasicProperties.DIFFUSE_MAP_MIPMAPPING, SamplerMipMapping.LINEAR),
					meshBindings.getProperty(BasicProperties.DIFFUSE_MAP_WRAPPING, SamplerWrapping.REPEAT),
                    0,
                    meshBindings.getProperty(BasicProperties.DIFFUSE_MAP_FORMAT, SamplerFormat.RGBA)
				);
				
				diffuseColor = sampleTexture(diffuseMap, uv);
			}
			else if (meshBindings.propertyExists(BasicProperties.DIFFUSE_COLOR))
			{
				diffuseColor = meshBindings.getParameter(BasicProperties.DIFFUSE_COLOR, 4);
			}
			else
			{
				diffuseColor = float4(0., 0., 0., 1.);
			}
			
			if (meshBindings.propertyExists(BasicProperties.ALPHA_MAP))
			{				
				var alphaMap 	: SFloat 	= meshBindings.getTextureParameter(
					BasicProperties.ALPHA_MAP,
					meshBindings.getProperty(BasicProperties.ALPHA_MAP_FILTERING, SamplerFiltering.LINEAR),
					meshBindings.getProperty(BasicProperties.ALPHA_MAP_MIPMAPPING, SamplerMipMapping.LINEAR),
					meshBindings.getProperty(BasicProperties.ALPHA_MAP_WRAPPING, SamplerWrapping.REPEAT),
					0,
					meshBindings.getProperty(BasicProperties.ALPHA_MAP_FORMAT, SamplerFormat.RGBA)
				);
				var alphaSample	: SFloat	= sampleTexture(alphaMap, uv);
				
				diffuseColor = float4(diffuseColor.rgb, alphaSample.r);					
			}
			
			if (meshBindings.propertyExists(BasicProperties.DIFFUSE_TRANSFORM))
			{
				diffuseColor = multiply4x4(
					diffuseColor,
					meshBindings.getParameter(BasicProperties.DIFFUSE_TRANSFORM, 16)
				);
			}
			
			if (meshBindings.propertyExists(BasicProperties.ALPHA_THRESHOLD))
			{
				var alphaThreshold : SFloat = meshBindings.getParameter(
					BasicProperties.ALPHA_THRESHOLD, 1
				);
				
				kill(subtract(0.5, lessThan(diffuseColor.w, alphaThreshold)));
			}
			
			return diffuseColor;
		}
	}
}