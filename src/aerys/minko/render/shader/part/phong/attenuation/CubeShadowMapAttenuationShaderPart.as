package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
	import aerys.minko.type.enum.SamplerDimension;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
	public class CubeShadowMapAttenuationShaderPart extends LightAwareShaderPart implements IAttenuationShaderPart
	{
		private static const DEFAULT_BIAS : Number = 1 / 10000;
		
		public function CubeShadowMapAttenuationShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function getAttenuation(lightId : uint) : SFloat
		{
			// retrieve shadow bias
			var shadowBias : SFloat;
			if (meshBindings.propertyExists(PhongProperties.SHADOW_BIAS))
				shadowBias = meshBindings.getParameter(PhongProperties.SHADOW_BIAS, 1);
			else if (sceneBindings.propertyExists(PhongProperties.SHADOW_BIAS))
				shadowBias = sceneBindings.getParameter(PhongProperties.SHADOW_BIAS, 1);
			else
				shadowBias = float(DEFAULT_BIAS);
			
			// retrieve depthmap, transformation matrix, zNear and zFar
			var worldToLight		: SFloat = getLightParameter(lightId, 'worldToLocal', 16);
			var zNear				: SFloat = getLightParameter(lightId, 'shadowZNear', 1);
			var zFar				: SFloat = getLightParameter(lightId, 'shadowZFar', 1);
			var cubeDepthMap		: SFloat = getLightTextureParameter(lightId, 'shadowMap', 
				SamplerFiltering.NEAREST,
				SamplerMipMapping.DISABLE, 
				SamplerWrapping.CLAMP, 
				SamplerDimension.CUBE
			);
			
			// retrieve precompute depth
			var positionFromLight	: SFloat = interpolate(multiply4x4(vsWorldPosition, worldToLight));
			var precomputedDepth	: SFloat = unpack(sampleTexture(cubeDepthMap, positionFromLight));
			
			// retrieve real depth
			var currentDepth		: SFloat = divide(subtract(length(positionFromLight.xyz), zNear), subtract(zFar, zNear));
			currentDepth = min(subtract(1, shadowBias), currentDepth);
			
			return lessEqual(currentDepth, add(shadowBias, precomputedDepth));
		}
	}
}
