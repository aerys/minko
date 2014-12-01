#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("TextureLod.extension.glsl")

#pragma include("Phong.function.glsl")
#pragma include("Envmap.function.glsl")
#pragma include("Fog.function.glsl")
#pragma include("TextureLod.function.glsl")

#pragma include("Phong.struct.glsl")

#ifdef NUM_AMBIENT_LIGHTS
	uniform AmbientLight		ambientLights[NUM_AMBIENT_LIGHTS];
#endif // NUM_AMBIENT_LIGHTS

#ifdef NUM_DIRECTIONAL_LIGHTS
	uniform DirectionalLight	directionalLights[NUM_DIRECTIONAL_LIGHTS];
#endif // NUM_DIRECTIONAL_LIGHTS

#ifdef NUM_POINT_LIGHTS
	uniform PointLight			pointLights[NUM_POINT_LIGHTS];
#endif // NUM_POINT_LIGHTS

#ifdef NUM_SPOT_LIGHTS
	uniform SpotLight			spotLights[NUM_SPOT_LIGHTS];
#endif // NUM_SPOT_LIGHTS

// diffuse
uniform vec4 		diffuseColor;
uniform sampler2D 	diffuseMap;

// alpha
uniform sampler2D 	alphaMap;
uniform float 		alphaThreshold;

// phong
uniform vec4 		specularColor;
uniform sampler2D 	normalMap;
uniform sampler2D 	specularMap;
uniform float 		shininess;
uniform vec3 		cameraPosition;

// env. mapping
uniform float 		environmentAlpha;

// texture lod
uniform float 		diffuseMapMaxAvailableLod;
uniform vec2 		diffuseMapSize;

uniform float		normalMapMaxAvailableLod;
uniform vec2		normalMapSize;

uniform float		specularMapMaxAvailableLod;
uniform vec2		specularMapSize;

varying vec3 		vertexPosition;
varying vec2 		vertexUV;
varying vec3 		vertexNormal;
varying vec3 		vertexTangent;

