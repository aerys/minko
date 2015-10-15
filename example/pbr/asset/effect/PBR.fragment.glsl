#ifdef FRAGMENT_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "TextureLod.extension.glsl"
#pragma include "TextureLod.function.glsl"
#pragma include "ShadowMapping.function.glsl"
#pragma include "Math.function.glsl"
#pragma include "PBR.function.glsl"

// camera
uniform vec3 uCameraPosition;
uniform vec3 uCameraDirection;

// alpha
uniform sampler2D uAlphaMap;
uniform float uAlphaThreshold;

// fog
uniform vec4 uFogColor;
uniform vec2 uFogBounds;

// pbr material
uniform vec4 uAlbedoColor;
uniform sampler2D uAlbedoMap;
uniform vec3 uSpecularColor;
uniform sampler2D uSpecularMap;
uniform sampler2D uNormalMap;
uniform float uRoughness;
uniform float uMetalness;
uniform sampler2D uRadianceMap;
uniform sampler2D uIrradianceMap;

uniform float uGammaCorrection;

// texture lod
uniform float uAlbedoMapMaxAvailableLod;
uniform vec2 uAlbedoMapSize;

uniform float uNormalMapMaxAvailableLod;
uniform vec2 uNormalMapSize;

uniform float uSpecularMapMaxAvailableLod;
uniform vec2 uSpecularMapSize;

