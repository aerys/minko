#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("Reflection/Reflection.function.glsl")
#pragma include("Phong.function.glsl")
#pragma include("Envmap.function.glsl")
#pragma include("Water.function.glsl")

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

// phong
uniform vec4 		specularColor;
uniform sampler2D 	normalMap;
uniform sampler2D 	specularMap;
uniform sampler2D 	flowMap;
uniform sampler2D 	noiseMap;
uniform sampler2D	reflectionMap;
uniform sampler2D	dudvMap;
uniform float 		shininess;
uniform vec3 		cameraPosition;
uniform float		normalSpeed;
uniform float		frameId;
uniform float 		normalMapScale;
uniform float 		flowMapScale;
uniform float 		flowMapCycle;
uniform float 		flowMapOffset1;
uniform float 		flowMapOffset2;
uniform float		reflectivity;
uniform float		dudvSpeed;
uniform float		dudvFactor;

varying vec4		vertexScreenPosition;
varying vec3 		vertexPosition;
varying vec2 		vertexUV;
varying vec3 		vertexNormal;
varying vec3 		vertexTangent;

void main(void)
{
	vec4 diffuse 			= diffuseColor;
	vec4 specular 			= specularColor;
	float specularAlpha		= diffuse.a;

	#ifdef DIFFUSE_MAP
		diffuse = texture2D(diffuseMap, vertexUV);
	#endif // DIFFUSE_MAP

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

	vec3 eyeVector	= normalize(cameraPosition - vertexPosition); // always in world-space

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
		
		vec3 	normalVector			= normalize(vertexNormal); // always in world-space
		
		#ifdef NORMAL_MAP
			// warning: the normal vector must be normalized at this point!
			mat3 tangentToWorldMatrix 	= phong_getTangentToWorldSpaceMatrix(normalVector, vertexTangent);

			#ifdef FLOW_MAP
				float noiseOffset 			= 0.f;

				#ifdef NOISE_MAP
				noiseOffset = texture2D(noiseMap, vertexUV).r;
				#endif
				vec2 flowmap 				= (texture2D(flowMap, vertexUV * flowMapScale).rg * 2.0f - 1.f);

				float phase0 				= noiseOffset * 0.5f + flowMapOffset1;
				float phase1 				= noiseOffset * 0.5f + flowMapOffset2;

				vec3 normalSample1			= 2.0f * texture2D(normalMap, vertexUV * normalMapScale + flowmap * phase0).xyz - 1.f;
				vec3 normalSample2			= 2.0f * texture2D(normalMap, vertexUV * normalMapScale + flowmap * phase1).xyz - 1.f;

				float halfCycle				= flowMapCycle / 2.f;
				float f 					= abs(halfCycle - flowMapOffset1) / halfCycle;

				normalVector 				= tangentToWorldMatrix * normalize(mix(normalSample1, normalSample2, f));
			#else
				vec2 uvOffset				= vec2(frameId * normalSpeed, frameId * normalSpeed);
				vec3 normalSample1			= 2.0f * texture2D(normalMap, vertexUV * normalMapScale + uvOffset).xyz - 1.f;
				vec3 normalSample2			= 2.0f * texture2D(normalMap, vertexUV * normalMapScale - uvOffset).xyz - 1.f;

				normalVector				= tangentToWorldMatrix * normalize((normalSample1 + normalSample2) / 2); // bring normal from tangent-space normal to world-space
			#endif
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
					phong_specularReflection(normalVector, lightDirection, eyeVector, shininess) 
					* phong_fresnel(specular.rgb, lightDirection, eyeVector)
					* lightColor
					* lightSpecularCoeff;
				specularAlpha += phong_specularReflection(normalVector, lightDirection, eyeVector, shininess) * lightSpecularCoeff;
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
					phong_specularReflection(normalVector, lightDirection, eyeVector, shininess) 
					* phong_fresnel(specular.rgb, lightDirection, eyeVector)
					* lightColor
					* (lightSpecularCoeff * attenuation);
				specularAlpha += phong_specularReflection(normalVector, lightDirection, eyeVector, shininess) * (lightSpecularCoeff * attenuation);
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
						phong_specularReflection(normalVector, lightDirection, eyeVector, shininess) 
						* phong_fresnel(specular.rgb, lightDirection, eyeVector)
						* lightColor
						* (lightSpecularCoeff * attenuation * cutoff);
					specularAlpha += phong_specularReflection(normalVector, lightDirection, eyeVector, shininess) * (lightSpecularCoeff * attenuation * cutoff);

				#endif // SHININESS
			}
		}
		#endif // NUM_SPOT_LIGHTS
		
	#endif // defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS

	// Final blend of ambient, diffuse, and specular parts
	//----------------------------------------------------
	vec3 phongColor	= diffuse.rgb * (ambientAccum + diffuseAccum);
	
	#ifdef REFLECTION_MAP
		#ifdef DUDV_MAP
			vec2 dudvOffset = ((1.f - vertexUV) + (frameId * dudvSpeed));

			phongColor = mix(phongColor, getDuDvReflectionColor(vertexScreenPosition, vertexUV.xy, reflectionMap, texture2D(dudvMap, dudvOffset).rb * dudvFactor).rgb, reflectivity);
		#else
			phongColor = mix(phongColor, getReflectionColor(vertexScreenPosition, vertexUV, reflectionMap).rgb, reflectivity);
		#endif
	#endif

	vec3 phong		= phongColor + specularAccum;
	gl_FragColor	= vec4(phong.rgb, specularAlpha);
}

#endif // FRAGMENT_SHADER