void main(void)
{
	vec4 diffuse = diffuseColor;
	vec4 specular = specularColor;
	float shininessCoeff = 1.0;

	#ifdef SHININESS
		shininessCoeff = max(1.0, shininess);
	#endif // SHININESS

	#ifdef DIFFUSE_MAP
		#ifdef DIFFUSE_MAP_LOD
			diffuse = texturelod_texture2D(diffuseMap, vertexUV, diffuseMapSize, 0, diffuseMapMaxAvailableLod, diffuseColor);
		#else
			diffuse = texture2D(diffuseMap, vertexUV);
		#endif
	#endif // DIFFUSE_MAP

	#ifdef ALPHA_MAP
		diffuse.a 	= texture2D(alphaMap, vertexUV).r;
	#endif // ALPHA_MAP

	#ifdef ALPHA_THRESHOLD
		if (diffuse.a < alphaThreshold)
			discard;
	#endif // ALPHA_THRESHOLD

	#if defined(SHININESS) || ( (defined(ENVIRONMENT_MAP_2D) || defined(ENVIRONMENT_CUBE_MAP)) && !defined(ENVIRONMENT_ALPHA) )

		#ifdef SPECULAR_MAP
			#ifdef SPECULAR_MAP_LOD
				specular = texturelod_texture2D(specularMap, vertexUV, specularMapSize, 0, specularMapMaxAvailableLod, specularColor);
			#else
				specular = texture2D(specularMap, vertexUV);
			#endif
		#elif defined NORMAL_MAP
			specular.a = texture2D(normalMap, vertexUV).a; // ???
		#endif // SPECULAR_MAP

	#endif

	vec3 ambientAccum = vec3(0.0);
	vec3 diffuseAccum = vec3(0.0);
	vec3 specularAccum = vec3(0.0);

	#ifdef NUM_AMBIENT_LIGHTS
		for (int i = 0; i < NUM_AMBIENT_LIGHTS; ++i)
			ambientAccum += ambientLights[i].color * ambientLights[i].ambient;
	#endif // NUM_AMBIENT_LIGHTS

	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS || defined ENVIRONMENT_MAP_2D || defined ENVIRONMENT_CUBE_MAP

		vec3	eyeVector		= normalize(cameraPosition - vertexPosition); // always in world-space
		vec3	normalVector	= normalize(vertexNormal); // always in world-space

	#endif // NUM_DIRECTIONAL_LIGHTS || NUM_POINT_LIGHTS || NUM_SPOT_LIGHTS || ENVIRONMENT_MAP_2D || ENVIRONMENT_CUBE_MAP

	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS

		vec3 lightColor = vec3(0.0);
		vec3 lightDirection = vec3(0.0);
		vec3 lightSpotDirection = vec3(0.0);
		vec3 lightPosition = vec3(0.0);
		float lightDiffuseCoeff = 1.0;
		float lightSpecularCoeff = 1.0;
		vec3 lightAttenuationCoeffs = vec3(1.0, 0.0, 0.0);
		float lightCosInnerAng = 0.0;
		float lightCosOuterAng = 0.0;

		#ifdef NORMAL_MAP
			// warning: the normal vector must be normalized at this point!
			mat3 tangentToWorldMatrix = phong_getTangentToWorldSpaceMatrix(normalVector, vertexTangent);

			// bring normal from tangent-space normal to world-space
			#ifdef NORMAL_MAP_LOD
				vec4 normalColor = texturelod_texture2D(normalMap, vertexUV, normalMapSize, 0, normalMapMaxAvailableLod, vec4(0.0));
			#else
				vec4 normalColor = texture2D(normalMap, vertexUV);
			#endif

			normalVector = tangentToWorldMatrix * normalize(2.0 * normalColor.xyz - 1.0);
		#endif // NORMAL_MAP

		#ifdef NUM_DIRECTIONAL_LIGHTS
		//---------------------------
		for (int i = 0; i < NUM_DIRECTIONAL_LIGHTS; ++i)
		{
			lightColor = directionalLights[i].color;
			lightDiffuseCoeff = directionalLights[i].diffuse;
			lightSpecularCoeff = directionalLights[i].specular;
			lightDirection = directionalLights[i].direction;

			lightDirection = normalize(-lightDirection);

			diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
				* lightColor
				* lightDiffuseCoeff;

			#if defined(SHININESS)
				specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
					* phong_fresnel(specular.rgb, lightDirection, eyeVector)
					* lightColor
					* lightSpecularCoeff;
			#endif // SHININESS
		}
		#endif // NUM_DIRECTIONAL_LIGHTS

		#ifdef NUM_POINT_LIGHTS
		//---------------------
		for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
		{
			lightColor = pointLights[i].color;
			lightDiffuseCoeff = pointLights[i].diffuse;
			lightSpecularCoeff = pointLights[i].specular;
			lightAttenuationCoeffs = pointLights[i].attenuationCoeffs;
			lightPosition = pointLights[i].position;

			lightDirection = lightPosition - vertexPosition;
			float distanceToLight = length(lightDirection);
			lightDirection /= distanceToLight;

			vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
			float attenuation = any(lessThan(lightAttenuationCoeffs, vec3(0.0)))
				? 1.0
				: max(0.0, 1.0 - distanceToLight / dot(lightAttenuationCoeffs, distVec));

			diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
				* lightColor
				* (lightDiffuseCoeff * attenuation);

			#if defined(SHININESS)
				specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
					* phong_fresnel(specular.rgb, lightDirection, eyeVector)
					* lightColor
					* (lightSpecularCoeff * attenuation);
			#endif // SHININESS
		}
		#endif // NUM_POINT_LIGHTS

		#ifdef NUM_SPOT_LIGHTS
			//--------------------
			for (int i = 0; i < NUM_SPOT_LIGHTS; ++i)
			{
				SpotLight light = spotLights[i];

				lightDirection = light.position - vertexPosition;
				float distanceToLight = length(lightDirection);
				lightDirection /= distanceToLight;

				lightSpotDirection = light.direction;
				lightSpotDirection = normalize(lightSpotDirection);
				float cosSpot = dot(-lightDirection, lightSpotDirection);

				//gl_FragColor = vec4(abs(light.direction), 1.);
				//return ;

				if (light.cosOuterConeAngle < cosSpot)
				{
					vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
					float attenuation = any(lessThan(light.attenuationCoeffs, vec3(0.0)))
						? 1.0
						: max(0.0, 1.0 - distanceToLight / dot(light.attenuationCoeffs, distVec));

					float cutoff = cosSpot < light.cosInnerConeAngle && light.cosOuterConeAngle < light.cosInnerConeAngle
						? (cosSpot - light.cosOuterConeAngle) / (light.cosInnerConeAngle - light.cosOuterConeAngle)
						: 1.0;

					diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
						* light.color
						* light.diffuse * attenuation * cutoff;

					#ifdef SHININESS
						specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
							* phong_fresnel(specular.rgb, lightDirection, eyeVector)
							* light.color
							* (light.specular * attenuation * cutoff);
					#endif // SHININESS
				}
			}
		#endif // NUM_SPOT_LIGHTS

	#endif // defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS

	#if defined(ENVIRONMENT_MAP_2D) || defined(ENVIRONMENT_CUBE_MAP)
		vec4 envmapColor = envmap_sampleEnvironmentMap(eyeVector, normalVector);
		float reflectivity = specular.a;

		#ifdef ENVIRONMENT_ALPHA
			reflectivity	= environmentAlpha;
		#endif // ENVIRONMENT_ALPHA

		diffuse.rgb = mix(diffuse.rgb, envmapColor.rgb, reflectivity);
	#endif // defined(ENVIRONMENT_MAP_2D) || defined(ENVIRONMENT_CUBE_MAP)


	// Final blend of ambient, diffuse, and specular parts
	//----------------------------------------------------
	vec3 phong		= diffuse.rgb * (ambientAccum + diffuseAccum) + specular.a * specularAccum;

	gl_FragColor	= vec4(phong.rgb, diffuse.a);

	// Applying fog if necessary
	//----------------------------------------------------
	gl_FragColor 	= fog_sampleFog(gl_FragColor, gl_FragCoord);
}

#endif // FRAGMENT_SHADER
