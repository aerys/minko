#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include "Envmap.function.glsl"
#pragma include "Pack.function.glsl"
#pragma include "Phong.function.glsl"
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

// directional lights
uniform vec3 directionalLight0_direction;
uniform vec3 directionalLight0_color;
uniform float directionalLight0_diffuse;
uniform float directionalLight0_specular;
uniform sampler2D directionalLight0_shadowMap;
uniform float directionalLight0_shadowMapSize;
uniform float directionalLight0_shadowSpread;
uniform float directionalLight0_shadowBias;
uniform float directionalLight0_zNear;
uniform float directionalLight0_zFar;
uniform mat4 directionalLight0_viewProjection;

uniform vec3 directionalLight1_direction;
uniform vec3 directionalLight1_color;
uniform float directionalLight1_diffuse;
uniform float directionalLight1_specular;
uniform sampler2D directionalLight1_shadowMap;
uniform float directionalLight1_shadowMapSize;
uniform float directionalLight1_shadowSpread;
uniform float directionalLight1_shadowBias;
uniform float directionalLight1_zNear;
uniform float directionalLight1_zFar;
uniform mat4 directionalLight1_viewProjection;

uniform vec3 directionalLight2_direction;
uniform vec3 directionalLight2_color;
uniform float directionalLight2_diffuse;
uniform float directionalLight2_specular;
uniform sampler2D directionalLight2_shadowMap;
uniform float directionalLight2_shadowMapSize;
uniform float directionalLight2_shadowSpread;
uniform float directionalLight2_shadowBias;
uniform float directionalLight2_zNear;
uniform float directionalLight2_zFar;
uniform mat4 directionalLight2_viewProjection;

uniform vec3 directionalLight3_direction;
uniform vec3 directionalLight3_color;
uniform float directionalLight3_diffuse;
uniform float directionalLight3_specular;
uniform sampler2D directionalLight3_shadowMap;
uniform float directionalLight3_shadowMapSize;
uniform float directionalLight3_shadowSpread;
uniform float directionalLight3_shadowBias;
uniform float directionalLight3_zNear;
uniform float directionalLight3_zFar;
uniform mat4 directionalLight3_viewProjection;
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

float getShadow(sampler2D shadowMap, mat4 viewProj, float size, float zNear, float zFar, float bias)
{
	float shadow = 1.f;
	vec3 vertexLightPosition = (viewProj * vec4(vertexPosition, 1)).xyz;

	// FIXME: avoid branch (usint step()?)
	if (shadowMapping_vertexIsInShadowMap(vertexLightPosition))
	{
		float shadowDepth = vertexLightPosition.z - bias;
		vec2 depthUV = vertexLightPosition.xy / 2.0 + 0.5;

		#if SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_HARD
			shadow = shadowMapping_texture2DCompare(shadowMap, depthUV, shadowDepth, zNear, zFar);
		#elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_ESM
			shadow = shadowMapping_ESM(shadowMap, depthUV, shadowDepth, zNear, zFar, 80);
		#elif SHADOW_MAPPING_TECHNIQUE == SHADOW_MAPPING_TECHNIQUE_PCF
			shadow = shadowMapping_PCF(
				shadowMap,
				vec2(size, size),
				depthUV,
				shadowDepth,
				zNear,
				zFar
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
	vec3 diffuseRef = phong_directionalLight(
		lightDirection, lightColor, lightDiffuse, lightSpecular, eyeVector, normalVector, specular.rgb, shininessCoeff
	);

	diffuseAccum += diffuseRef * shadow;

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
		diffuse = texture2D(uDiffuseMap, vertexUV);
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
			specular = texture2D(uSpecularMap, vertexUV);
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
			normalVector = tangentToWorldMatrix * normalize(2.0 * texture2D(normalMap, vertexUV).xyz - 1.0);
		#endif // NORMAL_MAP

		float shadow;
		#if NUM_DIRECTIONAL_LIGHTS > 0
			shadow = 1.0;
			#ifdef DIRECTIONAL_0_SHADOW_MAP
				shadow = getShadow(directionalLight0_shadowMap, directionalLight0_viewProjection, directionalLight0_shadowMapSize, directionalLight0_zNear, directionalLight0_zFar, directionalLight0_shadowBias);
			#endif
			directionalLight(directionalLight0_direction, directionalLight0_color, directionalLight0_diffuse, directionalLight0_specular, shadow);
		#endif // NUM_DIRECTIONAL_LIGHTS > 0
		#if NUM_DIRECTIONAL_LIGHTS > 1
			shadow = 1.0;
			#ifdef DIRECTIONAL_1_SHADOW_MAP
				shadow = getShadow(directionalLight1_shadowMap, directionalLight1_viewProjection, directionalLight1_shadowMapSize, directionalLight1_zNear, directionalLight1_zFar, directionalLight1_shadowBias);
			#endif
			directionalLight(directionalLight1_direction, directionalLight1_color, directionalLight1_diffuse, directionalLight1_specular, shadow);
		#endif // NUM_DIRECTIONAL_LIGHTS > 1
		#if NUM_DIRECTIONAL_LIGHTS > 2
			shadow = 1.0;
			#ifdef DIRECTIONAL_2_SHADOW_MAP
				shadow = getShadow(directionalLight2_shadowMap, directionalLight2_viewProjection, directionalLight2_shadowMapSize, directionalLight2_zNear, directionalLight2_zFar, directionalLight2_shadowBias);
			#endif
			directionalLight(directionalLight2_direction, directionalLight2_color, directionalLight2_diffuse, directionalLight2_specular, shadow);
		#endif // NUM_DIRECTIONAL_LIGHTS > 2
		#if NUM_DIRECTIONAL_LIGHTS > 3
			shadow = 1.0;
			#ifdef DIRECTIONAL_3_SHADOW_MAP
				shadow = getShadow(directionalLight3_shadowMap, directionalLight3_viewProjection, directionalLight3_shadowMapSize, directionalLight3_zNear, directionalLight3_zFar, directionalLight3_shadowBias);
			#endif
			directionalLight(directionalLight3_direction, directionalLight3_color, directionalLight3_diffuse, directionalLight3_specular, shadow);
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
