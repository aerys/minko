#ifdef FRAGMENT_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "TextureLod.extension.glsl"

#pragma include "Envmap.function.glsl"
#pragma include "Phong.function.glsl"
#pragma include "TextureLod.function.glsl"
#pragma include "ShadowMapping.function.glsl"
#pragma include "Fog.function.glsl"

// diffuse
uniform vec4 uDiffuseColor;
uniform sampler2D uDiffuseMap;

// alpha
uniform sampler2D uAlphaMap;
uniform float uAlphaThreshold;

// fog
uniform vec4 uFogColor;
uniform vec2 uFogBounds;

// phong
uniform vec4 uSpecularColor;
uniform sampler2D uNormalMap;
uniform sampler2D uSpecularMap;
uniform float uShininess;
uniform vec3 uCameraPosition;

// env. mapping
uniform float uEnvironmentAlpha;

// texture lod
uniform float uDiffuseMapMaxAvailableLod;
uniform vec2 uDiffuseMapSize;

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
// ! directional lights

varying vec3 vVertexPosition;
varying vec2 vVertexUV;
varying vec3 vVertexNormal;
varying vec3 vVertexTangent;
varying vec4 vVertexScreenPosition;

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
    vec4 weights = shadowMapping_getCascadeWeights(vertexScreenPosition.z, splitNear, splitFar);
	vec4 viewport = shadowMapping_getCascadeViewport(weights);
    mat4 viewProjection = shadowMapping_getCascadeViewProjection(weights, viewProj);
    float near = shadowMapping_getCascadeZ(weights, zNear);
    float far = shadowMapping_getCascadeZ(weights, zFar);
	vec3 vertexLightPosition = (viewProjection * vec4(vertexPosition, 1)).xyz;

	if (shadowMapping_vertexIsInShadowMap(vertexLightPosition))
	{
		float shadowDepth = vertexLightPosition.z - bias;
		vec2 depthUV = vertexLightPosition.xy / 2.0 + 0.5;

		depthUV = vec2(depthUV.xy * viewport.zw + viewport.xy);

		#if SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_HARD
			shadow = shadowMapping_texture2DCompare(shadowMap, depthUV, shadowDepth, near, far);
		#elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_ESM
			shadow = shadowMapping_ESM(shadowMap, depthUV, shadowDepth, near, far, 30.0);
		#elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_PCF
			shadow = shadowMapping_PCF(shadowMap, vec2(size, size), depthUV, shadowDepth, near, far);
		// #elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_PCF_POISSON
		// 	shadow = shadowMapping_PCFPoisson(
		// 		shadowMap,
		// 		vec2(size, size),
		// 		depthUV,
		// 		shadowDepth,
		// 		zNear,
		// 		zFar,
		// 		uShadowRandomMap,
		// 		vertexScreenPosition.xy / vertexScreenPosition.w / 2.0 + 0.5,
		// 		uShadowSpread
		// 	);
		#endif
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
	vec3 eyeVector = normalize(uCameraPosition - vertexPosition); // always in world-space
	vec3 normalVector = normalize(vertexNormal); // always in world-space

	#ifdef SHININESS
		shininessCoeff = max(1.0, uShininess);
	#endif // SHININESS

	#ifdef DIFFUSE_MAP
		#ifdef DIFFUSE_MAP_LOD
			diffuse = texturelod_texture2D(uDiffuseMap, vertexUV, uDiffuseMapSize, 0.0, uDiffuseMapMaxAvailableLod, uDiffuseColor);
		#else
			diffuse = texture2D(uDiffuseMap, vertexUV);
		#endif
	#endif // DIFFUSE_MAP

	#ifdef ALPHA_MAP
		diffuse.a = texture2D(uAlphaMap, vertexUV).r;
	#endif // ALPHA_MAP

	#ifdef ALPHA_THRESHOLD
		if (diffuse.a < uAlphaThreshold)
			discard;
	#endif // ALPHA_THRESHOLD

	#if defined(SHININESS) || ( (defined(ENVIRONMENT_MAP_2D) || defined(ENVIRONMENT_CUBE_MAP)) && !defined(ENVIRONMENT_ALPHA) )
		#ifdef SPECULAR_MAP
			#ifdef SPECULAR_MAP_LOD
				specular = texturelod_texture2D(uSpecularMap, vertexUV, uSpecularMapSize, 0.0, uSpecularMapMaxAvailableLod, uSpecularColor);
			#else
				specular = texture2D(uSpecularMap, vertexUV);
			#endif
		#elif defined NORMAL_MAP
			specular.a = texture2D(uNormalMap, vertexUV).a; // ???
		#endif // SPECULAR_MAP
	#endif

	#ifdef NUM_AMBIENT_LIGHTS
		for (int i = 0; i < NUM_AMBIENT_LIGHTS; ++i)
			ambientAccum += uAmbientLights[i].color * uAmbientLights[i].ambient;
	#endif // NUM_AMBIENT_LIGHTS

	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS
		#ifdef NORMAL_MAP
			// warning: the normal vector must be normalized at this point!
			mat3 tangentToWorldMatrix = phong_getTangentToWorldSpaceMatrix(normalVector, vertexTangent);

			// bring normal from tangent-space normal to world-space
			#ifdef NORMAL_MAP_LOD
				vec4 normalColor = texturelod_texture2D(uNormalMap, vertexUV, uNormalMapSize, 0.0, uNormalMapMaxAvailableLod, vec4(0.0));
			#else
				vec4 normalColor = texture2D(uNormalMap, vertexUV);
			#endif

			normalVector = tangentToWorldMatrix * normalize(2.0 * normalColor.xyz - 1.0);
		#endif // NORMAL_MAP

		float shadow;
		vec3 dir;
		#if NUM_DIRECTIONAL_LIGHTS > 0
			shadow = 1.0;
			dir = normalize(-uDirLight0_direction);
			#ifdef DIRECTIONAL_0_SHADOW_MAP
				shadow = getShadow(uDirLight0_shadowMap, uDirLight0_viewProjection, uDirLight0_zNear, uDirLight0_zFar, uDirLight0_shadowSplitNear, uDirLight0_shadowSplitFar, uDirLight0_shadowMapSize, uDirLight0_shadowBias);
			#endif
			diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uDirLight0_diffuse * uDirLight0_color;
			#if defined(SHININESS)
				specularAccum += phong_specularReflection(normalVector, uDirLight0_direction, eyeVector, shininessCoeff) * uDirLight0_color * uDirLight0_specular
					* phong_fresnel(specular.rgb, dir, eyeVector);
			#endif // SHININESS
		#endif // NUM_DIRECTIONAL_LIGHTS > 0
		#if NUM_DIRECTIONAL_LIGHTS > 1
			shadow = 1.0;
			dir = normalize(-uDirLight1_direction);
			#ifdef DIRECTIONAL_1_SHADOW_MAP
				shadow = getShadow(uDirLight1_shadowMap, uDirLight1_viewProjection, uDirLight1_zNear, uDirLight1_zFar, uDirLight1_shadowSplitNear, uDirLight1_shadowSplitFar, uDirLight1_shadowMapSize, uDirLight1_shadowBias);
			#endif
			diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uDirLight1_diffuse * uDirLight1_color;
			#if defined(SHININESS)
				specularAccum += phong_specularReflection(normalVector, uDirLight1_direction, eyeVector, shininessCoeff) * uDirLight1_color * uDirLight1_specular
					* phong_fresnel(specular.rgb, dir, eyeVector);
			#endif // SHININESS
		#endif // NUM_DIRECTIONAL_LIGHTS > 1
		#if NUM_DIRECTIONAL_LIGHTS > 2
			shadow = 1.0;
			dir = normalize(-uDirLight2_direction);
			#ifdef DIRECTIONAL_2_SHADOW_MAP
				shadow = getShadow(uDirLight2_shadowMap, uDirLight2_viewProjection, uDirLight2_zNear, uDirLight2_zFar, uDirLight2_shadowSplitNear, uDirLight2_shadowSplitFar, uDirLight2_shadowMapSize, uDirLight2_shadowBias);
			#endif
			diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uDirLight2_diffuse * uDirLight2_color;
			#if defined(SHININESS)
				specularAccum += phong_specularReflection(normalVector, uDirLight2_direction, eyeVector, shininessCoeff) * uDirLight2_color * uDirLight2_specular
					* phong_fresnel(specular.rgb, dir, eyeVector);
			#endif // SHININESS
		#endif // NUM_DIRECTIONAL_LIGHTS > 2
		#if NUM_DIRECTIONAL_LIGHTS > 3
			shadow = 1.0;
			dir = normalize(-uDirLight3_direction);
			#ifdef DIRECTIONAL_3_SHADOW_MAP
				shadow = getShadow(uDirLight3_shadowMap, uDirLight3_viewProjection, uDirLight3_zNear, uDirLight3_zFar, uDirLight3_shadowSplitNear, uDirLight3_shadowSplitFar, uDirLight3_shadowMapSize, uDirLight3_shadowBias);
			#endif
			diffuseAccum += phong_diffuseReflection(normalVector, dir) * shadow * uDirLight3_diffuse * uDirLight3_color;
			#if defined(SHININESS)
				specularAccum += phong_specularReflection(normalVector, uDirLight3_direction, eyeVector, shininessCoeff) * uDirLight3_color * uDirLight3_specular
					* phong_fresnel(specular.rgb, dir, eyeVector);
			#endif // SHININESS
		#endif // NUM_DIRECTIONAL_LIGHTS > 1

	#endif // defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS

	#if defined(ENVIRONMENT_MAP_2D) || defined(ENVIRONMENT_CUBE_MAP)
		vec4 envmapColor = envmap_sampleEnvironmentMap(eyeVector, normalVector);
		float reflectivity = specular.a;

		#ifdef ENVIRONMENT_ALPHA
			reflectivity = uEnvironmentAlpha;
		#endif // ENVIRONMENT_ALPHA

		diffuse.rgb = mix(diffuse.rgb, envmapColor.rgb, reflectivity);
	#endif // defined(ENVIRONMENT_MAP_2D) || defined(ENVIRONMENT_CUBE_MAP)

	// Final blend of ambient, diffuse, and specular parts
	//----------------------------------------------------
	vec3 phong = diffuse.rgb * (ambientAccum + diffuseAccum) + specular.a * specularAccum;

	#ifdef FOG_TECHNIQUE
		phong = fog_sampleFog(phong, vertexScreenPosition.z, uFogColor.xyz, uFogColor.a, uFogBounds.x, uFogBounds.y);
	#endif

	gl_FragColor = vec4(phong.rgb, diffuse.a);
}

#endif // FRAGMENT_SHADER
