package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
	import aerys.minko.render.shader.part.phong.depth.IDepthFromLightShaderPart;
	import aerys.minko.render.shader.part.phong.depth.LinearDepthFromLightShaderPart;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.type.enum.SamplerDimension;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
	public final class VarianceShadowMapAttenuationShaderPart extends LightAwareShaderPart implements IAttenuationShaderPart
	{
		private static const MIN_VARIANCE		: Number			= 0.002;
		private static const EPSILON			: Number			= 0.0001;
		private var _depthShaderPart			: IDepthFromLightShaderPart	= null;
		
		public function VarianceShadowMapAttenuationShaderPart(main : Shader)
		{
			super(main);
		}

		private function createDepthShaderPart() : void
		{
			_depthShaderPart = new LinearDepthFromLightShaderPart(this.main);
		}

		
		private function chebyshevUpperBound(moment1					: SFloat,
											 moment2					: SFloat,
											 depth						: SFloat,
											 lightBleedingCorrection	: Function,
											 lightBleedingBias 			: SFloat) : SFloat
		{
			var p				: SFloat	= lessThan(depth, moment1);
			var e_x2			: SFloat	= moment2;						// e(x^2)
			var ex_2			: SFloat	= multiply(moment1, moment1);	// e(x)^2
			var variance		: SFloat	= subtract(e_x2, ex_2);
			variance						= add(variance, EPSILON);
			variance						= max(variance, MIN_VARIANCE);
			var dist			: SFloat	= subtract(depth, moment1);
			var distSquared		: SFloat	= multiply(dist, dist);
			var pMax			: SFloat	= divide(variance, add(variance, distSquared));
			pMax							= lightBleedingCorrection(pMax, lightBleedingBias);

			return max(p, pMax);
		}
		
		private function doNotReduceLightBleeding(pMax : SFloat, amount : SFloat) : SFloat
		{
			return pMax;
		}
		
		private function reduceLightBleedingWithLinstep(pMax : SFloat, amount : SFloat) : SFloat
		{
			return linstep(amount, 1.0, pMax);
		}
		
		private function reduceLightBleedingWithSmoothstep(pMax : SFloat, amount : SFloat) : SFloat
		{
			return smoothstep(amount, 1.0, pMax);
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
			var precomputedDepth			: SFloat	= sampleTexture(depthMap, uv);
			var moment1						: SFloat	= unpackHalf(precomputedDepth.xy);
			var moment2						: SFloat	= unpackHalf(precomputedDepth.zw);
			var lightBleedingCorrection		: Function	= doNotReduceLightBleeding;
			var lightBleedingInterpolation	: uint		= getLightProperty(
				lightId,
				'lightBleedingInterpolation',
				LightBleedingInterpolation.NONE
			);
			switch (lightBleedingInterpolation)
			{
				case LightBleedingInterpolation.LINSTEP :
					lightBleedingCorrection = reduceLightBleedingWithLinstep;
					break;
				case LightBleedingInterpolation.SMOOTHSTEP :
					lightBleedingCorrection = reduceLightBleedingWithSmoothstep;
					break;
			}
			var lightBleedingBias			: SFloat	= getLightParameter(lightId, 'lightBleedingBias', 1);
			var shadow						: SFloat	= chebyshevUpperBound(
				moment1,
				moment2,
				depth,
				lightBleedingCorrection,
				lightBleedingBias
			);
			
		//	shadow = coloredShadow(lightId, shadow);
						
			var insideShadow 	: SFloat = and(and(lessThan(uv.x, 1), greaterEqual(uv.x, 0)), and(lessThan(uv.y, 1), greaterEqual(uv.y, 0)));
			var outsideShadow	: SFloat = subtract(1, insideShadow);
			
			return add(shadow.scaleBy(insideShadow), outsideShadow);
		}
	}
}