// directional lights
uniform vec3 uDirLight0_direction;
uniform vec3 uDirLight0_color;
uniform float uDirLight0_diffuse;
uniform float uDirLight0_specular;
uniform sampler2D uDirLight0_shadowMap;
uniform mat4 uDirLight0_viewProjection[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight0_zNear[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight0_zFar[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform vec4 uDirLight0_shadowSplitNear;
uniform vec4 uDirLight0_shadowSplitFar;
uniform float uDirLight0_shadowMapSize;
uniform float uDirLight0_shadowSpread;
uniform float uDirLight0_shadowBias;

uniform vec3 uDirLight1_direction;
uniform vec3 uDirLight1_color;
uniform float uDirLight1_diffuse;
uniform float uDirLight1_specular;
uniform sampler2D uDirLight1_shadowMap;
uniform mat4 uDirLight1_viewProjection[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight1_zNear[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight1_zFar[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform vec4 uDirLight1_shadowSplitNear;
uniform vec4 uDirLight1_shadowSplitFar;
uniform float uDirLight1_shadowMapSize;
uniform float uDirLight1_shadowSpread;
uniform float uDirLight1_shadowBias;

uniform vec3 uDirLight2_direction;
uniform vec3 uDirLight2_color;
uniform float uDirLight2_diffuse;
uniform float uDirLight2_specular;
uniform sampler2D uDirLight2_shadowMap;
uniform mat4 uDirLight2_viewProjection[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight2_zNear[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight2_zFar[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform vec4 uDirLight2_shadowSplitNear;
uniform vec4 uDirLight2_shadowSplitFar;
uniform float uDirLight2_shadowMapSize;
uniform float uDirLight2_shadowSpread;
uniform float uDirLight2_shadowBias;

uniform vec3 uDirLight3_direction;
uniform vec3 uDirLight3_color;
uniform float uDirLight3_diffuse;
uniform float uDirLight3_specular;
uniform sampler2D uDirLight3_shadowMap;
uniform mat4 uDirLight3_viewProjection[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight3_zNear[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight3_zFar[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform vec4 uDirLight3_shadowSplitNear;
uniform vec4 uDirLight3_shadowSplitFar;
uniform float uDirLight3_shadowMapSize;
uniform float uDirLight3_shadowSpread;
uniform float uDirLight3_shadowBias;

// ambient lights
uniform vec3 uAmbientLight0_color;
uniform float uAmbientLight0_ambient;

uniform vec3 uAmbientLight1_color;
uniform float uAmbientLight1_ambient;

uniform vec3 uAmbientLight2_color;
uniform float uAmbientLight2_ambient;

uniform vec3 uAmbientLight3_color;
uniform float uAmbientLight3_ambient;

// point lights
uniform vec3 uPointLight0_color;
uniform float uPointLight0_diffuse;
uniform float uPointLight0_specular;
uniform vec3 uPointLight0_attenuationCoeffs;
uniform vec3 uPointLight0_position;

uniform vec3 uPointLight1_color;
uniform float uPointLight1_diffuse;
uniform float uPointLight1_specular;
uniform vec3 uPointLight1_attenuationCoeffs;
uniform vec3 uPointLight1_position;

uniform vec3 uPointLight2_color;
uniform float uPointLight2_diffuse;
uniform float uPointLight2_specular;
uniform vec3 uPointLight2_attenuationCoeffs;
uniform vec3 uPointLight2_position;

uniform vec3 uPointLight3_color;
uniform float uPointLight3_diffuse;
uniform float uPointLight3_specular;
uniform vec3 uPointLight3_attenuationCoeffs;
uniform vec3 uPointLight3_position;

// spot lights
uniform vec3 uSpotLight0_direction;
uniform float uSpotLight0_diffuse;
uniform vec3 uSpotLight0_position;
uniform float uSpotLight0_cosInnerConeAngle;
uniform vec3 uSpotLight0_color;
uniform float uSpotLight0_cosOuterConeAngle;
uniform vec3 uSpotLight0_attenuationCoeffs;
uniform float uSpotLight0_specular;

uniform vec3 uSpotLight1_direction;
uniform float uSpotLight1_diffuse;
uniform vec3 uSpotLight1_position;
uniform float uSpotLight1_cosInnerConeAngle;
uniform vec3 uSpotLight1_color;
uniform float uSpotLight1_cosOuterConeAngle;
uniform vec3 uSpotLight1_attenuationCoeffs;
uniform float uSpotLight1_specular;

uniform vec3 uSpotLight2_direction;
uniform float uSpotLight2_diffuse;
uniform vec3 uSpotLight2_position;
uniform float uSpotLight2_cosInnerConeAngle;
uniform vec3 uSpotLight2_color;
uniform float uSpotLight2_cosOuterConeAngle;
uniform vec3 uSpotLight2_attenuationCoeffs;
uniform float uSpotLight2_specular;

uniform vec3 uSpotLight3_direction;
uniform float uSpotLight3_diffuse;
uniform vec3 uSpotLight3_position;
uniform float uSpotLight3_cosInnerConeAngle;
uniform vec3 uSpotLight3_color;
uniform float uSpotLight3_cosOuterConeAngle;
uniform vec3 uSpotLight3_attenuationCoeffs;
uniform float uSpotLight3_specular;

varying vec3 vVertexPosition;
varying vec2 vVertexUV;
varying vec3 vVertexNormal;
varying vec3 vVertexTangent;
varying vec4 vVertexScreenPosition;
varying vec4 vVertexColor;

float getShadow(sampler2D 	shadowMap,
				mat4 		viewProj[SHADOW_MAPPING_MAX_NUM_CASCADES],
				float 		zNear[SHADOW_MAPPING_MAX_NUM_CASCADES],
				float 		zFar[SHADOW_MAPPING_MAX_NUM_CASCADES],
				vec4		splitNear,
                vec4        splitFar,
				float 		size,
				float 		bias)
{
	float shadow = 1.0;
    vec4 weights = shadowMapping_getCascadeWeights(vVertexScreenPosition.z, splitNear, splitFar);
	vec4 viewport = shadowMapping_getCascadeViewport(weights);
    mat4 viewProjection = shadowMapping_getCascadeViewProjection(weights, viewProj);
    float near = shadowMapping_getCascadeZ(weights, zNear);
    float far = shadowMapping_getCascadeZ(weights, zFar);
	vec3 vertexLightPosition = (viewProjection * vec4(vVertexPosition, 1)).xyz;

	if (shadowMapping_vertexIsInShadowMap(vertexLightPosition))
	{
		float shadowDepth = vertexLightPosition.z;// + bias;
		vec2 depthUV = vertexLightPosition.xy / 2.0 + 0.5;

		depthUV = vec2(depthUV.xy * viewport.zw + viewport.xy);

		#if SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_HARD
			shadow = shadowMapping_texture2DCompare(shadowMap, depthUV, shadowDepth, near, far);
		#elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_PCF
			shadow = shadowMapping_PCF(shadowMap, vec2(size), depthUV, shadowDepth, near, far);
		#elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_ESM
			shadow = shadowMapping_ESM(shadowMap, depthUV, shadowDepth, near, far, (far - near) * 1.5);
        #elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_PCF_POISSON
			shadow = shadowMapping_PCFPoisson(shadowMap, vec2(size), depthUV, shadowDepth, near, far, vec4(vVertexPosition.xy, vVertexNormal.xy));
		#endif
	}

	return shadow;
}

void main(void)
{
    float roughness = max(1e-4, uRoughness);
	vec3 ambientAccum = vec3(0.0);
	vec3 diffuseAccum = vec3(0.0);
	vec3 specularAccum	= vec3(0.0);
	vec3 eyeVector = normalize(uCameraPosition - vVertexPosition); // always in world-space
	vec3 normalVector = normalize(vVertexNormal); // always in world-space

    // albedo color
	vec4 albedo = uAlbedoColor;
	#ifdef VERTEX_COLOR
		albedo = vVertexColor;
	#endif // VERTEX_COLOR

	#if defined(ALBEDO_MAP) && defined(VERTEX_UV)
		#ifdef ALBEDO_MAP_LOD
			albedo = texturelod_texture2D(uAlbedoMap, vVertexUV, uAlbedoMapSize, 0.0, uAlbedoMapMaxAvailableLod, albedo);
		#else
			albedo = texture2D(uAlbedoMap, vVertexUV);
		#endif
	#endif // ALBEDO_MAP

	#if defined(ALPHA_MAP) && defined(VERTEX_UV)
		albedo.a = texture2D(uAlphaMap, vVertexUV).r;
	#endif // ALPHA_MAP

	#ifdef ALPHA_THRESHOLD
		if (albedo.a < uAlphaThreshold)
			discard;
	#endif // ALPHA_THRESHOLD

    // specular color
	vec3 specular = uSpecularColor;
	#if defined(SPECULAR_MAP) && defined(VERTEX_UV)
		#ifdef SPECULAR_MAP_LOD
			specular = texturelod_texture2D(uSpecularMap, vVertexUV, uSpecularMapSize, 0.0, uSpecularMapMaxAvailableLod, uSpecularColor).rgb;
		#else
			specular = texture2D(uSpecularMap, vVertexUV).rgb;
		#endif
	#endif // SPECULAR_MAP

    specular = mix(specular, albedo.rgb, uMetalness);
    albedo.rgb = mix(albedo.rgb, vec3(0.0), uMetalness);

    vec3 N = normalVector;
    vec3 V = eyeVector;
    float NoV = saturate(dot(N, V));

    // image based lighting
    #ifdef IRRADIANCE_MAP
        diffuseAccum += pbr_envDiffuse(uIrradianceMap, normalVector);
    #endif // IRRADIANCE_CUBEMAP

    #ifdef RADIANCE_MAP
        specularAccum = pbr_envSpecular(
            specular,
            uRadianceMap,
            10,
            roughness,
            reflect(-eyeVector, normalVector),
            NoV,
            normalVector,
            eyeVector
        );
    #endif // RADIANCE_CUBEMAP

    // ambient lights
    #ifdef NUM_AMBIENT_LIGHTS
    	#if NUM_AMBIENT_LIGHTS > 0
            ambientAccum += uAmbientLight0_color * uAmbientLight0_ambient;
    	#endif // NUM_AMBIENT_LIGHTS > 0
    	#if NUM_AMBIENT_LIGHTS > 1
            ambientAccum += uAmbientLight1_color * uAmbientLight1_ambient;
    	#endif // NUM_AMBIENT_LIGHTS > 1
        #if NUM_AMBIENT_LIGHTS > 2
            ambientAccum += uAmbientLight2_color * uAmbientLight2_ambient;
    	#endif // NUM_AMBIENT_LIGHTS > 2
        #if NUM_AMBIENT_LIGHTS > 3
            ambientAccum += uAmbientLight3_color * uAmbientLight3_ambient;
    	#endif // NUM_AMBIENT_LIGHTS > 3
    #endif

	#if defined(NUM_DIRECTIONAL_LIGHTS) || defined(NUM_POINT_LIGHTS) || defined(NUM_SPOT_LIGHTS) || defined(IRRADIANCE_CUBEMAP) || defined(RADIANCE_CUBEMAP)
		#if defined(NORMAL_MAP) && defined(VERTEX_UV)
			// warning: the normal vector must be normalized at this point!
			mat3 tangentToWorldMatrix = phong_getTangentToWorldSpaceMatrix(normalVector, vVertexTangent);

			// bring normal from tangent-space to world-space
			#ifdef NORMAL_MAP_LOD
				vec4 normalColor = texturelod_texture2D(uNormalMap, vVertexUV, uNormalMapSize, 0.0, uNormalMapMaxAvailableLod, vec4(0.0));
			#else
				vec4 normalColor = texture2D(uNormalMap, vVertexUV);
			#endif

			normalVector = tangentToWorldMatrix * normalize(2.0 * normalColor.xyz - 1.0);
		#endif // NORMAL_MAP

    	vec3 L;
        vec3 H;
    	float shadow;
        float NoL, NoH, VoH;
        #ifdef NUM_DIRECTIONAL_LIGHTS

    		#if NUM_DIRECTIONAL_LIGHTS > 0
    			shadow = 1.0;
    			L = normalize(-uDirLight0_direction);
                H = normalize(V + L);
                NoL = saturate(dot(N, L));
                NoH = saturate(dot(N, H));
                VoH = saturate(dot(L, H));
    			#ifdef DIRECTIONAL_0_SHADOW_MAP
    				shadow = getShadow(uDirLight0_shadowMap, uDirLight0_viewProjection, uDirLight0_zNear, uDirLight0_zFar, uDirLight0_shadowSplitNear, uDirLight0_shadowSplitFar, uDirLight0_shadowMapSize, uDirLight0_shadowBias);
    			#endif
                diffuseAccum += pbr_diffuse(uDirLight0_color * uDirLight0_diffuse, uRoughness, NoV, NoL, VoH) * shadow;
                specularAccum += pbr_specular(specular.rgb, roughness, NoL, NoV, NoH, VoH) * shadow;
    		#endif // NUM_DIRECTIONAL_LIGHTS > 0

    		#if NUM_DIRECTIONAL_LIGHTS > 1
    			shadow = 1.0;
                L = normalize(-uDirLight1_direction);
                H = normalize(V + L);
                NoL = saturate(dot(N, L));
                NoH = saturate(dot(N, H));
                VoH = saturate(dot(L, H));
    			#ifdef DIRECTIONAL_1_SHADOW_MAP
    				shadow = getShadow(uDirLight1_shadowMap, uDirLight1_viewProjection, uDirLight1_zNear, uDirLight1_zFar, uDirLight1_shadowSplitNear, uDirLight1_shadowSplitFar, uDirLight1_shadowMapSize, uDirLight1_shadowBias);
    			#endif
                diffuseAccum += pbr_diffuse(uDirLight1_color * uDirLight1_diffuse, uRoughness, NoV, NoL, VoH) * shadow;
                specularAccum += pbr_specular(specular.rgb, roughness, NoL, NoV, NoH, VoH) * shadow;
    		#endif // NUM_DIRECTIONAL_LIGHTS > 1

    		#if NUM_DIRECTIONAL_LIGHTS > 2
    			shadow = 1.0;
                L = normalize(-uDirLight2_direction);
                H = normalize(V + L);
                NoL = saturate(dot(N, L));
                NoH = saturate(dot(N, H));
                VoH = saturate(dot(L, H));
    			#ifdef DIRECTIONAL_2_SHADOW_MAP
    				shadow = getShadow(uDirLight2_shadowMap, uDirLight2_viewProjection, uDirLight2_zNear, uDirLight2_zFar, uDirLight2_shadowSplitNear, uDirLight2_shadowSplitFar, uDirLight2_shadowMapSize, uDirLight2_shadowBias);
    			#endif
    			diffuseAccum += pbr_diffuse(uDirLight2_color * uDirLight2_diffuse, uRoughness, NoV, NoL, VoH) * shadow;
				specularAccum += pbr_specular(specular.rgb, roughness, NoL, NoV, NoH, VoH) * shadow;
    		#endif // NUM_DIRECTIONAL_LIGHTS > 2

    		#if NUM_DIRECTIONAL_LIGHTS > 3
    			shadow = 1.0;
                L = normalize(-uDirLight3_direction);
                H = normalize(V + L);
                NoL = saturate(dot(N, L));
                NoH = saturate(dot(N, H));
                VoH = saturate(dot(L, H));
    			#ifdef DIRECTIONAL_3_SHADOW_MAP
    				shadow = getShadow(uDirLight3_shadowMap, uDirLight3_viewProjection, uDirLight3_zNear, uDirLight3_zFar, uDirLight3_shadowSplitNear, uDirLight3_shadowSplitFar, uDirLight3_shadowMapSize, uDirLight3_shadowBias);
    			#endif
    			diffuseAccum += pbr_diffuse(uDirLight3_color * uDirLight3_diffuse, uRoughness, NoV, NoL, VoH) * shadow;
				specularAccum += pbr_specular(specular.rgb, roughness, NoL, NoV, NoH, VoH) * shadow;
    		#endif // NUM_DIRECTIONAL_LIGHTS > 3

        #endif // defined(NUM_DIRECTIONAL_LIGHTS)

	#endif // defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS || defined IRRADIANCE_CUBEMAP || defined RADIANCE_CUBEMAP

    vec3 specularLighting = specularAccum;// * specular;
    vec3 diffuseLighting = diffuseAccum * albedo.rgb * (1.0 - specularLighting);
    vec3 color = specularLighting + diffuseLighting;

	#ifdef FOG_TECHNIQUE
		color = fog_sampleFog(color, vVertexScreenPosition.z, uFogColor.xyz, uFogColor.a, uFogBounds.x, uFogBounds.y);
	#endif

    #ifdef GAMMA_CORRECTION
        color = pow(color, vec3(1.0 / uGammaCorrection));
    #endif

	gl_FragColor = vec4(color, albedo.a);
}

#endif // FRAGMENT_SHADER
