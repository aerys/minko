package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
	import aerys.minko.render.shader.part.projection.ParaboloidProjectionShaderPart;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
	import flash.geom.Rectangle;
	
	public class DPShadowMapAttenuationShaderPart extends LightAwareShaderPart implements IAttenuationShaderPart
	{
		private static const TEXTURE_RECTANGLE	: Rectangle	= new Rectangle(0, 0, 1, 1);
		private static const DEFAULT_BIAS		: Number	= 0.2;
		
		private var _paraboloidFrontPart	: ParaboloidProjectionShaderPart;
		private var _paraboloidBackPart		: ParaboloidProjectionShaderPart;
		
		public function DPShadowMapAttenuationShaderPart(main : Shader)
		{
			super(main);
			
			_paraboloidFrontPart	= new ParaboloidProjectionShaderPart(main, true);
			_paraboloidBackPart		= new ParaboloidProjectionShaderPart(main, false);
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
			
			// retrieve shadow maps and tranform matrix
			var worldToLight	: SFloat = getLightParameter(lightId, 'worldToLocal', 16);
			
			var frontDepthMap	: SFloat = getLightTextureParameter(
				lightId,
				'shadowMapFront', 
				SamplerFiltering.LINEAR, 
				SamplerMipMapping.DISABLE, 
				SamplerWrapping.CLAMP
			);
			
			var backDepthMap	: SFloat = getLightTextureParameter(
				lightId,
				'shadowMapBack',
				SamplerFiltering.LINEAR, 
				SamplerMipMapping.DISABLE, 
				SamplerWrapping.CLAMP
			);
			
			// transform position to light space
			var positionFromLight		: SFloat = interpolate(
				multiply4x4(vsWorldPosition, worldToLight)
			);
			var isFront					: SFloat = greaterEqual(positionFromLight.z, 0);
			
			// retrieve front depth
			var uvFront					: SFloat = _paraboloidFrontPart.projectVector(
				positionFromLight, TEXTURE_RECTANGLE, 0.01, 1000
			);
			var frontPrecomputedDepth	: SFloat = unpack(sampleTexture(frontDepthMap, uvFront));
			// retrieve back depth
			var uvBack					: SFloat = _paraboloidBackPart.projectVector(
				positionFromLight, TEXTURE_RECTANGLE, 0.01, 1000
			);
			var backPrecomputedDepth	: SFloat = unpack(sampleTexture(backDepthMap, uvBack));
		
			var currentDepth			: SFloat = mix(uvBack.z, uvFront.z, isFront);
			var precomputedDepth		: SFloat = mix(
				backPrecomputedDepth, frontPrecomputedDepth, isFront
			);
			
			return lessThan(currentDepth, add(shadowBias, precomputedDepth));
		}
	}
}
