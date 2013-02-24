package aerys.minko.render.shader.part.phong
{
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.material.phong.PhongProperties;
	import aerys.minko.render.shader.SFloat;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.part.phong.attenuation.DPShadowMapAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.DistanceAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.ExponentialShadowMapAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.HardConicAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.IAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.PCFShadowMapAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.PolynomialAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.SmoothConicAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.attenuation.VarianceShadowMapAttenuationShaderPart;
	import aerys.minko.render.shader.part.phong.contribution.InfiniteShaderPart;
	import aerys.minko.render.shader.part.phong.contribution.LocalizedShaderPart;
	import aerys.minko.scene.node.light.AmbientLight;
	import aerys.minko.scene.node.light.DirectionalLight;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.scene.node.light.SpotLight;
	import aerys.minko.type.enum.NormalMappingType;
	import aerys.minko.type.enum.ShadowMappingType;
	
	/**
	 * This shader part compute the lighting contribution of all lights
	 * 
	 * @author Romain Gilliotte
	 */	
	public class PhongShaderPart extends LightAwareShaderPart
	{
		private var _shadowAttenuators				: Vector.<IAttenuationShaderPart>;
		private var _infinitePart					: InfiniteShaderPart;
		private var _localizedPart					: LocalizedShaderPart;
		private var _distanceAttenuationPart		: DistanceAttenuationShaderPart;
		private var _polynomialAttenuationPart		: PolynomialAttenuationShaderPart;
		private var _smoothConicAttenuationPart		: SmoothConicAttenuationShaderPart;
		private var _hardConicAttenuationPart		: HardConicAttenuationShaderPart;
		private var _matrixShadowMapAttenuation		: PCFShadowMapAttenuationShaderPart;
		private var _dpShadowMapAttenuation			: DPShadowMapAttenuationShaderPart;
		private var _varianceShadowMapAttenuation	: VarianceShadowMapAttenuationShaderPart;
		private var _exponentialShadowMapAttenuation: ExponentialShadowMapAttenuationShaderPart;
		
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
		
		private function get polynomialAttenuationPart() : PolynomialAttenuationShaderPart
		{
			_polynomialAttenuationPart ||= new PolynomialAttenuationShaderPart(main);
			return _polynomialAttenuationPart;
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
		
		private function get matrixShadowMapAttenuation() : PCFShadowMapAttenuationShaderPart
		{
			return _matrixShadowMapAttenuation
				|| (_matrixShadowMapAttenuation = new PCFShadowMapAttenuationShaderPart(main));
		}
		
		private function get dpShadowMapAttenuation() : DPShadowMapAttenuationShaderPart
		{
			return _dpShadowMapAttenuation
				|| (_dpShadowMapAttenuation = new DPShadowMapAttenuationShaderPart(main));
		}
		
		private function get varianceShadowMapAttenuation() : VarianceShadowMapAttenuationShaderPart
		{
			return _varianceShadowMapAttenuation
				|| (_varianceShadowMapAttenuation = new VarianceShadowMapAttenuationShaderPart(main));
		}
		
		private function get exponentialShadowMapAttenuation() : ExponentialShadowMapAttenuationShaderPart
		{
			return _exponentialShadowMapAttenuation
				|| (_exponentialShadowMapAttenuation = new ExponentialShadowMapAttenuationShaderPart(main));
		}
		
		public function PhongShaderPart(main : Shader)
		{
			super(main);
		}
		
		public function getStaticLighting() : SFloat
		{
			var contribution : SFloat;
			
			if (meshBindings.propertyExists(PhongProperties.LIGHT_MAP))
			{
				var uv			: SFloat = getVertexAttribute(VertexComponent.UV);
				var lightMap	: SFloat = meshBindings.getTextureParameter(
					PhongProperties.LIGHT_MAP
				);
				
				contribution = sampleTexture(lightMap, interpolate(uv));
				contribution = contribution.xyz;
				
				if (meshBindings.propertyExists(PhongProperties.LIGHTMAP_MULTIPLIER))
					contribution.scaleBy(meshBindings.getParameter(
						PhongProperties.LIGHTMAP_MULTIPLIER,
						1
					));
			}
			else
				contribution = float3(0, 0, 0);
			
			return contribution;
		}
		
//		public function getDynamicLighting(skipAmbient : Boolean   = false,
//                                           lightId     : int       = -1) : SFloat
//		{
//			var dynamicLighting : SFloat	= float3(0, 0, 0);
//            var multiPassMode   : Boolean   = lightId != -1;
//			var receptionMask	: uint		= meshBindings.getProperty(
//				PhongProperties.RECEPTION_MASK,
//				1
//			);
//            
//            if (!multiPassMode)
//                lightId = 0;
//			
//			while (lightPropertyExists(lightId, 'emissionMask'))
//			{
//				var emissionMask : uint = getLightProperty(lightId, 'emissionMask');
//				
//				if ((emissionMask & receptionMask) != 0)
//				{
//					var type    : uint  = getLightProperty(lightId, 'type');
//                    
//                    if ((type != AmbientLight.LIGHT_TYPE || !skipAmbient)
//                        && getLightProperty(lightId, 'enabled'))
//                    {
//    					var color           : SFloat	= getLightParameter(lightId, 'color', 4);
//                        var contribution    : SFloat    = null;
//                        
//                        if (type == AmbientLight.LIGHT_TYPE)
//                            contribution = getAmbientLightContribution(lightId);
//                        else if (type == DirectionalLight.LIGHT_TYPE)
//                            contribution = getDirectionalLightContribution(lightId);
//                        else if (type == PointLight.LIGHT_TYPE)
//                            contribution = getPointLightContribution(lightId);
//                        else if (type == SpotLight.LIGHT_TYPE)
//                            contribution = getSpotLightContribution(lightId);
//                        
//                        if (contribution)
//                            dynamicLighting.incrementBy(multiply(color.rgb, contribution));
//                    }
//				}
//                
//                if (multiPassMode)
//                    break;
//                
//                ++lightId;
//			}
//			
//			return dynamicLighting;
//		}
		
        public function getAmbientLighting() : SFloat
        {
            var ambientLighting : SFloat    = float3(0., 0., 0.);
            var lightId         : uint      = 0;
            var receptionMask	: uint		= meshBindings.getProperty(
                PhongProperties.RECEPTION_MASK,
                1
            );
            
            while (lightPropertyExists(lightId, 'emissionMask')
                && getLightProperty(lightId, 'type') == AmbientLight.LIGHT_TYPE)
            {
                var emissionMask : uint = getLightProperty(lightId, 'emissionMask');
                
                if ((emissionMask & receptionMask) != 0)
                {
                    var isEnabled : Boolean = lightPropertyExists(lightId, 'enabled')
                        && getLightProperty(lightId, 'enabled');
                    
                    if (!isEnabled)
                        continue;
                    
                    var color			: SFloat	= getLightParameter(lightId, 'color', 4);
                    var contribution	: SFloat	= getAmbientLightContribution(lightId);
                    
                    ambientLighting.incrementBy(multiply(color.rgb, contribution));
                }
                
                ++lightId;
            }
            
            ambientLighting.incrementBy(multiply(lessEqual(ambientLighting, 0), .5));
            
            return ambientLighting;
        }
        
        public function getAmbientLightContribution(lightId : uint) : SFloat
		{
			var ambient : SFloat = getLightParameter(lightId, 'ambient', 1);
			
			if (meshBindings.propertyExists(PhongProperties.AMBIENT_MULTIPLIER))
				ambient.scaleBy(sceneBindings.getParameter(PhongProperties.AMBIENT_MULTIPLIER, 1));
			
			return ambient;
		}
		
		public function getDirectionalLightContribution(lightId : uint) : SFloat
		{
			var shadowCasting		: uint		= getLightProperty(lightId, 'shadowCastingType');
			var meshReceiveShadows	: Boolean	= meshBindings.getProperty(
				PhongProperties.RECEIVE_SHADOWS,
				false
			);
			var computeShadows		: Boolean	= shadowCasting != ShadowMappingType.NONE
				&& meshReceiveShadows;
            var contribution		: SFloat	= null;
            var diffuseLighting     : SFloat    = getDirectionalLightDiffuse(lightId);
            var specularLighting    : SFloat    = getDirectionalLightSpecular(lightId);
            
            if (diffuseLighting)
                contribution = diffuseLighting;
            if (specularLighting)
                contribution = contribution ? add(contribution, specularLighting) : specularLighting;
			
            // attenuation
			if (contribution && computeShadows)
            {
                if (shadowCasting == ShadowMappingType.PCF)
                    contribution.scaleBy(matrixShadowMapAttenuation.getAttenuation(lightId));
                else if (shadowCasting == ShadowMappingType.DUAL_PARABOLOID)
                    contribution.scaleBy(dpShadowMapAttenuation.getAttenuation(lightId));
                else if (shadowCasting == ShadowMappingType.VARIANCE)
                    contribution.scaleBy(varianceShadowMapAttenuation.getAttenuation(lightId));
                else if (shadowCasting == ShadowMappingType.EXPONENTIAL)
                    contribution.scaleBy(exponentialShadowMapAttenuation.getAttenuation(lightId));
            }
            
			return contribution;
		}
        
        public function getSpotLightContribution(lightId : uint) : SFloat
        {
            var shadowCasting		: uint		= getLightProperty(lightId, 'shadowCastingType');
            var isAttenuated		: Boolean	= getLightProperty(lightId, 'attenuationEnabled');
            var lightHasSmoothEdge	: Boolean	= getLightProperty(lightId, 'smoothRadius');
            var meshReceiveShadows	: Boolean	= meshBindings.getProperty(
                PhongProperties.RECEIVE_SHADOWS,
                false
            );
            var computeShadows		: Boolean	= shadowCasting != ShadowMappingType.NONE
                && meshReceiveShadows;
            var contribution		: SFloat	= null;
            var diffuseLighting     : SFloat    = getSpotLightDiffuse(lightId);
            var specularLighting    : SFloat    = getSpotLightSpecular(lightId);
            
            if (diffuseLighting)
                contribution = diffuseLighting;
            if (specularLighting)
                contribution = contribution ? add(contribution, specularLighting) : specularLighting;

            // attenuation
            if (contribution)
            {
                if (isAttenuated)
                {
                    if (lightPropertyExists(lightId, 'attenuationPolynomial'))
                        contribution.scaleBy(polynomialAttenuationPart.getAttenuation(lightId));
                    else
                        contribution.scaleBy(distanceAttenuationPart.getAttenuation(lightId));
                }
                
                if (lightHasSmoothEdge)
                    contribution.scaleBy(smoothConicAttenuationPart.getAttenuation(lightId));
                else
                    contribution.scaleBy(hardConicAttenuationPart.getAttenuation(lightId));
                
                if (computeShadows)
                    switch (shadowCasting)
                    {
                        case ShadowMappingType.PCF :
                            contribution.scaleBy(matrixShadowMapAttenuation.getAttenuation(lightId));
                            break ;
                        case ShadowMappingType.DUAL_PARABOLOID :
                            contribution.scaleBy(dpShadowMapAttenuation.getAttenuation(lightId));
                            break ;
                        case ShadowMappingType.VARIANCE :
                            contribution.scaleBy(varianceShadowMapAttenuation.getAttenuation(lightId));
                            break ;
                        case ShadowMappingType.EXPONENTIAL :
                            contribution.scaleBy(exponentialShadowMapAttenuation.getAttenuation(lightId));
                            break ;
                    }
            }
            
            return contribution;
        }
		
        public function getPointLightContribution(lightId : uint) : SFloat
		{
			var shadowCasting		: uint		= getLightProperty(lightId, 'shadowCastingType');
			var isAttenuated		: Boolean	= getLightProperty(lightId, 'attenuationEnabled');
			var meshReceiveShadows	: Boolean	= meshBindings.getProperty(
				PhongProperties.RECEIVE_SHADOWS,
				false
			);
			var computeShadows		: Boolean	= shadowCasting != ShadowMappingType.NONE
				&& meshReceiveShadows;
			
			var contribution		: SFloat	= null;
            var diffuseLighting     : SFloat    = getPointLightDiffuse(lightId);
            var specularLighting    : SFloat    = getPointLightSpecular(lightId);
            
            if (diffuseLighting)
                contribution = diffuseLighting;
            if (specularLighting)
                contribution = contribution ? add(contribution, specularLighting) : specularLighting;
			
            // attenuation
            if (contribution)
            {
    			if (isAttenuated)
    			{
    				if (lightPropertyExists(lightId, 'attenuationPolynomial'))
    					contribution.scaleBy(polynomialAttenuationPart.getAttenuation(lightId));
    				else
    					contribution.scaleBy(distanceAttenuationPart.getAttenuation(lightId));
    			}
    			
    			if (computeShadows)
    			{
    				switch (shadowCasting)
    				{
    					case ShadowMappingType.PCF :
    						contribution.scaleBy(matrixShadowMapAttenuation.getAttenuation(lightId));
    						break ;
    					case ShadowMappingType.DUAL_PARABOLOID :
    						contribution.scaleBy(dpShadowMapAttenuation.getAttenuation(lightId));
    						break ;
    					case ShadowMappingType.VARIANCE :
    						contribution.scaleBy(varianceShadowMapAttenuation.getAttenuation(lightId));
    						break ;
    					case ShadowMappingType.EXPONENTIAL :
    						contribution.scaleBy(exponentialShadowMapAttenuation.getAttenuation(lightId));
    						break ;
    				}
    			}
            }
			
			return contribution;
		}
        
//        private function getDiffuse(receptionMask : uint = 1) : SFloat
//        {
//            var diffuse     : SFloat    = null;
//            var lightId     : uint      = 0;
//            
//            while (lightPropertyExists(lightId, 'emissionMask'))
//            {
//                var emissionMask : uint = getLightProperty(lightId, 'emissionMask');
//                
//                if ((emissionMask & receptionMask) != 0)
//                {
//                    var isEnabled : Boolean = lightPropertyExists(lightId, 'enabled')
//                        && getLightProperty(lightId, 'enabled');
//                    
//                    if (!isEnabled)
//                        continue;
//                    
//                    var color	        : SFloat	= getLightParameter(lightId, 'color', 4);
//                    var type            : uint  	= getLightProperty(lightId, 'type');
//                    var contribution    : SFloat    = null;
//                    
//                    if (type == DirectionalLight.LIGHT_TYPE)
//                        contribution = getDirectionalLightDiffuse(lightId);
//                    else if (type == SpotLight.LIGHT_TYPE)
//                        contribution = getSpotLightDiffuse(lightId);
//                    else if (type == PointLight.LIGHT_TYPE)
//                        contribution = getPointLightDiffuse(lightId);
//                    
//                    if (contribution)
//                        diffuse = diffuse ? add(diffuse, contribution) : contribution;
//                }
//                
//                ++lightId;
//            }
//            
//            return diffuse;
//        }
        
        private function getDirectionalLightDiffuse(lightId : int) : SFloat
        {
            if (getLightProperty(lightId, 'diffuseEnabled'))
            {
                var normalMappingType	: uint		= meshBindings.getProperty(
                    PhongProperties.NORMAL_MAPPING_TYPE,
                    NormalMappingType.NONE
                );
                
                return normalMappingType != NormalMappingType.NONE
                    ? infinitePart.computeDiffuseInTangentSpace(lightId)
                    : infinitePart.computeDiffuseInWorldSpace(lightId);
            }
            
            return null;
        }
        
        private function getSpotLightDiffuse(lightId : int) : SFloat
        {
            if (getLightProperty(lightId, 'diffuseEnabled'))
            {
                var normalMappingType   : uint  = meshBindings.getProperty(
                    PhongProperties.NORMAL_MAPPING_TYPE,
                    NormalMappingType.NONE
                );
                
                return normalMappingType != NormalMappingType.NONE
                    ? localizedPart.computeDiffuseInTangentSpace(lightId)
                    : localizedPart.computeDiffuseInWorldSpace(lightId);
            }
            
            return null;
        }
        
        private function getPointLightDiffuse(lightId : int) : SFloat
        {
            if (getLightProperty(lightId, 'diffuseEnabled'))
            {
                var normalMappingType   : uint  = meshBindings.getProperty(
                    PhongProperties.NORMAL_MAPPING_TYPE,
                    NormalMappingType.NONE
                );
                
                return normalMappingType != NormalMappingType.NONE
                    ? localizedPart.computeDiffuseInTangentSpace(lightId)
                    : localizedPart.computeDiffuseInWorldSpace(lightId);
            }
            
            return null;
        }
        
        private function getSpecular(receptionMask : uint = 1) : SFloat
        {
            var specular        : SFloat    = null;
            var lightId         : uint      = 0;
            
            while (lightPropertyExists(lightId, 'emissionMask'))
            {
                var emissionMask : uint = getLightProperty(lightId, 'emissionMask');
                
                if ((emissionMask & receptionMask) != 0)
                {
                    var isEnabled : Boolean = lightPropertyExists(lightId, 'enabled')
                        && getLightProperty(lightId, 'enabled');
                    
                    if (!isEnabled)
                        continue;
                    
                    var color	        : SFloat	= getLightParameter(lightId, 'color', 4);
                    var type            : uint  	= getLightProperty(lightId, 'type');
                    var contribution    : SFloat    = null;
                    
                    if (type == DirectionalLight.LIGHT_TYPE)
                        contribution = getDirectionalLightSpecular(lightId);
                    else if (type == SpotLight.LIGHT_TYPE)
                        contribution = getSpotLightSpecular(lightId);
                    else if (type == PointLight.LIGHT_TYPE)
                        contribution = getPointLightContribution(lightId);
                    
                    if (contribution)
                        specular = specular ? add(specular, contribution) : contribution;
                }
                
                ++lightId;
            }
            
            return specular;
        }

        private function getDirectionalLightSpecular(lightId : int) : SFloat
        {
            if (getLightProperty(lightId, 'specularEnabled'))
            {
                var normalMappingType	: uint		= meshBindings.getProperty(
                    PhongProperties.NORMAL_MAPPING_TYPE,
                    NormalMappingType.NONE
                );
                
                if (normalMappingType != NormalMappingType.NONE)
                    return infinitePart.computeSpecularInTangentSpace(lightId);
                else
                    return infinitePart.computeSpecularInWorldSpace(lightId);
            }
            
            return null;
        }
        
        private function getSpotLightSpecular(lightId : int) : SFloat
        {
            if (getLightProperty(lightId, 'specularEnabled'))
            {
                var normalMappingType	: uint		= meshBindings.getProperty(
                    PhongProperties.NORMAL_MAPPING_TYPE,
                    NormalMappingType.NONE
                );
                
                if (normalMappingType != NormalMappingType.NONE)
                    return localizedPart.computeSpecularInTangentSpace(lightId);
                else
                    return localizedPart.computeSpecularInWorldSpace(lightId);
            }
            
            return null;
        }
        
        private function getPointLightSpecular(lightId : int) : SFloat
        {
            if (getLightProperty(lightId, 'specularEnabled'))
            {
                var normalMappingType	: uint		= meshBindings.getProperty(
                    PhongProperties.NORMAL_MAPPING_TYPE,
                    NormalMappingType.NONE
                );
                
                if (normalMappingType != NormalMappingType.NONE)
                    return localizedPart.computeSpecularInTangentSpace(lightId);
                else
                    return localizedPart.computeSpecularInWorldSpace(lightId);
            }
            
            return null;
        }
        
        public function getBaseLighting() : SFloat
        {
            var lighting : SFloat = add(getAmbientLighting(), getStaticLighting());
            
            for (var lightId : uint = 0; lightPropertyExists(lightId, 'type'); ++lightId)
                if (getLightProperty(lightId, 'type') == DirectionalLight.LIGHT_TYPE
                    && getLightProperty(lightId, 'enabled'))
                {
                    lighting.incrementBy(getDirectionalLightContribution(lightId))
                    break;
                }
            
            return lighting;
        }
        
        public function getAdditionalLighting(lightId   : int   = -1) : SFloat
        {
            var additionalLighting  : SFloat	= float3(0, 0, 0);
            var discardDirectional  : Boolean   = true;
            var singleLight         : Boolean   = lightId != -1;
            var receptionMask	    : uint		= meshBindings.getProperty(
                PhongProperties.RECEPTION_MASK,
                1
            );
            
            if (!singleLight)
                lightId = 0;
            
            while (lightPropertyExists(lightId, 'emissionMask'))
            {
                var emissionMask : uint = getLightProperty(lightId, 'emissionMask');
                
                if ((emissionMask & receptionMask) != 0)
                {
                    var type    : uint  = getLightProperty(lightId, 'type');
                    
                    // if not in single light mode, discard first directional mode because it's
                    // already in the base lighting
                    if (type == DirectionalLight.LIGHT_TYPE && discardDirectional && !singleLight)
                    {
                        discardDirectional = false;
                        ++lightId;
                        continue;
                    }
                    
                    if (type != AmbientLight.LIGHT_TYPE && getLightProperty(lightId, 'enabled'))
                    {
                        var color           : SFloat	= getLightParameter(lightId, 'color', 4);
                        var contribution    : SFloat    = null;
                        
                        if (type == AmbientLight.LIGHT_TYPE)
                            contribution = getAmbientLightContribution(lightId);
                        else if (type == DirectionalLight.LIGHT_TYPE)
                            contribution = getDirectionalLightContribution(lightId);
                        else if (type == PointLight.LIGHT_TYPE)
                            contribution = getPointLightContribution(lightId);
                        else if (type == SpotLight.LIGHT_TYPE)
                            contribution = getSpotLightContribution(lightId);
                        
                        if (contribution)
                            additionalLighting.incrementBy(multiply(color.rgb, contribution));
                    }
                }
                
                if (singleLight)
                    break;
                
                ++lightId;
            }
            
            return additionalLighting;
        }
	}
}
