package aerys.minko.render.shader.part
{
	import aerys.minko.render.effect.basic.BasicProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	
	public class NormalMappingShaderPart extends ShaderPart
	{
		public function NormalMappingShaderPart(main : Shader)
		{
			super(main);
		}
		
		/**
		 * 
		 * @param vertexNormal Non-interpolated normal. This normal can be modified if the model is skinned or morphed. The most common use is to feed the output of _vertexAnimation.getAnimatedNormal()
		 * @return Perturbed normal in the fragment shader.
		 * 
		 */		
		public function getPerturbedNormal(vertexUV		: SFloat,
										   vertexNormal	: SFloat) : SFloat
		{
			// first try to recover perturbed normal from an object space normal map
			if (meshBindings.propertyExists(BasicProperties.OBJ_SPACE_NORMAL_MAP))
			{
				var objSpaceNormalMap	: SFloat = meshBindings.getTextureParameter(BasicProperties.OBJ_SPACE_NORMAL_MAP);
				var objSpaceNormal		: SFloat = sampleTexture(objSpaceNormalMap, interpolate(vertexUV));
				
				return objSpaceNormal;
			}
			else if (meshBindings.propertyExists(BasicProperties.TGT_SPACE_NORMAL_MAP))
			{
				return null;
			}
			else
			{
				return null;
			}
		}
	}
}