package aerys.minko.render.shader.part
{
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
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
		
		public function getDiffuseColor(killOnAlphaThreshold : Boolean = true) : SFloat
		{
			var diffuseColor : SFloat	= null;
			
			if (meshBindings.propertyExists(BasicProperties.DIFFUSE_MAP))
			{
				var uv			: SFloat	= vertexUV.xy;
				var diffuseMap	: SFloat	= meshBindings.getTextureParameter(
					BasicProperties.DIFFUSE_MAP,
					meshBindings.getConstant(BasicProperties.DIFFUSE_FILTERING, SamplerFiltering.LINEAR),
					meshBindings.getConstant(BasicProperties.DIFFUSE_MIPMAPPING, SamplerMipMapping.LINEAR),
					meshBindings.getConstant(BasicProperties.DIFFUSE_WRAPPING, SamplerWrapping.REPEAT)
				);
				
				if (meshBindings.propertyExists(BasicProperties.DIFFUSE_UV_SCALE))
					uv.scaleBy(meshBindings.getParameter(BasicProperties.DIFFUSE_UV_SCALE, 2));
				
				if (meshBindings.propertyExists(BasicProperties.DIFFUSE_UV_OFFSET))
					uv.incrementBy(meshBindings.getParameter(BasicProperties.DIFFUSE_UV_OFFSET, 2));
				
				diffuseColor = sampleTexture(diffuseMap,interpolate(uv));
			}
			else if (meshBindings.propertyExists(BasicProperties.DIFFUSE_COLOR))
			{
				diffuseColor = meshBindings.getParameter(BasicProperties.DIFFUSE_COLOR, 4);
			}
			else
			{
				diffuseColor = float4(0., 0., 0., 1.);
			}
			
			// Apply HLSA modifiers
			if (meshBindings.propertyExists(BasicProperties.DIFFUSE_TRANSFORM))
			{
				diffuseColor = multiply4x4(
					diffuseColor,
					meshBindings.getParameter(BasicProperties.DIFFUSE_TRANSFORM, 16)
				);
			}
			
			if (killOnAlphaThreshold && meshBindings.propertyExists(BasicProperties.ALPHA_THRESHOLD))
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