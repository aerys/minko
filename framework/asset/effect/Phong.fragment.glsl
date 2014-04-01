#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("Phong.function.glsl")
#pragma include("Envmap.function.glsl")
#pragma include("Fog.function.glsl")

#ifdef PRECOMPUTED_AMBIENT
	uniform vec3 sumAmbients;
#endif // PRECOMPUTED_AMBIENT

#ifndef MINKO_NO_GLSL_STRUCT

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

#else

	#ifdef NUM_AMBIENT_LIGHTS
		uniform vec3	ambientLights_color[NUM_AMBIENT_LIGHTS];
		uniform float	ambientLights_ambient[NUM_AMBIENT_LIGHTS];
	#endif // NUM_AMBIENT_LIGHTS

	#ifdef NUM_DIRECTIONAL_LIGHTS
		uniform vec3	directionalLights_color[NUM_DIRECTIONAL_LIGHTS];
		uniform float	directionalLights_diffuse[NUM_DIRECTIONAL_LIGHTS];
		uniform float	directionalLights_specular[NUM_DIRECTIONAL_LIGHTS];
		uniform vec3	directionalLights_direction[NUM_DIRECTIONAL_LIGHTS];
	#endif // NUM_DIRECTIONAL_LIGHTS

	#ifdef NUM_POINT_LIGHTS
		uniform vec3	pointLights_color[NUM_POINT_LIGHTS];
		uniform float	pointLights_diffuse[NUM_POINT_LIGHTS];
		uniform float	pointLights_specular[NUM_POINT_LIGHTS];
		uniform vec3	pointLights_attenuationCoeffs[NUM_POINT_LIGHTS];
		uniform vec3	pointLights_position[NUM_POINT_LIGHTS];
	#endif // NUM_POINT_LIGHTS

	#ifdef NUM_SPOT_LIGHTS
		uniform vec3	spotLights_color[NUM_SPOT_LIGHTS];
		uniform float	spotLights_diffuse[NUM_SPOT_LIGHTS];
		uniform float	spotLights_specular[NUM_SPOT_LIGHTS];
		uniform vec3	spotLights_attenuationCoeffs[NUM_SPOT_LIGHTS];
		uniform vec3	spotLights_position[NUM_SPOT_LIGHTS];
		uniform vec3	spotLights_direction[NUM_SPOT_LIGHTS];
		uniform float	spotLights_cosInnerConeAngle[NUM_SPOT_LIGHTS];
		uniform float	spotLights_cosOuterConeAngle[NUM_SPOT_LIGHTS];
	#endif // NUM_SPOT_LIGHTS
	
#endif // MINKO_NO_GLSL_STRUCT
	
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

varying vec3 		vertexPosition;
varying vec2 		vertexUV;
varying vec3 		vertexNormal;
varying vec3 		vertexTangent;

