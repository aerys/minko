#ifdef FRAGMENT_SHADER

#pragma include "TextureLod.extension.glsl"

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "Envmap.function.glsl"
#pragma include "Phong.function.glsl"
#pragma include "TextureLod.function.glsl"
#pragma include "ShadowMapping.function.glsl"
#pragma include "Fog.function.glsl"
#pragma include "LightMapping.function.glsl"
#pragma include "PBR.function.glsl"
#pragma include "ToneMapping.function.glsl"

#ifdef GAMMA_CORRECTION
uniform float uGammaCorrection;
#endif

#ifdef UV_SCALE
uniform vec2 uUVScale;
#endif
#ifdef UV_OFFSET
uniform vec2 uUVOffset;
#endif

// diffuse
uniform vec4 uDiffuseColor;

#ifdef DIFFUSE_MAP
uniform sampler2D uDiffuseMap;
#endif

#ifdef LIGHT_MAP
uniform sampler2D uLightMap;
#endif

// alpha
#ifdef ALPHA_MAP
uniform sampler2D uAlphaMap;
#endif
#ifdef ALPHA_THRESHOLD
uniform float uAlphaThreshold;
#endif

// fog
#ifdef FOG_TECHNIQUE
uniform vec4 uFogColor;
uniform vec2 uFogBounds;
#endif

#ifdef NORMAL_MAP
uniform sampler2D uNormalMap;
#endif

// specular
uniform vec4 uSpecularColor;
#ifdef SPECULAR_MAP
uniform sampler2D uSpecularMap;
#endif

#ifdef SHININESS
uniform float uShininess;
#endif

uniform vec3 uCameraPosition;

// env. mapping
#ifdef ENVIRONMENT_MAP_2D
uniform sampler2D uEnvironmentMap2d;
uniform float uEnvironmentAlpha;
#endif
#ifdef ENVIRONMENT_CUBE_MAP
uniform samplerCube uEnvironmentCubemap;
uniform float uEnvironmentAlpha;
#endif

// fresnel
#ifdef FRESNEL
uniform float uFresnelReflectance;
uniform float uFresnelExponent;
#endif

// texture lod
#ifdef DIFFUSE_MAP_LOD
uniform float uDiffuseMapMaxAvailableLod;
uniform vec2 uDiffuseMapSize;
#endif

#ifdef LIGHT_MAP_LOD
uniform float uLightMapMaxAvailableLod;
uniform vec2 uLightMapSize;
#endif

#ifdef NORMAL_MAP_LOD
uniform float uNormalMapMaxAvailableLod;
uniform vec2 uNormalMapSize;
#endif

#ifdef SPECULAR_MAP_LOD
uniform float uSpecularMapMaxAvailableLod;
uniform vec2 uSpecularMapSize;
#endif

#ifdef ALPHA_MAP_LOD
uniform float uAlphaMapMaxAvailableLod;
uniform vec2 uAlphaMapSize;
#endif

#ifdef NUM_IMAGE_BASED_LIGHTS
# if NUM_IMAGE_BASED_LIGHTS > 0
uniform float uImageBasedLight0_diffuse;
uniform float uImageBasedLight0_specular;
uniform sampler2D uImageBasedLight0_radianceMap;
uniform sampler2D uImageBasedLight0_irradianceMap;
uniform float uImageBasedLight0_brightness;
uniform float uImageBasedLight0_orientation;
# endif
#endif

// directional lights
#ifdef NUM_DIRECTIONAL_LIGHTS

