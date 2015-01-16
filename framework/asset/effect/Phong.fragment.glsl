#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include "TextureLod.extension.glsl"

#pragma include "Envmap.function.glsl"
#pragma include "Pack.function.glsl"
#pragma include "Phong.function.glsl"
#pragma include "TextureLod.function.glsl"
#pragma include "ShadowMapping.function.glsl"

struct AmbientLight
{
	vec3 	color;
	float 	ambient;
};

struct PointLight
{
	vec3	color;
	float	diffuse;
	float	specular;
	vec3	attenuationCoeffs;
	vec3	position;
};

struct SpotLight
{
	vec3	direction;
	float	diffuse;
	vec3	position;
	float	cosInnerConeAngle;
	vec3	color;
	float	cosOuterConeAngle;
	vec3	attenuationCoeffs;
	float	specular;
};

#ifdef NUM_AMBIENT_LIGHTS
	uniform AmbientLight uAmbientLights[NUM_AMBIENT_LIGHTS];
#endif // NUM_AMBIENT_LIGHTS

#ifdef NUM_POINT_LIGHTS
	uniform PointLight uPointLights[NUM_POINT_LIGHTS];
#endif // NUM_POINT_LIGHTS

#ifdef NUM_SPOT_LIGHTS
	uniform SpotLight uSpotLights[NUM_SPOT_LIGHTS];
#endif // NUM_SPOT_LIGHTS

// diffuse
uniform vec4 uDiffuseColor;
uniform sampler2D uDiffuseMap;

// alpha
uniform sampler2D uAlphaMap;
uniform float uAlphaThreshold;

// phong
uniform vec4 uSpecularColor;
uniform sampler2D uNormalMap;
uniform sampler2D uSpecularMap;
uniform float uShininess;
uniform vec3 uCameraPosition;

// env. mapping
uniform float uEnvironmentAlpha;

// texture lod
uniform float 		uDiffuseMapMaxAvailableLod;
uniform vec2 		uDiffuseMapSize;

uniform float		uNormalMapMaxAvailableLod;
uniform vec2		uNormalMapSize;

uniform float		uSpecularMapMaxAvailableLod;
uniform vec2		uSpecularMapSize;

