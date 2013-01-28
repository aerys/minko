package aerys.minko.render.shader.part.phong.attenuation
{
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.depth.DepthAlgorithm;
	import aerys.minko.render.shader.part.depth.IDepthShaderPart;
	import aerys.minko.render.shader.part.depth.LinearDepthShaderPart;
	import aerys.minko.render.shader.part.depth.ProjectedDepthShaderPart;
	import aerys.minko.render.shader.part.phong.LightAwareShaderPart;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.scene.node.light.SpotLight;
	import aerys.minko.type.enum.SamplerDimension;
	import aerys.minko.type.enum.SamplerFiltering;
	import aerys.minko.type.enum.SamplerMipMapping;
	import aerys.minko.type.enum.SamplerWrapping;
	
	public final class ExponentialShadowMapAttenuationShaderPart extends LightAwareShaderPart implements IAttenuationShaderPart
	{
		private static const EPSILON			: Number	= 0.0001;
		private var _depthShaderPart			: IDepthShaderPart;
		
		public function ExponentialShadowMapAttenuationShaderPart(main:Shader)
		{
			super(main);
		}
		
		private function createDepthShaderPart(lightId : uint) : void
		{
			var lightTypeName			: String	= LightDataProvider.getLightPropertyName('type', lightId);
			var lightType				: uint		= sceneBindings.getProperty(lightTypeName, lightId);
			if (lightType == PointLight.LIGHT_TYPE || lightType == SpotLight.LIGHT_TYPE)
			{
				_depthShaderPart					= new LinearDepthShaderPart(this.main);
			}
			else
			{
				var depthAlgorithmName	: String	= LightDataProvider.getLightPropertyName('depthAlgorithm', lightId);
				var depthAlgorithm		: uint		= sceneBindings.getProperty(depthAlgorithmName, 0);
				switch (depthAlgorithm)
				{
					case DepthAlgorithm.LINEAR :
						_depthShaderPart			= new LinearDepthShaderPart(this.main);
						break;
					case DepthAlgorithm.PROJECTED :
						_depthShaderPart			= new ProjectedDepthShaderPart(this.main);
						break;
				}
			}
		}
		
		public function getAttenuation(lightId : uint) : SFloat
		{
			createDepthShaderPart(lightId);
			
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
			var precomputedDepth			: SFloat	= unpack(sampleTexture(depthMap, uv));
			var expFactor					: SFloat	= getLightParameter(lightId, 'exponentialFactor', 1);
			expFactor									= max(expFactor, 1.);
			var shadow						: SFloat	= saturate(exp(multiply(expFactor, subtract(precomputedDepth, depth))));
			
			return shadow.x;
		}
	}
}