# if NUM_DIRECTIONAL_LIGHTS > 0
uniform vec3 uDirLight0_direction;
uniform vec3 uDirLight0_color;
uniform float uDirLight0_diffuse;
#  ifdef SHININESS
uniform float uDirLight0_specular;
#  endif
#  ifdef DIRECTIONAL_0_SHADOW_MAP
uniform sampler2D uDirLight0_shadowMap;
uniform mat4 uDirLight0_viewProjection[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight0_zNear[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight0_zFar[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight0_shadowMaxDistance;
uniform vec4 uDirLight0_shadowSplitNear;
uniform vec4 uDirLight0_shadowSplitFar;
uniform float uDirLight0_shadowMapSize;
uniform float uDirLight0_shadowSpread;
uniform float uDirLight0_shadowBias;
#  endif
# endif

# if NUM_DIRECTIONAL_LIGHTS > 1
uniform vec3 uDirLight1_direction;
uniform vec3 uDirLight1_color;
uniform float uDirLight1_diffuse;
#  ifdef SHININESS
uniform float uDirLight1_specular;
#  endif
#  ifdef DIRECTIONAL_1_SHADOW_MAP
uniform sampler2D uDirLight1_shadowMap;
uniform mat4 uDirLight1_viewProjection[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight1_zNear[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight1_zFar[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight1_shadowMaxDistance;
uniform vec4 uDirLight1_shadowSplitNear;
uniform vec4 uDirLight1_shadowSplitFar;
uniform float uDirLight1_shadowMapSize;
uniform float uDirLight1_shadowSpread;
uniform float uDirLight1_shadowBias;
#   endif
# endif

# if NUM_DIRECTIONAL_LIGHTS > 2
uniform vec3 uDirLight2_direction;
uniform vec3 uDirLight2_color;
uniform float uDirLight2_diffuse;
#  ifdef SHININESS
uniform float uDirLight2_specular;
#  endif
#  ifdef DIRECTIONAL_2_SHADOW_MAP
uniform sampler2D uDirLight2_shadowMap;
uniform mat4 uDirLight2_viewProjection[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight2_zNear[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight2_zFar[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight2_shadowMaxDistance;
uniform vec4 uDirLight2_shadowSplitNear;
uniform vec4 uDirLight2_shadowSplitFar;
uniform float uDirLight2_shadowMapSize;
uniform float uDirLight2_shadowSpread;
uniform float uDirLight2_shadowBias;
#   endif
# endif

# if NUM_DIRECTIONAL_LIGHTS > 3
uniform vec3 uDirLight3_direction;
uniform vec3 uDirLight3_color;
uniform float uDirLight3_diffuse;
#  ifdef SHININESS
uniform float uDirLight3_specular;
#  endif
#  ifdef DIRECTIONAL_3_SHADOW_MAP
uniform sampler2D uDirLight3_shadowMap;
uniform mat4 uDirLight3_viewProjection[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight3_zNear[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight3_zFar[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight3_shadowMaxDistance;
uniform vec4 uDirLight3_shadowSplitNear;
uniform vec4 uDirLight3_shadowSplitFar;
uniform float uDirLight3_shadowMapSize;
uniform float uDirLight3_shadowSpread;
uniform float uDirLight3_shadowBias;
#   endif
# endif

#endif

// ambient lights
#ifdef NUM_AMBIENT_LIGHTS

# if NUM_AMBIENT_LIGHTS > 0
uniform vec3 uAmbientLight0_color;
uniform float uAmbientLight0_ambient;
# endif

# if NUM_AMBIENT_LIGHTS > 1
uniform vec3 uAmbientLight1_color;
uniform float uAmbientLight1_ambient;
# endif

# if NUM_AMBIENT_LIGHTS > 2
uniform vec3 uAmbientLight2_color;
uniform float uAmbientLight2_ambient;
# endif

# if NUM_AMBIENT_LIGHTS > 3
uniform vec3 uAmbientLight3_color;
uniform float uAmbientLight3_ambient;
# endif

#endif

// point lights
#ifdef NUM_POINT_LIGHTS

# if NUM_POINT_LIGHTS > 0
uniform vec3 uPointLight0_color;
uniform float uPointLight0_diffuse;
uniform vec3 uPointLight0_attenuationCoeffs;
uniform vec3 uPointLight0_position;
#  ifdef SHININESS
uniform float uPointLight0_specular;
#  endif
# endif

# if NUM_POINT_LIGHTS > 1
uniform vec3 uPointLight1_color;
uniform float uPointLight1_diffuse;
uniform vec3 uPointLight1_attenuationCoeffs;
uniform vec3 uPointLight1_position;
#  ifdef SHININESS
uniform float uPointLight1_specular;
#  endif
# endif

# if NUM_POINT_LIGHTS > 2
uniform vec3 uPointLight2_color;
uniform float uPointLight2_diffuse;
uniform vec3 uPointLight2_attenuationCoeffs;
uniform vec3 uPointLight2_position;
#  ifdef SHININESS
uniform float uPointLight2_specular;
#  endif
# endif

# if NUM_POINT_LIGHTS > 3
uniform vec3 uPointLight3_color;
uniform float uPointLight3_diffuse;
uniform vec3 uPointLight3_attenuationCoeffs;
uniform vec3 uPointLight3_position;
#  ifdef SHININESS
uniform float uPointLight3_specular;
#  endif
# endif

#endif

// spot lights
#ifdef NUM_SPOT_LIGHTS

# if NUM_SPOT_LIGHTS > 0
uniform vec3 uSpotLight0_direction;
uniform float uSpotLight0_diffuse;
uniform vec3 uSpotLight0_position;
uniform float uSpotLight0_cosInnerConeAngle;
uniform vec3 uSpotLight0_color;
uniform float uSpotLight0_cosOuterConeAngle;
uniform vec3 uSpotLight0_attenuationCoeffs;
#  ifdef SHININESS
uniform float uSpotLight0_specular;
#  endif

#  ifdef SPOT_0_SHADOW_MAP
uniform sampler2D uSpotLight0_shadowMap;
uniform mat4 uSpotLight0_viewProjection;
uniform float uSpotLight0_zNear;
uniform float uSpotLight0_zFar;
uniform float uSpotLight0_shadowMaxDistance;
uniform float uSpotLight0_shadowMapSize;
uniform float uSpotLight0_shadowSpread;
uniform float uSpotLight0_shadowBias;
#  endif

# endif

# if NUM_SPOT_LIGHTS > 1
uniform vec3 uSpotLight1_direction;
uniform float uSpotLight1_diffuse;
uniform vec3 uSpotLight1_position;
uniform float uSpotLight1_cosInnerConeAngle;
uniform vec3 uSpotLight1_color;
uniform float uSpotLight1_cosOuterConeAngle;
uniform vec3 uSpotLight1_attenuationCoeffs;
#  ifdef SHININESS
uniform float uSpotLight1_specular;
#  endif

#  ifdef SPOT_1_SHADOW_MAP
uniform sampler2D uSpotLight1_shadowMap;
uniform mat4 uSpotLight1_viewProjection;
uniform float uSpotLight1_zNear;
uniform float uSpotLight1_zFar;
uniform float uSpotLight1_shadowMaxDistance;
uniform float uSpotLight1_shadowMapSize;
uniform float uSpotLight1_shadowSpread;
uniform float uSpotLight1_shadowBias;
#  endif

# endif

# if NUM_SPOT_LIGHTS > 2
uniform vec3 uSpotLight2_direction;
uniform float uSpotLight2_diffuse;
uniform vec3 uSpotLight2_position;
uniform float uSpotLight2_cosInnerConeAngle;
uniform vec3 uSpotLight2_color;
uniform float uSpotLight2_cosOuterConeAngle;
uniform vec3 uSpotLight2_attenuationCoeffs;
#  ifdef SHININESS
uniform float uSpotLight2_specular;
#  endif

#  ifdef SPOT_2_SHADOW_MAP
uniform sampler2D uSpotLight2_shadowMap;
uniform mat4 uSpotLight2_viewProjection;
uniform float uSpotLight2_zNear;
uniform float uSpotLight2_zFar;
uniform float uSpotLight2_shadowMaxDistance;
uniform float uSpotLight2_shadowMapSize;
uniform float uSpotLight2_shadowSpread;
uniform float uSpotLight2_shadowBias;
#  endif

# endif

# if NUM_SPOT_LIGHTS > 3
uniform vec3 uSpotLight3_direction;
uniform float uSpotLight3_diffuse;
uniform vec3 uSpotLight3_position;
uniform float uSpotLight3_cosInnerConeAngle;
uniform vec3 uSpotLight3_color;
uniform float uSpotLight3_cosOuterConeAngle;
uniform vec3 uSpotLight3_attenuationCoeffs;
#  ifdef SHININESS
uniform float uSpotLight3_specular;
#  endif
# endif

#  ifdef SPOT_3_SHADOW_MAP
uniform sampler2D uSpotLight3_shadowMap;
uniform mat4 uSpotLight3_viewProjection;
uniform float uSpotLight3_zNear;
uniform float uSpotLight3_zFar;
uniform float uSpotLight3_shadowMaxDistance;
uniform float uSpotLight3_shadowMapSize;
uniform float uSpotLight3_shadowSpread;
uniform float uSpotLight3_shadowBias;
#  endif

#endif

varying vec3 vVertexPosition;
varying vec2 vVertexUV;
varying vec2 vVertexUV1;
varying vec3 vVertexNormal;
varying vec3 vVertexTangent;
varying vec4 vVertexScreenPosition;
varying vec4 vVertexColor;

float getDirectionalLightShadow(sampler2D 	shadowMap,
				                mat4 		viewProj[SHADOW_MAPPING_MAX_NUM_CASCADES],
				                float 		zNear[SHADOW_MAPPING_MAX_NUM_CASCADES],
				                float 		zFar[SHADOW_MAPPING_MAX_NUM_CASCADES],
				                vec4		splitNear,
                                vec4        splitFar,
				                float 		size,
				                float 		bias,
                                float       maxDistance)
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
		float shadowDepth = vertexLightPosition.z + bias;
		vec2 depthUV = vertexLightPosition.xy / 2.0 + 0.5;

		depthUV = vec2(depthUV.xy * viewport.zw + viewport.xy);

		#if SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_DEFAULT
			shadow = shadowMapping_texture2DCompare(shadowMap, depthUV, shadowDepth, near, far);
		#elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_PCF
			shadow = shadowMapping_PCF(shadowMap, vec2(size), depthUV, shadowDepth, near, far);
		#elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_ESM
			shadow = shadowMapping_ESM(shadowMap, depthUV, shadowDepth, near, far, (far - near) * 1.5);
		#elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_PCF_POISSON
			shadow = shadowMapping_PCFPoisson(shadowMap, vec2(size), depthUV, shadowDepth, near, far, vec4(vVertexPosition.xy, vVertexNormal.xy));
		#endif

        shadow = shadowMapping_applyDistanceFade(vVertexScreenPosition.z, splitFar.w, shadow, maxDistance);
	}

	return shadow;
}

float getSpotLightShadow(sampler2D   shadowMap,
                         mat4        viewProjection,
                         float       zNear,
                         float       zFar,
                         float       size,
                         float       bias,
                         float       maxDistance)
{
    float shadow = 1.0;
    float near = zNear;
    float far = zFar;
    vec4 vertexLightPosition = viewProjection * vec4(vVertexPosition, 1);

    vertexLightPosition /= vertexLightPosition.w;

    if (shadowMapping_vertexIsInShadowMap(vertexLightPosition.xyz))
    {
        float shadowDepth = vertexLightPosition.z + (bias / vertexLightPosition.w);
        vec2 depthUV = vertexLightPosition.xy / 2.0 + 0.5;

        #if SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_DEFAULT
            shadow = shadowMapping_texture2DCompare(shadowMap, depthUV, shadowDepth, near, far);
        #elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_PCF
            shadow = shadowMapping_PCF(shadowMap, vec2(size), depthUV, shadowDepth, near, far);
        #elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_ESM
            shadow = shadowMapping_ESM(shadowMap, depthUV, shadowDepth, near, far, (far - near) * 1.5);
        #elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_PCF_POISSON
            shadow = shadowMapping_PCFPoisson(shadowMap, vec2(size), depthUV, shadowDepth, near, far, vec4(vVertexPosition.xy, vVertexNormal.xy));
        #endif

        shadow = shadowMapping_applyDistanceFade(vVertexScreenPosition.z, zFar, shadow, maxDistance);
    }

    return shadow;
}

void main(void)
{
	vec3 ambientAccum = vec3(0.0);
	vec3 diffuseAccum = vec3(0.0);
	vec3 specularAccum	= vec3(0.0);
	vec4 diffuse = uDiffuseColor;
	vec4 specular = uSpecularColor;
	float shininessCoeff = 1.0;
	vec3 eyeVector = normalize(uCameraPosition - vVertexPosition); // always in world-space
	vec3 normalVector = normalize(vVertexNormal); // always in world-space

    #if defined VERTEX_UV && (defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP || defined ALPHA_MAP)
        vec2 uv = vVertexUV;

        #ifdef UV_SCALE
            uv *= uUVScale;
        #endif // UV_SCALE

        #ifdef UV_OFFSET
            uv += uUVOffset;
        #endif // UV_OFFSET
    #endif // defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP || defined ALPHA_MAP

	#ifdef VERTEX_COLOR
		diffuse = vVertexColor;
	#endif // VERTEX_COLOR

	#ifdef SHININESS
		shininessCoeff = max(1.0, uShininess);
	#endif // SHININESS

	#if defined(DIFFUSE_MAP) && defined(VERTEX_UV)
		#ifdef DIFFUSE_MAP_LOD
			diffuse = texturelod_texture2D(uDiffuseMap, uv, uDiffuseMapSize, 0.0, uDiffuseMapMaxAvailableLod, diffuse);
		#else
			diffuse = texture2D(uDiffuseMap, uv);
		#endif

        #ifdef GAMMA_CORRECTION
            diffuse.rgb = pow(diffuse.rgb, vec3(uGammaCorrection));
        #endif // GAMMA_CORRECTION
	#endif // DIFFUSE_MAP

    #if defined(ALPHA_MAP) && defined(VERTEX_UV)
        #ifdef ALPHA_MAP_LOD
            diffuse.a = texturelod_texture2D(uAlphaMap, uv, uAlphaMapSize, 0.0, uAlphaMapMaxAvailableLod, vec4(diffuse.a)).r;
        #else
            diffuse.a = texture2D(uAlphaMap, uv).r;
        #endif // ALPHA_MAP_LOD
    #endif // ALPHA_MAP

	#ifdef ALPHA_THRESHOLD
		if (diffuse.a < uAlphaThreshold)
			discard;
	#endif // ALPHA_THRESHOLD

    #if (defined (VERTEX_UV) || defined(VERTEX_UV1)) && defined(LIGHT_MAP)
        vec2 lightMapUV = vec2(0.0);

        #ifdef VERTEX_UV1
            lightMapUV = vVertexUV1;
        #else
            lightMapUV = vVertexUV;
        #endif

        vec3 lightMapColor = vec3(0.0);

        #ifdef LIGHT_MAP_LOD
            lightMapColor = texturelod_texture2D(uLightMap, lightMapUV, uLightMapSize, 0.0, uLightMapMaxAvailableLod, vec4(1.0)).rgb;
        #else
            lightMapColor = texture2D(uLightMap, lightMapUV).rgb;
        #endif

        #ifdef GAMMA_CORRECTION
            lightMapColor = pow(lightMapColor, vec3(uGammaCorrection));
        #endif

        diffuseAccum += lightMapColor;
    #endif // (VERTEX_UV || VERTEX_UV1) && LIGHT_MAP

	#if defined(SHININESS) || ( (defined(ENVIRONMENT_MAP_2D) || defined(ENVIRONMENT_CUBE_MAP)) && !defined(ENVIRONMENT_ALPHA) )
		#if defined(SPECULAR_MAP) && defined(VERTEX_UV)
			#ifdef SPECULAR_MAP_LOD
				specular = texturelod_texture2D(uSpecularMap, uv, uSpecularMapSize, 0.0, uSpecularMapMaxAvailableLod, uSpecularColor);
			#else
				specular = texture2D(uSpecularMap, uv);
			#endif
		// #elif defined(NORMAL_MAP) && defined(VERTEX_UV)
		// 	specular.a = texture2D(uNormalMap, uv).a; // ???

            #ifdef GAMMA_CORRECTION
                specular.rgb = pow(specular.rgb, vec3(uGammaCorrection));
            #endif // GAMMA_CORRECTION
		#endif // SPECULAR_MAP
	#endif

    #ifdef NUM_IMAGE_BASED_LIGHTS
        #if NUM_IMAGE_BASED_LIGHTS > 0
            vec3 imageBasedLight0Diffuse = pbr_envDiffuse(
                uImageBasedLight0_irradianceMap,
                normalVector,
                vec2(uImageBasedLight0_orientation, 0.0)
            );

            #ifdef GAMMA_CORRECTION
                imageBasedLight0Diffuse = pow(imageBasedLight0Diffuse, vec3(uGammaCorrection));
            #endif // GAMMA_CORRECTION

            vec3 imageBasedLight0Specular = pbr_envSpecular(
                specular.rgb,
                uImageBasedLight0_radianceMap,
                10,
                pbr_shininessToRoughness(shininessCoeff),
                reflect(-eyeVector, normalVector),
                saturate(dot(normalVector, eyeVector)),
                normalVector,
                eyeVector,
                vec2(uImageBasedLight0_orientation, 0.0)
            );

            #ifdef GAMMA_CORRECTION
                imageBasedLight0Specular = pow(imageBasedLight0Specular, vec3(uGammaCorrection));
            #endif

            diffuseAccum += toneMapping_toneMap(uImageBasedLight0_diffuse * imageBasedLight0Diffuse, uImageBasedLight0_brightness);
            specularAccum += toneMapping_toneMap(uImageBasedLight0_specular * imageBasedLight0Specular, uImageBasedLight0_brightness);
        #endif // NUM_IMAGE_BASED_LIGHTS > 0
    #endif

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

	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS
		#if defined(NORMAL_MAP) && defined(VERTEX_UV)
			// warning: the normal vector must be normalized at this point!
			mat3 tangentToWorldMatrix = phong_getTangentToWorldSpaceMatrix(normalVector, vVertexTangent);

			// bring normal from tangent-space normal to world-space
			#ifdef NORMAL_MAP_LOD
				vec4 normalColor = texturelod_texture2D(uNormalMap, uv, uNormalMapSize, 0.0, uNormalMapMaxAvailableLod, vec4(0.0));
			#else
				vec4 normalColor = texture2D(uNormalMap, uv);
			#endif

			normalVector = tangentToWorldMatrix * normalize(2.0 * normalColor.xyz - 1.0);
		#endif // NORMAL_MAP

		float shadow;
		vec3 dir;
        vec3 lightFresnel = vec3(1.0);
        #ifdef NUM_DIRECTIONAL_LIGHTS

		#if NUM_DIRECTIONAL_LIGHTS > 0
			shadow = 1.0;
			dir = normalize(-uDirLight0_direction);
			#ifdef DIRECTIONAL_0_SHADOW_MAP
				shadow = getDirectionalLightShadow(uDirLight0_shadowMap, uDirLight0_viewProjection, uDirLight0_zNear, uDirLight0_zFar, uDirLight0_shadowSplitNear, uDirLight0_shadowSplitFar, uDirLight0_shadowMapSize, uDirLight0_shadowBias, uDirLight0_shadowMaxDistance);
			#endif
			diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uDirLight0_diffuse * uDirLight0_color;
			#if defined(SHININESS)
                #if defined(FRESNEL)
                    lightFresnel = phong_fresnel(specular.rgb, -dir, eyeVector);
                #endif // FRESNEL
				specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uDirLight0_color * uDirLight0_specular
					* lightFresnel;
			#endif // SHININESS
		#endif // NUM_DIRECTIONAL_LIGHTS > 0
		#if NUM_DIRECTIONAL_LIGHTS > 1
			shadow = 1.0;
			dir = normalize(-uDirLight1_direction);
			#ifdef DIRECTIONAL_1_SHADOW_MAP
				shadow = getDirectionalLightShadow(uDirLight1_shadowMap, uDirLight1_viewProjection, uDirLight1_zNear, uDirLight1_zFar, uDirLight1_shadowSplitNear, uDirLight1_shadowSplitFar, uDirLight1_shadowMapSize, uDirLight1_shadowBias, uDirLight1_shadowMaxDistance);
			#endif
			diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uDirLight1_diffuse * uDirLight1_color;
			#if defined(SHININESS)
                #if defined(FRESNEL)
                    lightFresnel = phong_fresnel(specular.rgb, -dir, eyeVector);
                #endif // FRESNEL
				specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uDirLight1_color * uDirLight1_specular
					* lightFresnel;
			#endif // SHININESS
		#endif // NUM_DIRECTIONAL_LIGHTS > 1
		#if NUM_DIRECTIONAL_LIGHTS > 2
			shadow = 1.0;
			dir = normalize(-uDirLight2_direction);
			#ifdef DIRECTIONAL_2_SHADOW_MAP
				shadow = getDirectionalLightShadow(uDirLight2_shadowMap, uDirLight2_viewProjection, uDirLight2_zNear, uDirLight2_zFar, uDirLight2_shadowSplitNear, uDirLight2_shadowSplitFar, uDirLight2_shadowMapSize, uDirLight2_shadowBias, uDirLight2_shadowMaxDistance);
			#endif
			diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uDirLight2_diffuse * uDirLight2_color;
			#if defined(SHININESS)
                #if defined(FRESNEL)
                    lightFresnel = phong_fresnel(specular.rgb, -dir, eyeVector);
                #endif // FRESNEL
				specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uDirLight2_color * uDirLight2_specular
					* lightFresnel;
			#endif // SHININESS
		#endif // NUM_DIRECTIONAL_LIGHTS > 2
		#if NUM_DIRECTIONAL_LIGHTS > 3
			shadow = 1.0;
			dir = normalize(-uDirLight3_direction);
			#ifdef DIRECTIONAL_3_SHADOW_MAP
				shadow = getDirectionalLightShadow(uDirLight3_shadowMap, uDirLight3_viewProjection, uDirLight3_zNear, uDirLight3_zFar, uDirLight3_shadowSplitNear, uDirLight3_shadowSplitFar, uDirLight3_shadowMapSize, uDirLight3_shadowBias, uDirLight3_shadowMaxDistance);
			#endif
			diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uDirLight3_diffuse * uDirLight3_color;
			#if defined(SHININESS)
                #if defined(FRESNEL)
                    lightFresnel = phong_fresnel(specular.rgb, -dir, eyeVector);
                #endif // FRESNEL
				specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uDirLight3_color * uDirLight3_specular
					* lightFresnel;
			#endif // SHININESS
		#endif // NUM_DIRECTIONAL_LIGHTS > 3

        #endif // defined(NUM_DIRECTIONAL_LIGHTS)

        float dist = 0.;
        vec3 distVec = vec3(0.);
        float att = 0.;
        #ifdef NUM_POINT_LIGHTS
    		#if NUM_POINT_LIGHTS > 0
                dir = normalize(uPointLight0_position - vVertexPosition);
                dist = length(uPointLight0_position - vVertexPosition);
                distVec = vec3(1.0, dist, dist * dist);
                att = any(lessThan(uPointLight0_attenuationCoeffs, vec3(0.0))) ? 1.0 : max(0.0, 1.0 - dist / dot(uPointLight0_attenuationCoeffs, distVec));
                diffuseAccum += phong_diffuseReflection(normalVector, dir) * uPointLight0_color * uPointLight0_diffuse;// * att;
                #if defined(SHININESS)
                    #if defined(FRESNEL)
                        lightFresnel = phong_fresnel(specular.rgb, dir, eyeVector);
                    #endif // FRESNEL
                    specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uPointLight0_color * (uPointLight0_specular * att)
                        * lightFresnel;
                #endif // defined(SHININESS)
            #endif // NUM_POINT_LIGHTS > 0
            #if NUM_POINT_LIGHTS > 1
                dir = normalize(uPointLight1_position - vVertexPosition);
                dist = length(uPointLight1_position - vVertexPosition);
                distVec = vec3(1.0, dist, dist * dist);
                att = any(lessThan(uPointLight1_attenuationCoeffs, vec3(0.0))) ? 1.0 : max(0.0, 1.0 - dist / dot(uPointLight1_attenuationCoeffs, distVec));
                diffuseAccum += phong_diffuseReflection(normalVector, dir) * uPointLight1_color * uPointLight1_diffuse;// * att;
                #if defined(SHININESS)
                    #if defined(FRESNEL)
                        lightFresnel = phong_fresnel(specular.rgb, dir, eyeVector);
                    #endif // FRESNEL
                    specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uPointLight1_color * (uPointLight1_specular * att)
                        * lightFresnel;
                #endif // defined(SHININESS)
            #endif // NUM_POINT_LIGHTS > 1
            #if NUM_POINT_LIGHTS > 2
                dir = normalize(uPointLight2_position - vVertexPosition);
                dist = length(uPointLight2_position - vVertexPosition);
                distVec = vec3(1.0, dist, dist * dist);
                att = any(lessThan(uPointLight2_attenuationCoeffs, vec3(0.0))) ? 1.0 : max(0.0, 1.0 - dist / dot(uPointLight2_attenuationCoeffs, distVec));
                diffuseAccum += phong_diffuseReflection(normalVector, dir) * uPointLight2_color * uPointLight2_diffuse;// * att;
                #if defined(SHININESS)
                    #if defined(FRESNEL)
                        lightFresnel = phong_fresnel(specular.rgb, dir, eyeVector);
                    #endif // FRESNEL
                    specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uPointLight2_color * (uPointLight2_specular * att)
                        * lightFresnel;
                #endif // defined(SHININESS)
            #endif // NUM_POINT_LIGHTS > 2
            #if NUM_POINT_LIGHTS > 3
                dir = normalize(uPointLight3_position - vVertexPosition);
                dist = length(uPointLight3_position - vVertexPosition);
                distVec = vec3(1.0, dist, dist * dist);
                att = any(lessThan(uPointLight3_attenuationCoeffs, vec3(0.0))) ? 1.0 : max(0.0, 1.0 - dist / dot(uPointLight3_attenuationCoeffs, distVec));
                diffuseAccum += phong_diffuseReflection(normalVector, dir) * uPointLight3_color * uPointLight3_diffuse;// * att;
                #if defined(SHININESS)
                    #if defined(FRESNEL)
                        lightFresnel = phong_fresnel(specular.rgb, dir, eyeVector);
                    #endif // FRESNEL
                    specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uPointLight3_color * (uPointLight3_specular * att)
                        * lightFresnel;
                #endif // defined(SHININESS)
            #endif // NUM_POINT_LIGHTS > 3
        #endif // defined(NUM_POINT_LIGHTS)

        float cosSpot = 0.;
        float cutoff = 0.;
        #ifdef NUM_SPOT_LIGHTS
    		#if NUM_SPOT_LIGHTS > 0
                shadow = 1.0;
                dir = normalize(uSpotLight0_position - vVertexPosition);
                dist = length(uSpotLight0_position - vVertexPosition);
                cosSpot = dot(-dir, uSpotLight0_direction);
                if (uSpotLight0_cosOuterConeAngle < cosSpot)
                {
                    distVec = vec3(1.0, dist, dist * dist);
                    att = any(lessThan(uSpotLight0_attenuationCoeffs, vec3(0.0))) ? 1.0 : max(0.0, 1.0 - dist / dot(uSpotLight0_attenuationCoeffs, distVec));
                    cutoff = cosSpot < uSpotLight0_cosInnerConeAngle && uSpotLight0_cosOuterConeAngle < uSpotLight0_cosInnerConeAngle
    					? (cosSpot - uSpotLight0_cosOuterConeAngle) / (uSpotLight0_cosInnerConeAngle - uSpotLight0_cosOuterConeAngle)
    					: 1.0;

                    #ifdef SPOT_0_SHADOW_MAP
                        shadow = getSpotLightShadow(uSpotLight0_shadowMap, uSpotLight0_viewProjection, uSpotLight0_zNear, uSpotLight0_zFar, uSpotLight0_shadowMapSize, uSpotLight0_shadowBias, uSpotLight0_shadowMaxDistance);
                    #endif

                    diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uSpotLight0_color * uSpotLight0_diffuse * att * cutoff;
                    #ifdef SHININESS
    					specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uSpotLight0_color * uSpotLight0_specular * att * cutoff
    						* lightFresnel;
    				#endif // defined(SHININESS)
                }
            #endif // NUM_SPOT_LIGHTS > 0
            #if NUM_SPOT_LIGHTS > 1
                dir = normalize(uSpotLight1_position - vVertexPosition);
                dist = length(uSpotLight1_position - vVertexPosition);
                cosSpot = dot(-dir, uSpotLight1_direction);
                if (uSpotLight1_cosOuterConeAngle < cosSpot)
                {
                    distVec = vec3(1.0, dist, dist * dist);
                    att = any(lessThan(uSpotLight1_attenuationCoeffs, vec3(0.0))) ? 1.0 : max(0.0, 1.0 - dist / dot(uSpotLight1_attenuationCoeffs, distVec));
                    cutoff = cosSpot < uSpotLight1_cosInnerConeAngle && uSpotLight1_cosOuterConeAngle < uSpotLight1_cosInnerConeAngle
    					? (cosSpot - uSpotLight1_cosOuterConeAngle) / (uSpotLight1_cosInnerConeAngle - uSpotLight1_cosOuterConeAngle)
    					: 1.0;

                    #ifdef SPOT_1_SHADOW_MAP
                        shadow = getSpotLightShadow(uSpotLight1_shadowMap, uSpotLight1_viewProjection, uSpotLight1_zNear, uSpotLight1_zFar, uSpotLight1_shadowMapSize, uSpotLight1_shadowBias, uSpotLight1_shadowMaxDistance);
                    #endif

                    diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uSpotLight1_color * uSpotLight1_diffuse * att * cutoff;
                    #ifdef SHININESS
                        #if defined(FRESNEL)
                            lightFresnel = phong_fresnel(specular.rgb, dir, eyeVector);
                        #endif // FRESNEL
    					specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uSpotLight1_color * uSpotLight1_specular * att * cutoff
    						* lightFresnel;
    				#endif // defined(SHININESS)
                }
            #endif // NUM_SPOT_LIGHTS > 1
            #if NUM_SPOT_LIGHTS > 2
                dir = normalize(uSpotLight2_position - vVertexPosition);
                dist = length(uSpotLight2_position - vVertexPosition);
                cosSpot = dot(-dir, uSpotLight2_direction);
                if (uSpotLight2_cosOuterConeAngle < cosSpot)
                {
                    distVec = vec3(1.0, dist, dist * dist);
                    att = any(lessThan(uSpotLight2_attenuationCoeffs, vec3(0.0))) ? 1.0 : max(0.0, 1.0 - dist / dot(uSpotLight2_attenuationCoeffs, distVec));
                    cutoff = cosSpot < uSpotLight2_cosInnerConeAngle && uSpotLight2_cosOuterConeAngle < uSpotLight2_cosInnerConeAngle
    					? (cosSpot - uSpotLight2_cosOuterConeAngle) / (uSpotLight2_cosInnerConeAngle - uSpotLight2_cosOuterConeAngle)
    					: 1.0;

                    #ifdef SPOT_2_SHADOW_MAP
                        shadow = getSpotLightShadow(uSpotLight2_shadowMap, uSpotLight2_viewProjection, uSpotLight2_zNear, uSpotLight2_zFar, uSpotLight2_shadowMapSize, uSpotLight2_shadowBias, uSpotLight2_shadowMaxDistance);
                    #endif

                    diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uSpotLight2_color * uSpotLight2_diffuse * att * cutoff;
                    #ifdef SHININESS
                        #if defined(FRESNEL)
                            lightFresnel = phong_fresnel(specular.rgb, dir, eyeVector);
                        #endif // FRESNEL
    					specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uSpotLight2_color * uSpotLight2_specular * att * cutoff
    						* lightFresnel;
    				#endif // defined(SHININESS)
                }
            #endif // NUM_SPOT_LIGHTS > 2
            #if NUM_SPOT_LIGHTS > 3
                dir = normalize(uSpotLight3_position - vVertexPosition);
                dist = length(uSpotLight3_position - vVertexPosition);
                cosSpot = dot(-dir, uSpotLight3_direction);
                if (uSpotLight3_cosOuterConeAngle < cosSpot)
                {
                    distVec = vec3(1.0, dist, dist * dist);
                    att = any(lessThan(uSpotLight3_attenuationCoeffs, vec3(0.0))) ? 1.0 : max(0.0, 1.0 - dist / dot(uSpotLight3_attenuationCoeffs, distVec));
                    cutoff = cosSpot < uSpotLight3_cosInnerConeAngle && uSpotLight3_cosOuterConeAngle < uSpotLight3_cosInnerConeAngle
    					? (cosSpot - uSpotLight3_cosOuterConeAngle) / (uSpotLight3_cosInnerConeAngle - uSpotLight3_cosOuterConeAngle)
    					: 1.0;

                    #ifdef SPOT_3_SHADOW_MAP
                        shadow = getSpotLightShadow(uSpotLight3_shadowMap, uSpotLight3_viewProjection, uSpotLight3_zNear, uSpotLight3_zFar, uSpotLight3_shadowMapSize, uSpotLight3_shadowBias, uSpotLight3_shadowMaxDistance);
                    #endif

                    diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uSpotLight3_color * uSpotLight3_diffuse * att * cutoff;
                    #ifdef SHININESS
                        #if defined(FRESNEL)
                            lightFresnel = phong_fresnel(specular.rgb, dir, eyeVector);
                        #endif // FRESNEL
    					specularAccum += phong_specularReflection(normalVector, dir, eyeVector, shininessCoeff) * uSpotLight3_color * uSpotLight3_specular * att * cutoff
    						* lightFresnel;
    				#endif // defined(SHININESS)
                }
            #endif // NUM_SPOT_LIGHTS > 3
        #endif // defined(NUM_SPOT_LIGHTS)

	#endif // defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS

	vec3 phong = diffuse.rgb * (ambientAccum + diffuseAccum) + specular.rgb * specular.a * specularAccum;

	#if defined(ENVIRONMENT_MAP_2D) || defined(ENVIRONMENT_CUBE_MAP)

		#if defined(ENVIRONMENT_MAP_2D)
			vec4 envmapColor = envmap_sampleEnvironmentMap2D(uEnvironmentMap2d, -eyeVector, normalVector);
		#elif defined(ENVIRONMENT_CUBE_MAP)
			vec4 envmapColor = envmap_sampleEnvironmentCubeMap(uEnvironmentCubemap, eyeVector, normalVector);
		#endif

        #if defined(GAMMA_CORRECTION)
            envmapColor.rgb = pow(envmapColor.rgb, vec3(uGammaCorrection));
        #endif // GAMMA_CORRECTION

        #if defined(ENVIRONMENT_ALPHA)
		    float reflectivity = uEnvironmentAlpha;
        #else
            float reflectivity = specular.a;
        #endif

        #if defined(FRESNEL)
            float fresnel = uFresnelReflectance + (1.0 - uFresnelReflectance) * pow(1.0 - dot(normalVector, eyeVector), uFresnelExponent);

            reflectivity *= fresnel;
        #endif // FRESNEL

		// diffuse.rgb = mix(diffuse.rgb, envmapColor.rgb, reflectivity);
        phong += envmapColor.rgb * reflectivity;
	#endif // defined(ENVIRONMENT_MAP_2D) || defined(ENVIRONMENT_CUBE_MAP)

	#ifdef FOG_TECHNIQUE
		phong = fog_sampleFog(phong, vVertexScreenPosition.z, uFogColor.xyz, uFogColor.a, uFogBounds.x, uFogBounds.y);
	#endif

    #ifdef GAMMA_CORRECTION
        phong.rgb = pow(phong.rgb, vec3(1.0 / uGammaCorrection));
    #endif

	gl_FragColor = vec4(phong.rgb, diffuse.a);
}

#endif // FRAGMENT_SHADER
