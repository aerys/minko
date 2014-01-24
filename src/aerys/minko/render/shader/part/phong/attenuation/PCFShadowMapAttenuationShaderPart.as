package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
	import aerys.minko.render.shader.part.phong.depth.LinearDepthFromLightShaderPart;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.type.enum.SamplerDimension;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	import aerys.minko.type.enum.ShadowMappingQuality;
	
	/**
	 * Fixme, bias should be:Total bias is m*SLOPESCALE + DEPTHBIAS
	 * Where m = max( | ∂z/∂x | , | ∂z/∂y | )
	 * ftp://download.nvidia.com/developer/presentations/2004/GPU_Jackpot/Shadow_Mapping.pdf
	 * 
	 * @author Romain Gilliotte
	 */
	public class PCFShadowMapAttenuationShaderPart extends LightAwareShaderPart implements IAttenuationShaderPart
	{
		private var _depthShaderPart	: LinearDepthFromLightShaderPart;
		public function PCFShadowMapAttenuationShaderPart(main : Shader)
		{
			super(main);
			_depthShaderPart = new LinearDepthFromLightShaderPart(main);
		}
		
		public function getAttenuation(lightId : uint) : SFloat
		{
			// retrieve shadow bias
			var shadowBias : SFloat;
			if (meshBindings.propertyExists(PhongProperties.SHADOW_BIAS))
				shadowBias = meshBindings.getParameter(PhongProperties.SHADOW_BIAS, 1);
			else
				shadowBias = getLightParameter(lightId, PhongProperties.SHADOW_BIAS, 1);
			
			// retrieve depthmap matrix
			var lightType			: uint		= getLightProperty(lightId, 'type');
						
			var depthMap	: SFloat = getLightTextureParameter(
				lightId,
				'shadowMap', 
				SamplerFiltering.NEAREST, 
				SamplerMipMapping.DISABLE, 
				SamplerWrapping.CLAMP,
				lightType == PointLight.LIGHT_TYPE ? SamplerDimension.CUBE : SamplerDimension.FLAT
			);
			
			var fsWorldPosition 	: SFloat 	= interpolate(vsWorldPosition);
			var uv 					: SFloat 	= _depthShaderPart.getUV(lightId, fsWorldPosition);
			var currentDepth 		: SFloat 	= _depthShaderPart.getDepthForAttenuation(lightId, fsWorldPosition);
			var precomputedDepth	: SFloat 	= unpack(sampleTexture(depthMap, uv));
			var curDepthSubBias		: SFloat 	= min(subtract(1, shadowBias), currentDepth);
			var noShadows			: SFloat 	= lessThan(curDepthSubBias, add(shadowBias, precomputedDepth));

			if (lightType == PointLight.LIGHT_TYPE)
				return coloredShadow(lightId, noShadows);
			
			var quality				: uint		= getLightProperty(lightId, 'shadowQuality');
			if (quality != ShadowMappingQuality.HARD)
			{
				var invertSize	: Number = getLightProperty(lightId, 'shadowSpread', 1) / getLightProperty(lightId, 'shadowMapSize', 2048);
				
				var uvs 		: Vector.<SFloat>	= new <SFloat>[];
				var uvDelta		: SFloat;
				
				var uvXYXY : SFloat = uv.xyxy;
				
				
				if (quality > ShadowMappingQuality.LOW)
				{
					uvDelta = multiply(float3(-1, 0, 1), invertSize);
					uvs.push(
						add(uvXYXY, uvDelta.xxxy),//XXXY),	// (-1, -1), (-1,  0)
						add(uvXYXY, uvDelta.xzyx),	// (-1,  1), ( 0, -1)
						add(uvXYXY, uvDelta.yzzx),	// ( 0,  1), ( 1, -1)
						add(uvXYXY, uvDelta.zyzz)	// ( 1,  0), ( 1,  1)
					);
				}
				
				if (quality > ShadowMappingQuality.MEDIUM)
				{
					uvDelta = multiply(float3(-2, 0, 2), invertSize);
					uvs.push(
						add(uvXYXY, uvDelta.xyyx),//XYYX),	// (-2, 0), (0, -2)
						add(uvXYXY, uvDelta.yzzy)	// ( 0, 2), (2, 0)
					);
				}
				
				if (quality > ShadowMappingQuality.HARD)
				{
					uvDelta = multiply(float4(-2, -1, 1, 2), invertSize);
					uvs.push(
						add(uvXYXY, uvDelta.xyzw),//XZYW),	// (-2,  1), (-1,  2)
						add(uvXYXY, uvDelta.zwwz),//ZWWZ),	// ( 1,  2), ( 2,  1)
						add(uvXYXY, uvDelta.wyzx),//WYZX),	// ( 2, -1), ( 1, -2)
						add(uvXYXY, uvDelta.xyyx)	// (-2, -1), (-1, -2)
					);
				}
				
				var numSamples : uint = uvs.length;
				for (var sampleId : uint = 0; sampleId < numSamples; sampleId += 2)
				{
					precomputedDepth = float4(
						unpack(sampleTexture(depthMap, uvs[sampleId].xy)),
						unpack(sampleTexture(depthMap, uvs[sampleId].zw)),
						unpack(sampleTexture(depthMap, uvs[sampleId + 1].xy)),
						unpack(sampleTexture(depthMap, uvs[sampleId + 1].zw))
					);
					
					var localNoShadows : SFloat = lessThan(curDepthSubBias, add(shadowBias, precomputedDepth));
					noShadows.incrementBy(dotProduct4(localNoShadows, float4(1, 1, 1, 1)));
				}
				
				noShadows.scaleBy(1 / (2 * numSamples + 1));
			}

			//noShadows = coloredShadow(lightId, noShadows);
			var uvX : SFloat = uv.x;
			var uvY : SFloat = uv.y;
			
			var insideShadow 	: SFloat = multiply(multiply(lessThan(uvX, 1), greaterEqual(uvX, 0)), multiply(lessThan(uvY, 1), greaterEqual(uvY, 0)));
			var outsideShadow	: SFloat = subtract(1, insideShadow);
			
			return add(noShadows.scaleBy(insideShadow), outsideShadow);
		}
	}
}