void main(void)
{
	vec4 	diffuse 		= diffuseColor;
	vec4 	specular 		= specularColor;
	float	shininessCoeff 	= 1.0;

	#ifdef SHININESS
		shininessCoeff = max(1.0, shininess);
	#endif // SHININESS

	#ifdef DIFFUSE_MAP
		diffuse 	*= texture2D(diffuseMap, vertexUV);
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
			specular = texture2D(specularMap, vertexUV);
		#elif defined NORMAL_MAP
			specular.a = texture2D(normalMap, vertexUV).a; // ???
		#endif // SPECULAR_MAP

	#endif
	
	vec3	ambientAccum	= vec3(0.0);
	vec3	diffuseAccum	= vec3(0.0);
	vec3	specularAccum	= vec3(0.0); 

	#ifdef PRECOMPUTED_AMBIENT
	//------------------------
		ambientAccum += sumAmbients;
	#else
	
		#ifdef NUM_AMBIENT_LIGHTS
			for (int i = 0; i < NUM_AMBIENT_LIGHTS; ++i)
			{
				#ifndef MINKO_NO_GLSL_STRUCT
					ambientAccum 	+= ambientLights[i].color * ambientLights[i].ambient;
				#else
					ambientAccum	+= ambientLights_color[i] * ambientLights_ambient[i];
				#endif // MINKO_NO_GLSL_STRUCT
			}
		#endif // NUM_AMBIENT_LIGHTS

	#endif // PRECOMPUTED_AMBIENT
	

	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS || defined ENVIRONMENT_MAP_2D || defined ENVIRONMENT_CUBE_MAP

	vec3	eyeVector		= normalize(cameraPosition - vertexPosition); // always in world-space
	vec3	normalVector	= normalize(vertexNormal); // always in world-space

	#endif // NUM_DIRECTIONAL_LIGHTS || NUM_POINT_LIGHTS || NUM_SPOT_LIGHTS || ENVIRONMENT_MAP_2D || ENVIRONMENT_CUBE_MAP

	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS
		
		vec3	lightColor				= vec3(0.0);
		vec3 	lightDirection			= vec3(0.0);
		vec3	lightSpotDirection		= vec3(0.0);
		vec3 	lightPosition			= vec3(0.0);
		float	lightDiffuseCoeff		= 1.0;
		float	lightSpecularCoeff		= 1.0;
		vec3	lightAttenuationCoeffs	= vec3(1.0, 0.0, 0.0);
		float	lightCosInnerAng		= 0.0;
		float	lightCosOuterAng		= 0.0;
		float 	contribution			= 0.0;
				
		#ifdef NORMAL_MAP
			// warning: the normal vector must be normalized at this point!
			mat3 tangentToWorldMatrix 	= phong_getTangentToWorldSpaceMatrix(normalVector, vertexTangent);
			
			normalVector				= tangentToWorldMatrix * normalize(2.0*texture2D(normalMap, vertexUV).xyz - 1.0); // bring normal from tangent-space normal to world-space
		#endif // NORMAL_MAP
				
		#ifdef NUM_DIRECTIONAL_LIGHTS
		//---------------------------
		for (int i = 0; i < NUM_DIRECTIONAL_LIGHTS; ++i)
		{
			#ifndef MINKO_NO_GLSL_STRUCT
				lightColor			= directionalLights[i].color;
				lightDiffuseCoeff	= directionalLights[i].diffuse;
				lightSpecularCoeff	= directionalLights[i].specular;
				lightDirection		= directionalLights[i].direction;
			#else
				lightColor			= directionalLights_color[i];
				lightDiffuseCoeff	= directionalLights_diffuse[i];
				lightSpecularCoeff	= directionalLights_specular[i];
				lightDirection		= directionalLights_direction[i];
			#endif // MINKO_NO_GLSL_STRUCT
	
			lightDirection	= normalize(-lightDirection);
			
			diffuseAccum		+= phong_diffuseReflection(normalVector, lightDirection)
				* lightColor
				* lightDiffuseCoeff;

			#if defined(SHININESS)
				specularAccum	+= 
					phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff) 
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
			#ifndef MINKO_NO_GLSL_STRUCT
				lightColor				= pointLights[i].color;
				lightDiffuseCoeff		= pointLights[i].diffuse;
				lightSpecularCoeff		= pointLights[i].specular;
				lightAttenuationCoeffs	= pointLights[i].attenuationCoeffs;
				lightPosition			= pointLights[i].position;
			#else
				lightColor				= pointLights_color[i];
				lightDiffuseCoeff		= pointLights_diffuse[i];
				lightSpecularCoeff		= pointLights_specular[i];
				lightAttenuationCoeffs	= pointLights_attenuationCoeffs[i];
				lightPosition			= pointLights_position[i];
			#endif // MINKO_NO_GLSL_STRUCT
		
			lightDirection			= lightPosition - vertexPosition;
			float distanceToLight 	= length(lightDirection);
			lightDirection 			/= distanceToLight;
			
			vec3	distVec 	= vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
			float 	attenuation = any(lessThan(lightAttenuationCoeffs, vec3(0.0)))
				? 1.0
				: max(0.0, 1.0 - distanceToLight / dot(lightAttenuationCoeffs, distVec)); 

			diffuseAccum		+= phong_diffuseReflection(normalVector, lightDirection)
				* lightColor
				* (lightDiffuseCoeff * attenuation);

			#if defined(SHININESS)
				specularAccum	+= 
					phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff) 
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
			#ifndef MINKO_NO_GLSL_STRUCT
				lightColor				= spotLights[i].color;
				lightDiffuseCoeff		= spotLights[i].diffuse;
				lightSpecularCoeff		= spotLights[i].specular;
				lightAttenuationCoeffs	= spotLights[i].attenuationCoeffs;
				lightPosition			= spotLights[i].position;
				lightSpotDirection		= spotLights[i].direction;
				lightCosInnerAng		= spotLights[i].cosInnerConeAngle;
				lightCosOuterAng		= spotLights[i].cosOuterConeAngle;
			#else
				lightColor				= spotLights_color[i];
				lightDiffuseCoeff		= spotLights_diffuse[i];
				lightSpecularCoeff		= spotLights_specular[i];
				lightAttenuationCoeffs	= spotLights_attenuationCoeffs[i];
				lightPosition			= spotLights_position[i];
				lightSpotDirection		= spotLights_direction[i];
				lightCosInnerAng		= spotLights_cosInnerConeAngle[i];
				lightCosOuterAng		= spotLights_cosOuterConeAngle[i];
			#endif // MINKO_NO_GLSL_STRUCT
			
			
			lightDirection			= lightPosition - vertexPosition;
			float distanceToLight	= length(lightDirection);
			lightDirection			/= distanceToLight;
			
			lightSpotDirection	= normalize(-lightSpotDirection);						
			float cosSpot		= dot(-lightDirection, lightSpotDirection);

			if (lightCosOuterAng < cosSpot)
			{
				vec3	distVec 	= vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
				float 	attenuation = any(lessThan(lightAttenuationCoeffs, vec3(0.0)))
					? 1.0
					: max(0.0, 1.0 - distanceToLight / dot(lightAttenuationCoeffs, distVec)); 
					
				float cutoff	= cosSpot < lightCosInnerAng && lightCosOuterAng < lightCosInnerAng 
					? (cosSpot - lightCosOuterAng) / (lightCosInnerAng - lightCosOuterAng) 
					: 1.0;	


				diffuseAccum		+= phong_diffuseReflection(normalVector, lightDirection)
					* lightColor
					* (lightDiffuseCoeff * attenuation * cutoff);

				#ifdef SHININESS
					specularAccum	+= 
						phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff) 
						* phong_fresnel(specular.rgb, lightDirection, eyeVector)
						* lightColor
						* (lightSpecularCoeff * attenuation * cutoff);
				#endif // SHININESS
			}
		}
		#endif // NUM_SPOT_LIGHTS
		
	#endif // defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS

	#if defined(ENVIRONMENT_MAP_2D) || defined(ENVIRONMENT_CUBE_MAP)

		vec4	envmapColor		= envmap_sampleEnvironmentMap(eyeVector, normalVector);
		float	reflectivity	= specular.a;

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