package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.depth.IDepthFromLightShaderPart;
	import aerys.minko.render.shader.part.phong.depth.LinearDepthFromLightShaderPart;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.type.enum.SamplerDimension;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
	public final class ExponentialShadowMapAttenuationShaderPart extends LightAwareShaderPart implements IAttenuationShaderPart
	{
		private var _depthShaderPart			: IDepthFromLightShaderPart;
		
		public function ExponentialShadowMapAttenuationShaderPart(main : Shader)
		{
			super(main);
		}
		
		private function createDepthShaderPart() : void
		{
			_depthShaderPart = new LinearDepthFromLightShaderPart(this.main);
		}
		
		public function getAttenuation(lightId : uint) : SFloat
		{
			createDepthShaderPart();
			
			var lightTypeName				: String 	= LightDataProvider.getLightPropertyName('type', lightId);
			var lightType					: uint		= sceneBindings.getProperty(lightTypeName);
			var dimension					: uint		= lightType == PointLight.LIGHT_TYPE ? SamplerDimension.CUBE : SamplerDimension.FLAT;
			var depthMap					: SFloat	= getLightTextureParameter(
				lightId,
				'shadowMap',
				SamplerFiltering.LINEAR,
				SamplerMipMapping.DISABLE,
				SamplerWrapping.CLAMP,
				dimension
			);

			var worldPosition				: SFloat	= interpolate(vsWorldPosition);
			var uv							: SFloat	= _depthShaderPart.getUV(lightId, worldPosition);
			var depth						: SFloat	= _depthShaderPart.getDepthForAttenuation(lightId, worldPosition);
			var precomputedDepth			: SFloat	= unpack(sampleTexture(depthMap, uv));
			var expFactor					: SFloat	= getLightParameter(lightId, 'exponentialFactor', 1);
			expFactor									= max(expFactor, 1.);
			var shadow						: SFloat	= saturate(exp(multiply(expFactor, subtract(precomputedDepth, depth))));
			
			var insideShadow 	: SFloat = and(and(lessEqual(uv.x, 1), greaterThan(uv.x, 0)), and(lessEqual(uv.y, 1), greaterThan(uv.y, 0)));
			var outsideShadow	: SFloat = subtract(1, insideShadow);
			
			return add(multiply(shadow.x, insideShadow), multiply(1, outsideShadow));
		}
	}
}