package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.depth.IDepthShaderPart;
	import aerys.minko.render.shader.part.depth.LinearDepthShaderPart;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
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
		private var _depthShaderPart			: IDepthShaderPart	= null;
		
		public function VarianceShadowMapAttenuationShaderPart(main:Shader)
		{
			super(main);
		}

		private function createDepthShaderPart() : void
		{
			_depthShaderPart = new LinearDepthShaderPart(this.main);
		}

		
		private function chebyshevUpperBound(moments					: SFloat,
											 depth						: SFloat,
											 lightBleedingCorrection	: Function,
											 lightBleedingBias 			: SFloat) : SFloat
		{
			var p				: SFloat	= lessEqual(depth, moments.x);
			var e_x2			: SFloat	= moments.y;						// e(x^2)
			var ex_2			: SFloat	= multiply(moments.x, moments.x);	// e(x)^2
			var variance		: SFloat	= subtract(e_x2, ex_2);
			variance						= add(variance, EPSILON);
			variance						= clamp(variance, MIN_VARIANCE, 1.0);
			var dist			: SFloat	= subtract(depth, moments.x);
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
				SamplerMipMapping.LINEAR,
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
			var lightBleedingInterpolation	: uint		= getLightConstant(
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
				float2(moment1, moment2),
				depth,
				lightBleedingCorrection,
				lightBleedingBias
			);
			
			return shadow.x;
		}
	}
}