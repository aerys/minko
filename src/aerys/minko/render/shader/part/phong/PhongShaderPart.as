package aerys.minko.render.shader.part.phong
{
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.attenuation.CubeShadowMapAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.DPShadowMapAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.DistanceAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.HardConicAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.IAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.MatrixShadowMapAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.SmoothConicAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.contribution.InfiniteShaderPart;
	import aerys.minko.render.shader.part.phong.contribution.LocalizedShaderPart;
	import aerys.minko.type.enum.NormalMappingType;
	import aerys.minko.type.enum.ShadowMappingType;
	
	/**
	 * This shader part compute the lighting contribution of all lights
	 * 
	 * @author Romain Gilliotte
	 */	
	public class PhongShaderPart extends LightAwareShaderPart
	{
		private const LIGHT_TYPE_TO_FACTORY : Vector.<Function> = new <Function>[
			getAmbientLightContribution,
			getDirectionalLightContribution,
			getPointLightContribution,
			getSpotLightContribution
		];
		
		private var _shadowAttenuators				: Vector.<IAttenuationShaderPart>;
		private var _infinitePart					: InfiniteShaderPart;
		private var _localizedPart					: LocalizedShaderPart;
		private var _distanceAttenuationPart		: DistanceAttenuationShaderPart;
		private var _smoothConicAttenuationPart		: SmoothConicAttenuationShaderPart;
		private var _hardConicAttenuationPart		: HardConicAttenuationShaderPart;
		
		private function get infinitePart() : InfiniteShaderPart
		{
			_infinitePart ||= new InfiniteShaderPart(main);
			return _infinitePart;
		}
		
		private function get localizedPart() : LocalizedShaderPart
		{
			_localizedPart ||= new LocalizedShaderPart(main);
			return _localizedPart;
		}
		
		private function get distanceAttenuationPart() : DistanceAttenuationShaderPart
		{
			_distanceAttenuationPart ||= new DistanceAttenuationShaderPart(main);
			return _distanceAttenuationPart;
		}
		
		private function get smoothConicAttenuationPart() : SmoothConicAttenuationShaderPart
		{
			_smoothConicAttenuationPart ||= new SmoothConicAttenuationShaderPart(main);
			return _smoothConicAttenuationPart;
		}
		
		private function get hardConicAttenuationPart() : HardConicAttenuationShaderPart
		{
			_hardConicAttenuationPart ||= new HardConicAttenuationShaderPart(main);
			return _hardConicAttenuationPart;
		}
		
		private function get shadowAttenuators() : Vector.<IAttenuationShaderPart>
		{
			if (!_shadowAttenuators)
			{
				var main : Shader = this.main;
				
				_shadowAttenuators = new <IAttenuationShaderPart>[
					null,
					new MatrixShadowMapAttenuationShaderPart(main),
					new DPShadowMapAttenuationShaderPart(main),
					new CubeShadowMapAttenuationShaderPart(main)
				];
			}
			
			return _shadowAttenuators
		}
		
		public function PhongShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function getLightingColor() : SFloat
		{
			var lightValue : SFloat = float3(0, 0, 0);
			
			lightValue.incrementBy(getStaticLighting());
			lightValue.incrementBy(getDynamicLighting());
			
			return lightValue;
		}
		
		private function getStaticLighting() : SFloat
		{
			var contribution : SFloat;
			
			if (meshBindings.propertyExists(PhongProperties.LIGHT_MAP))
			{
				var lightMap	: SFloat = meshBindings.getTextureParameter(PhongProperties.LIGHT_MAP);
				var uv			: SFloat = getVertexAttribute(VertexComponent.UV);
				
				contribution = sampleTexture(lightMap, interpolate(uv));
				contribution = contribution.xyz;
				
				if (meshBindings.propertyExists(PhongProperties.LIGHTMAP_MULTIPLIER))
					contribution.scaleBy(meshBindings.getParameter(PhongProperties.LIGHTMAP_MULTIPLIER, 1));
			}
			else
				contribution = float3(0, 0, 0);
			
			return contribution;
		}
		
		private function getDynamicLighting() : SFloat
		{
			var receptionMask	: uint		= meshBindings.getConstant(PhongProperties.RECEPTION_MASK, 1);
			var dynamicLighting : SFloat	= float3(0, 0, 0);
			
			for (var lightId : uint = 0; ; ++lightId)
			{
				if (!lightPropertyExists(lightId, 'emissionMask'))
					break;
				
				var emissionMask : uint = getLightConstant(lightId, 'emissionMask');
				
				if ((emissionMask & receptionMask) != 0)
				{
					var isEnabled : Boolean = lightPropertyExists(lightId, 'enabled')
										   && getLightConstant(lightId, 'enabled');
					if (!isEnabled)
					{
						continue;
					}
					
					var color			: SFloat	= getLightParameter(lightId, 'color', 4);
					var type			: uint		= getLightConstant(lightId, 'type')
					var contribution	: SFloat	= LIGHT_TYPE_TO_FACTORY[type](lightId);
					
					dynamicLighting.incrementBy(multiply(color.rgb, contribution));
				}
			}
			
			return dynamicLighting;
		}
		
		private function getAmbientLightContribution(lightId : uint) : SFloat
		{
			var ambient : SFloat = getLightParameter(lightId, 'ambient', 1);
			
			if (meshBindings.propertyExists(PhongProperties.AMBIENT_MULTIPLIER))
				ambient.scaleBy(sceneBindings.getParameter(PhongProperties.AMBIENT_MULTIPLIER, 1));
			
			return ambient;
		}
		
		private function getDirectionalLightContribution(lightId : uint) : SFloat
		{
			var hasDiffuse				: Boolean	= getLightConstant(lightId, 'diffuseEnabled');
			var hasSpecular				: Boolean	= getLightConstant(lightId, 'specularEnabled');
			var shadowCasting			: uint		= getLightConstant(lightId, 'shadowCastingType');
			var meshReceiveShadows		: Boolean	= meshBindings.getConstant(PhongProperties.RECEIVE_SHADOWS, false);
			var computeShadows			: Boolean	= shadowCasting != ShadowMappingType.NONE && meshReceiveShadows;
			var normalMappingType		: uint		= meshBindings.getConstant(PhongProperties.NORMAL_MAPPING_TYPE, NormalMappingType.NONE);
			
			var contribution			: SFloat	= float(0);
			
			if (hasDiffuse)
			{
				if (normalMappingType != NormalMappingType.NONE)
					contribution.incrementBy(infinitePart.computeDiffuseInTangentSpace(lightId));
				else
					contribution.incrementBy(infinitePart.computeDiffuseInWorldSpace(lightId));
			}
			
			if (hasSpecular)
			{
				if (normalMappingType != NormalMappingType.NONE)
					contribution.incrementBy(infinitePart.computeSpecularInTangentSpace(lightId));
				else
					contribution.incrementBy(infinitePart.computeSpecularInWorldSpace(lightId));
			}
			
			if (computeShadows)
				contribution.scaleBy(shadowAttenuators[shadowCasting].getAttenuation(lightId));
			
			return contribution;
		}
		
		private function getPointLightContribution(lightId : uint) : SFloat
		{
			var hasDiffuse			: Boolean	= getLightConstant(lightId, 'diffuseEnabled');
			var hasSpecular			: Boolean	= getLightConstant(lightId, 'specularEnabled');
			var shadowCasting		: uint		= getLightConstant(lightId, 'shadowCastingType');
			var isAttenuated		: Boolean	= getLightConstant(lightId, 'attenuationEnabled');
			var normalMappingType	: uint		= meshBindings.getConstant(PhongProperties.NORMAL_MAPPING_TYPE, NormalMappingType.NONE);
			var meshReceiveShadows	: Boolean	= meshBindings.getConstant(PhongProperties.RECEIVE_SHADOWS, false);
			var computeShadows		: Boolean	= shadowCasting != ShadowMappingType.NONE && meshReceiveShadows;
			
			var contribution		: SFloat	= float(0);
			
			if (hasDiffuse)
			{
				if (normalMappingType != NormalMappingType.NONE)
					contribution.incrementBy(localizedPart.computeDiffuseInTangentSpace(lightId));
				else
					contribution.incrementBy(localizedPart.computeDiffuseInWorldSpace(lightId));
			}
			
			if (hasSpecular)
			{
				if (normalMappingType != NormalMappingType.NONE)
					contribution.incrementBy(localizedPart.computeSpecularInTangentSpace(lightId));
				else
					contribution.incrementBy(localizedPart.computeSpecularInWorldSpace(lightId));
			}
			
			if (isAttenuated)
				contribution.scaleBy(distanceAttenuationPart.getAttenuation(lightId));
			
			if (computeShadows)
				contribution.scaleBy(shadowAttenuators[shadowCasting].getAttenuation(lightId));
			
			return contribution;
		}
		
		private function getSpotLightContribution(lightId : uint) : SFloat
		{
			var hasDiffuse			: Boolean	= getLightConstant(lightId, 'diffuseEnabled');
			var hasSpecular			: Boolean	= getLightConstant(lightId, 'specularEnabled');
			var shadowCasting		: uint		= getLightConstant(lightId, 'shadowCastingType');
			var isAttenuated		: Boolean	= getLightConstant(lightId, 'attenuationEnabled');
			var lightHasSmoothEdge	: Boolean	= getLightConstant(lightId, 'smoothRadius');
			var meshReceiveShadows	: Boolean	= meshBindings.getConstant(PhongProperties.RECEIVE_SHADOWS, false);
			var computeShadows		: Boolean	= shadowCasting != ShadowMappingType.NONE && meshReceiveShadows;
			var normalMappingType	: uint		= meshBindings.getConstant(PhongProperties.NORMAL_MAPPING_TYPE, NormalMappingType.NONE);
			var contribution		: SFloat	= float(0);
			
			if (hasDiffuse)
			{
				if (normalMappingType != NormalMappingType.NONE)
					contribution.incrementBy(localizedPart.computeDiffuseInTangentSpace(lightId));
				else
					contribution.incrementBy(localizedPart.computeDiffuseInWorldSpace(lightId));
			}
			
			if (hasSpecular)
			{
				if (normalMappingType != NormalMappingType.NONE)
					contribution.incrementBy(localizedPart.computeSpecularInTangentSpace(lightId));
				else
					contribution.incrementBy(localizedPart.computeSpecularInWorldSpace(lightId));
			}
			
			if (isAttenuated)
				contribution.scaleBy(distanceAttenuationPart.getAttenuation(lightId));
			
			if (lightHasSmoothEdge)
				contribution.scaleBy(smoothConicAttenuationPart.getAttenuation(lightId));
			else
				contribution.scaleBy(hardConicAttenuationPart.getAttenuation(lightId));
			
			if (computeShadows)
				contribution.scaleBy(shadowAttenuators[shadowCasting].getAttenuation(lightId));
			
			return contribution;
		}
		
		public function applyPhongLighting(diffuse : SFloat) : SFloat
		{
			return float4(multiply(diffuse.rgb, getLightingColor()), diffuse.a);
		}
	}
}
