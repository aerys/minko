package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
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
		public function PCFShadowMapAttenuationShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function getAttenuation(lightId : uint) : SFloat
		{
			var lightType	: uint		= getLightConstant(lightId, 'type');
			var screenPos	: SFloat	= interpolate(localToScreen(fsLocalPosition));
			
			// retrieve shadow bias
			var shadowBias : SFloat;
			if (meshBindings.propertyExists(PhongProperties.SHADOW_BIAS))
				shadowBias = meshBindings.getParameter(PhongProperties.SHADOW_BIAS, 1);
			else
				shadowBias = getLightParameter(lightId, PhongProperties.SHADOW_BIAS, 1);
			
			// retrieve depthmap and projection matrix
			var worldToUV	: SFloat = getLightParameter(lightId, 'worldToUV', 16);
			var depthMap	: SFloat = getLightTextureParameter(
				lightId,
				'shadowMap', 
				SamplerFiltering.NEAREST, 
				SamplerMipMapping.DISABLE, 
				SamplerWrapping.CLAMP
			);
			
			// read expected depth from shadow map, and compute current depth
			var uv : SFloat;
			uv = multiply4x4(vsWorldPosition, worldToUV);
			uv = interpolate(uv);
			
			var currentDepth : SFloat = uv.z;
			currentDepth = min(subtract(1, shadowBias), currentDepth);
            
			uv = divide(uv, uv.w);
			
			var outsideMap			: SFloat = notEqual(
                0,
                dotProduct4(notEqual(uv, saturate(uv)), notEqual(uv, saturate(uv)))
            );
			
			var precomputedDepth	: SFloat = unpack(sampleTexture(depthMap, uv.xyyy));
			
			var curDepthSubBias		: SFloat = subtract(currentDepth, shadowBias);
			var noShadows			: SFloat = lessEqual(curDepthSubBias, precomputedDepth);
			
			var quality		: uint		= getLightConstant(lightId, 'shadowQuality');
			
			if (quality != ShadowMappingQuality.HARD)
			{
				var invertSize	: SFloat			= divide(
					getLightParameter(lightId, 'shadowSpread', 1),
					getLightParameter(lightId, 'shadowMapSize', 1)
				);
				
				var uvs 		: Vector.<SFloat>	= new <SFloat>[];
				var uvDelta		: SFloat;
				
				if (quality > ShadowMappingQuality.LOW)
				{
					uvDelta = multiply(float3(-1, 0, 1), invertSize);
					uvs.push(
						add(uv.xyxy, uvDelta.xxxy),	// (-1, -1), (-1,  0)
						add(uv.xyxy, uvDelta.xzyx),	// (-1,  1), ( 0, -1)
						add(uv.xyxy, uvDelta.yzzx),	// ( 0,  1), ( 1, -1)
						add(uv.xyxy, uvDelta.zyzz)	// ( 1,  0), ( 1,  1)
					);
				}
				
				if (quality > ShadowMappingQuality.MEDIUM)
				{
					uvDelta = multiply(float3(-2, 0, 2), invertSize);
					uvs.push(
						add(uv.xyxy, uvDelta.xyyx),	// (-2, 0), (0, -2)
						add(uv.xyxy, uvDelta.yzzy)	// ( 0, 2), (2, 0)
					);
				}
				
				if (quality > ShadowMappingQuality.HARD)
				{
					uvDelta = multiply(float4(-2, -1, 1, 2), invertSize);
					uvs.push(
						add(uv.xyxy, uvDelta.xzyw),	// (-2,  1), (-1,  2)
						add(uv.xyxy, uvDelta.zwwz),	// ( 1,  2), ( 2,  1)
						add(uv.xyxy, uvDelta.wyzx),	// ( 2, -1), ( 1, -2)
						add(uv.xyxy, uvDelta.xyyx)	// (-2, -1), (-1, -2)
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
					
					var localNoShadows : SFloat = lessEqual(curDepthSubBias, precomputedDepth);
					noShadows.incrementBy(dotProduct4(localNoShadows, float4(1, 1, 1, 1)));
				}
				
				noShadows.scaleBy(1 / (2 * numSamples + 1));
			}
			
			return noShadows;
		}
	}
}