// directional lights
uniform vec3 uDirLight0_direction;
uniform vec3 uDirLight0_color;
uniform float uDirLight0_diffuse;
uniform float uDirLight0_specular;
uniform sampler2D uDirLight0_shadowMap;
uniform mat4 uDirLight0_viewProjection[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight0_zNear[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform float uDirLight0_zFar[SHADOW_MAPPING_MAX_NUM_CASCADES];
uniform vec4 uDirLight0_shadowCascadeDepths;
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
uniform vec4 uDirLight1_shadowCascadeDepths;
uniform float uDirLight1_shadowMapSize;
uniform float uDirLight1_shadowSpread;
uniform float uDirLight1_shadowBias;

uniform vec3 uDirLight2_direction;
uniform vec3 uDirLight2_color;
uniform float uDirLight2_diffuse;
uniform float uDirLight2_specular;
uniform sampler2D uDirLight2_shadowMap;
uniform float uDirLight2_shadowMapSize;
uniform float uDirLight2_shadowSpread;
uniform float uDirLight2_shadowBias;
uniform float uDirLight2_zNear;
uniform float uDirLight2_zFar;
uniform mat4 uDirLight2_viewProjection;

uniform vec3 uDirLight3_direction;
uniform vec3 uDirLight3_color;
uniform float uDirLight3_diffuse;
uniform float uDirLight3_specular;
uniform sampler2D uDirLight3_shadowMap;
uniform float uDirLight3_shadowMapSize;
uniform float uDirLight3_shadowSpread;
uniform float uDirLight3_shadowBias;
uniform float uDirLight3_zNear;
uniform float uDirLight3_zFar;
uniform mat4 uDirLight3_viewProjection;
// ! directional lights

varying vec3 vertexPosition;
varying vec2 vertexUV;
varying vec3 vertexNormal;
varying vec3 vertexTangent;
varying vec4 vertexScreenPosition;

vec3 ambientAccum = vec3(0.0);
vec3 diffuseAccum = vec3(0.0);
vec3 specularAccum	= vec3(0.0);
vec4 diffuse = uDiffuseColor;
vec4 specular = uSpecularColor;
float shininessCoeff = 1.0;
vec3 eyeVector = normalize(uCameraPosition - vertexPosition); // always in world-space
vec3 normalVector = normalize(vertexNormal); // always in world-space

float getShadow(sampler2D 	shadowMap,
				mat4 		viewProj[SHADOW_MAPPING_MAX_NUM_CASCADES],
				float 		zNear[SHADOW_MAPPING_MAX_NUM_CASCADES],
				float 		zFar[SHADOW_MAPPING_MAX_NUM_CASCADES],
				vec4		cascadeDepths,
				float 		size,
				float 		bias)
{
	float shadow = 1.f;
	int index = shadowMapping_getCascadeIndex(vertexScreenPosition.z, cascadeDepths);
	vec4 viewport = shadowMapping_viewports[index];
	vec3 vertexLightPosition = (viewProj[index] * vec4(vertexPosition, 1)).xyz;

	if (shadowMapping_vertexIsInShadowMap(vertexLightPosition))
	{
		float shadowDepth = vertexLightPosition.z - bias;
		vec2 depthUV = vertexLightPosition.xy / 2.0 + 0.5;

		depthUV = vec2(depthUV.xy * viewport.zw + viewport.xy);

		#if SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_HARD
			shadow = shadowMapping_texture2DCompare(shadowMap, depthUV, shadowDepth, zNear[index], zFar[index]);
		#elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_ESM
			shadow = shadowMapping_ESM(shadowMap, depthUV, shadowDepth, zNear[index], zFar[index], 30.0);
		#elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_PCF
			shadow = shadowMapping_PCF(
				shadowMap,
				vec2(size, size),
				depthUV,
				shadowDepth,
				zNear[index],
				zFar[index]
			);
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

void directionalLight(vec3 lightDirection, vec3 lightColor, float lightDiffuse, float lightSpecular, float shadow)
{
	diffuseAccum += phong_diffuseReflection(normalVector, normalize(-lightDirection))
		* shadow
		* lightDiffuse
		* lightColor;

	#if defined(SHININESS)
		specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
			* phong_fresnel(specular.rgb, normalize(-lightDirection), eyeVector)
			* lightColor
			* lightSpecular;
	#endif // SHININESS
}

void main(void)
{
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
		#if NUM_DIRECTIONAL_LIGHTS > 0
			shadow = 1.0;
			#ifdef DIRECTIONAL_0_SHADOW_MAP
				shadow = getShadow(uDirLight0_shadowMap, uDirLight0_viewProjection, uDirLight0_zNear, uDirLight0_zFar, uDirLight0_shadowCascadeDepths, uDirLight0_shadowMapSize, uDirLight0_shadowBias);
			#endif
			directionalLight(uDirLight0_direction, uDirLight0_color, uDirLight0_diffuse, uDirLight0_specular, shadow);
		#endif // NUM_DIRECTIONAL_LIGHTS > 0
		#if NUM_DIRECTIONAL_LIGHTS > 1
			shadow = 1.0;
			#ifdef DIRECTIONAL_1_SHADOW_MAP
				shadow = getShadow(uDirLight1_shadowMap, uDirLight1_viewProjection, uDirLight1_zNear, uDirLight1_zFar, uDirLight1_shadowCascadeDepths, uDirLight1_shadowMapSize, uDirLight1_shadowBias);
			#endif
			directionalLight(uDirLight1_direction, uDirLight1_color, uDirLight1_diffuse, uDirLight1_specular, shadow);
		#endif // NUM_DIRECTIONAL_LIGHTS > 1
		#if NUM_DIRECTIONAL_LIGHTS > 2
			shadow = 1.0;
			#ifdef DIRECTIONAL_2_SHADOW_MAP
				shadow = getShadow(uDirLight2_shadowMap, uDirLight2_viewProjection, uDirLight2_zNear, uDirLight2_zFar, uDirLight2_shadowCascadeDepths, uDirLight2_shadowMapSize, uDirLight2_shadowBias);
			#endif
			directionalLight(uDirLight2_direction, uDirLight2_color, uDirLight2_diffuse, uDirLight2_specular, shadow);
		#endif // NUM_DIRECTIONAL_LIGHTS > 2
		#if NUM_DIRECTIONAL_LIGHTS > 3
			shadow = 1.0;
			#ifdef DIRECTIONAL_3_SHADOW_MAP
				shadow = getShadow(uDirLight3_shadowMap, uDirLight3_viewProjection, uDirLight3_zNear, uDirLight3_zFar, uDirLight3_shadowCascadeDepths, uDirLight3_shadowMapSize, uDirLight3_shadowBias);
			#endif
			directionalLight(uDirLight3_direction, uDirLight3_color, uDirLight3_diffuse, uDirLight3_specular, shadow);
		#endif // NUM_DIRECTIONAL_LIGHTS > 1


		#ifdef NUM_POINT_LIGHTS
		//---------------------
			for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
			{
				vec3 lightDirection = uPointLights[i].position - vertexPosition;
				float distanceToLight = length(lightDirection);
				lightDirection /= distanceToLight;

				vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
				float attenuation = any(lessThan(uPointLights[i].attenuationCoeffs, vec3(0.0)))
					? 1.0
					: max(0.0, 1.0 - distanceToLight / dot(uPointLights[i].attenuationCoeffs, distVec));

				diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
					* uPointLights[i].color
					* (uPointLights[i].diffuse * attenuation);

				#if defined(SHININESS)
					specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
						* phong_fresnel(specular.rgb, lightDirection, eyeVector)
						* uPointLights[i].color
						* (uPointLights[i].specular * attenuation);
				#endif // SHININESS
			}
		#endif // NUM_POINT_LIGHTS

		#ifdef NUM_SPOT_LIGHTS
		//--------------------
			for (int i = 0; i < NUM_SPOT_LIGHTS; ++i)
			{
				vec3 lightDirection = uSpotLights[i].position - vertexPosition;
				float distanceToLight = length(lightDirection);
				lightDirection /= distanceToLight;

				vec3 lightSpotDirection = uSpotLights[i].direction;
				lightSpotDirection	= normalize(lightSpotDirection);
				float cosSpot = dot(-lightDirection, lightSpotDirection);

				if (uSpotLights[i].cosOuterConeAngle < cosSpot)
				{
					vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
					float attenuation = any(lessThan(uSpotLights[i].attenuationCoeffs, vec3(0.0)))
						? 1.0
						: max(0.0, 1.0 - distanceToLight / dot(uSpotLights[i].attenuationCoeffs, distVec));

					float cutoff = cosSpot < uSpotLights[i].cosInnerConeAngle && uSpotLights[i].cosOuterConeAngle < uSpotLights[i].cosInnerConeAngle
						? (cosSpot - uSpotLights[i].cosOuterConeAngle) / (uSpotLights[i].cosInnerConeAngle - uSpotLights[i].cosOuterConeAngle)
						: 1.0;

					diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
						* uSpotLights[i].color
						* uSpotLights[i].diffuse * attenuation * cutoff;

					#ifdef SHININESS
						specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
							* phong_fresnel(uSpotLights[i].specular.rgb, lightDirection, eyeVector)
							* uSpotLights[i].color
							* (uSpotLights[i].specular * attenuation * cutoff);
					#endif // SHININESS
				}
			}
		#endif // NUM_SPOT_LIGHTS

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

	gl_FragColor = vec4(phong.rgb, diffuse.a);
}

#endif // FRAGMENT_SHADER
