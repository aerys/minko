#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("Phong.function.glsl")
#pragma include("Envmap.function.glsl")
#pragma include("Fog.function.glsl")

// #pragma include("Phong.struct.glsl")

uniform vec3  ambient0_color;
uniform float ambient0_ambient;

uniform vec3  ambient1_color;
uniform float ambient1_ambient;

uniform vec3  ambient2_color;
uniform float ambient2_ambient;

uniform vec3  ambient3_color;
uniform float ambient3_ambient;

uniform vec3  ambient4_color;
uniform float ambient4_ambient;

uniform vec3  ambient5_color;
uniform float ambient5_ambient;

uniform vec3  ambient6_color;
uniform float ambient6_ambient;

uniform vec3  ambient7_color;
uniform float ambient7_ambient;

uniform vec3  directional0_color;
uniform float directional0_diffuse;
uniform float directional0_specular;
uniform vec3  directional0_direction;

uniform vec3  directional1_color;
uniform float directional1_diffuse;
uniform float directional1_specular;
uniform vec3  directional1_direction;

uniform vec3  directional2_color;
uniform float directional2_diffuse;
uniform float directional2_specular;
uniform vec3  directional2_direction;

uniform vec3  directional3_color;
uniform float directional3_diffuse;
uniform float directional3_specular;
uniform vec3  directional3_direction;

uniform vec3  directional4_color;
uniform float directional4_diffuse;
uniform float directional4_specular;
uniform vec3  directional4_direction;

uniform vec3  directional5_color;
uniform float directional5_diffuse;
uniform float directional5_specular;
uniform vec3  directional5_direction;

uniform vec3  directional6_color;
uniform float directional6_diffuse;
uniform float directional6_specular;
uniform vec3  directional6_direction;

uniform vec3  directional7_color;
uniform float directional7_diffuse;
uniform float directional7_specular;
uniform vec3  directional7_direction;

uniform vec3  point0_color;
uniform float point0_diffuse;
uniform float point0_specular;
uniform vec3  point0_attenuationCoeffs;
uniform vec3  point0_position;

uniform vec3  point1_color;
uniform float point1_diffuse;
uniform float point1_specular;
uniform vec3  point1_attenuationCoeffs;
uniform vec3  point1_position;

uniform vec3  point2_color;
uniform float point2_diffuse;
uniform float point2_specular;
uniform vec3  point2_attenuationCoeffs;
uniform vec3  point2_position;

uniform vec3  point3_color;
uniform float point3_diffuse;
uniform float point3_specular;
uniform vec3  point3_attenuationCoeffs;
uniform vec3  point3_position;

uniform vec3  point4_color;
uniform float point4_diffuse;
uniform float point4_specular;
uniform vec3  point4_attenuationCoeffs;
uniform vec3  point4_position;

uniform vec3  point5_color;
uniform float point5_diffuse;
uniform float point5_specular;
uniform vec3  point5_attenuationCoeffs;
uniform vec3  point5_position;

uniform vec3  point6_color;
uniform float point6_diffuse;
uniform float point6_specular;
uniform vec3  point6_attenuationCoeffs;
uniform vec3  point6_position;

uniform vec3  point7_color;
uniform float point7_diffuse;
uniform float point7_specular;
uniform vec3  point7_attenuationCoeffs;
uniform vec3  point7_position;

uniform vec3  spot0_direction;
uniform float spot0_diffuse;
uniform vec3  spot0_position;
uniform float spot0_cosInnerConeAngle;
uniform vec3  spot0_color;
uniform float spot0_cosOuterConeAngle;
uniform vec3  spot0_attenuationCoeffs;
uniform float spot0_specular;

uniform vec3  spot1_direction;
uniform float spot1_diffuse;
uniform vec3  spot1_position;
uniform float spot1_cosInnerConeAngle;
uniform vec3  spot1_color;
uniform float spot1_cosOuterConeAngle;
uniform vec3  spot1_attenuationCoeffs;
uniform float spot1_specular;

uniform vec3  spot2_direction;
uniform float spot2_diffuse;
uniform vec3  spot2_position;
uniform float spot2_cosInnerConeAngle;
uniform vec3  spot2_color;
uniform float spot2_cosOuterConeAngle;
uniform vec3  spot2_attenuationCoeffs;
uniform float spot2_specular;

uniform vec3  spot3_direction;
uniform float spot3_diffuse;
uniform vec3  spot3_position;
uniform float spot3_cosInnerConeAngle;
uniform vec3  spot3_color;
uniform float spot3_cosOuterConeAngle;
uniform vec3  spot3_attenuationCoeffs;
uniform float spot3_specular;

uniform vec3  spot4_direction;
uniform float spot4_diffuse;
uniform vec3  spot4_position;
uniform float spot4_cosInnerConeAngle;
uniform vec3  spot4_color;
uniform float spot4_cosOuterConeAngle;
uniform vec3  spot4_attenuationCoeffs;
uniform float spot4_specular;

uniform vec3  spot5_direction;
uniform float spot5_diffuse;
uniform vec3  spot5_position;
uniform float spot5_cosInnerConeAngle;
uniform vec3  spot5_color;
uniform float spot5_cosOuterConeAngle;
uniform vec3  spot5_attenuationCoeffs;
uniform float spot5_specular;

uniform vec3  spot6_direction;
uniform float spot6_diffuse;
uniform vec3  spot6_position;
uniform float spot6_cosInnerConeAngle;
uniform vec3  spot6_color;
uniform float spot6_cosOuterConeAngle;
uniform vec3  spot6_attenuationCoeffs;
uniform float spot6_specular;

uniform vec3  spot7_direction;
uniform float spot7_diffuse;
uniform vec3  spot7_position;
uniform float spot7_cosInnerConeAngle;
uniform vec3  spot7_color;
uniform float spot7_cosOuterConeAngle;
uniform vec3  spot7_attenuationCoeffs;
uniform float spot7_specular;

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
	vec4 diffuse = diffuseColor;
	vec4 specular = specularColor;
	float shininessCoeff = 1.0;

	#if defined(SHININESS)
		shininessCoeff = max(1.0, shininess);
	#endif // SHININESS

	#if defined(DIFFUSE_MAP)
		diffuse 	= texture2D(diffuseMap, vertexUV);
	#endif // DIFFUSE_MAP

	#if defined(ALPHA_MAP)
		diffuse.a 	= texture2D(alphaMap, vertexUV).r;
	#endif // ALPHA_MAP

	#if defined(ALPHA_THRESHOLD)
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

	vec3 ambientAccum = vec3(0.0);
	vec3 diffuseAccum = vec3(0.0);
	vec3 specularAccum = vec3(0.0);

	#if defined(NUM_AMBIENT_LIGHTS)
		#if NUM_AMBIENT_LIGHTS > 0
			ambientAccum += ambient0_color * ambient0_ambient;
		#endif

		#if NUM_AMBIENT_LIGHTS > 1
			ambientAccum += ambient1_color * ambient1_ambient;
		#endif

		#if NUM_AMBIENT_LIGHTS > 2
			ambientAccum += ambient2_color * ambient2_ambient;
		#endif

		#if NUM_AMBIENT_LIGHTS > 3
			ambientAccum += ambient3_color * ambient3_ambient;
		#endif

		#if NUM_AMBIENT_LIGHTS > 4
			ambientAccum += ambient4_color * ambient4_ambient;
		#endif

		#if NUM_AMBIENT_LIGHTS > 5
			ambientAccum += ambient5_color * ambient5_ambient;
		#endif

		#if NUM_AMBIENT_LIGHTS > 6
			ambientAccum += ambient6_color * ambient6_ambient;
		#endif

		#if NUM_AMBIENT_LIGHTS > 7
			ambientAccum += ambient7_color * ambient7_ambient;
		#endif
	#endif

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

		#if defined(NORMAL_MAP)
			// warning: the normal vector must be normalized at this point!
			mat3 tangentToWorldMatrix = phong_getTangentToWorldSpaceMatrix(normalVector, vertexTangent);

			// bring normal from tangent-space normal to world-space
			normalVector = tangentToWorldMatrix * normalize(2.0 * texture2D(normalMap, vertexUV).xyz - 1.0);
		#endif // NORMAL_MAP

		#if defined(NUM_DIRECTIONAL_LIGHTS)
			#if NUM_DIRECTIONAL_LIGHTS > 0
				//---------------------------
				lightColor = directional0_color;
				lightDiffuseCoeff = directional0_diffuse;
				lightSpecularCoeff = directional0_specular;
				lightDirection = directional0_direction;

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
			#endif

			#if NUM_DIRECTIONAL_LIGHTS > 1
				//---------------------------
				lightColor = directional1_color;
				lightDiffuseCoeff = directional1_diffuse;
				lightSpecularCoeff = directional1_specular;
				lightDirection = directional1_direction;

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
			#endif

			#if NUM_DIRECTIONAL_LIGHTS > 2
				//---------------------------
				lightColor = directional2_color;
				lightDiffuseCoeff = directional2_diffuse;
				lightSpecularCoeff = directional2_specular;
				lightDirection = directional2_direction;

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
			#endif

			#if NUM_DIRECTIONAL_LIGHTS > 3
				//---------------------------
				lightColor = directional3_color;
				lightDiffuseCoeff = directional3_diffuse;
				lightSpecularCoeff = directional3_specular;
				lightDirection = directional3_direction;

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
			#endif

			#if NUM_DIRECTIONAL_LIGHTS > 4
				//---------------------------
				lightColor = directional4_color;
				lightDiffuseCoeff = directional4_diffuse;
				lightSpecularCoeff = directional4_specular;
				lightDirection = directional4_direction;

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
			#endif

			#if NUM_DIRECTIONAL_LIGHTS > 4
				//---------------------------
				lightColor = directional4_color;
				lightDiffuseCoeff = directional4_diffuse;
				lightSpecularCoeff = directional4_specular;
				lightDirection = directional4_direction;

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
			#endif

			#if NUM_DIRECTIONAL_LIGHTS > 5
				//---------------------------
				lightColor = directional5_color;
				lightDiffuseCoeff = directional5_diffuse;
				lightSpecularCoeff = directional5_specular;
				lightDirection = directional5_direction;

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
			#endif

			#if NUM_DIRECTIONAL_LIGHTS > 6
				//---------------------------
				lightColor = directional6_color;
				lightDiffuseCoeff = directional6_diffuse;
				lightSpecularCoeff = directional6_specular;
				lightDirection = directional6_direction;

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
			#endif

			#if NUM_DIRECTIONAL_LIGHTS > 7
				//---------------------------
				lightColor = directional7_color;
				lightDiffuseCoeff = directional7_diffuse;
				lightSpecularCoeff = directional7_specular;
				lightDirection = directional7_direction;

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
			#endif
		#endif

		#if defined(NUM_POINT_LIGHTS)
			#if NUM_POINT_LIGHTS > 0
				//---------------------
				lightColor = point0_color;
				lightDiffuseCoeff = point0_diffuse;
				lightSpecularCoeff = point0_specular;
				lightAttenuationCoeffs = point0_attenuationCoeffs;
				lightPosition = point0_position;

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
			#endif

			#if NUM_POINT_LIGHTS > 1
				//---------------------
				lightColor = point1_color;
				lightDiffuseCoeff = point1_diffuse;
				lightSpecularCoeff = point1_specular;
				lightAttenuationCoeffs = point1_attenuationCoeffs;
				lightPosition = point1_position;

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
			#endif

			#if NUM_POINT_LIGHTS > 2
				//---------------------
				lightColor = point2_color;
				lightDiffuseCoeff = point2_diffuse;
				lightSpecularCoeff = point2_specular;
				lightAttenuationCoeffs = point2_attenuationCoeffs;
				lightPosition = point2_position;

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
			#endif

			#if NUM_POINT_LIGHTS > 3
				//---------------------
				lightColor = point3_color;
				lightDiffuseCoeff = point3_diffuse;
				lightSpecularCoeff = point3_specular;
				lightAttenuationCoeffs = point3_attenuationCoeffs;
				lightPosition = point3_position;

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
			#endif

			#if NUM_POINT_LIGHTS > 4
				//---------------------
				lightColor = point4_color;
				lightDiffuseCoeff = point4_diffuse;
				lightSpecularCoeff = point4_specular;
				lightAttenuationCoeffs = point4_attenuationCoeffs;
				lightPosition = point4_position;

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
			#endif

			#if NUM_POINT_LIGHTS > 5
				//---------------------
				lightColor = point5_color;
				lightDiffuseCoeff = point5_diffuse;
				lightSpecularCoeff = point5_specular;
				lightAttenuationCoeffs = point5_attenuationCoeffs;
				lightPosition = point5_position;

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
			#endif

			#if NUM_POINT_LIGHTS > 6
				//---------------------
				lightColor = point6_color;
				lightDiffuseCoeff = point6_diffuse;
				lightSpecularCoeff = point6_specular;
				lightAttenuationCoeffs = point6_attenuationCoeffs;
				lightPosition = point6_position;

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
			#endif

			#if NUM_POINT_LIGHTS > 7
				//---------------------
				lightColor = point7_color;
				lightDiffuseCoeff = point7_diffuse;
				lightSpecularCoeff = point7_specular;
				lightAttenuationCoeffs = point7_attenuationCoeffs;
				lightPosition = point7_position;

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
			#endif
		#endif

		#if defined(NUM_SPOT_LIGHTS)
			#if NUM_SPOT_LIGHTS > 0
				//--------------------
				lightDirection = spot0_direction;
				lightDiffuseCoeff = spot0_diffuse;
				lightPosition = spot0_position;
				lightCosInnerConeAngle = spot0_cosInnerConeAngle;
				lightColor = spot0_color;
				lightCosOuterConeAngle = spot0_cosOuterConeAngle;
				lightAttenuationCoeffs = spot0_attenuationCoeffs;
				lightSpecular = spot0_specular;

				lightDirection = lightPosition - vertexPosition;
				float distanceToLight = length(lightDirection);
				lightDirection /= distanceToLight;

				lightSpotDirection = lightDirection;
				lightSpotDirection = normalize(lightSpotDirection);
				float cosSpot = dot(-lightDirection, lightSpotDirection);

				if (lightCosOuterConeAngle < cosSpot)
				{
					vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
					float attenuation = any(lessThan(lightAttenuationCoeffs, vec3(0.0)))
						? 1.0
						: max(0.0, 1.0 - distanceToLight / dot(lightAttenuationCoeffs, distVec));

					float cutoff = cosSpot < lightCosInnerConeAngle && lightCosOuterConeAngle < lightCosInnerConeAngle
						? (cosSpot - lightCosOuterConeAngle) / (lightCosInnerConeAngle - lightCosOuterConeAngle)
						: 1.0;

					diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
						* lightColor
						* lightDiffuse * attenuation * cutoff;

					#if defined(SHININESS)
						specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
							* phong_fresnel(specular.rgb, lightDirection, eyeVector)
							* lightColor
							* (lightSpecular * attenuation * cutoff);
					#endif // SHININESS
				}
			#endif

			#if NUM_SPOT_LIGHTS > 1
				//--------------------
				lightDirection = spot1_direction;
				lightDiffuseCoeff = spot1_diffuse;
				lightPosition = spot1_position;
				lightCosInnerConeAngle = spot1_cosInnerConeAngle;
				lightColor = spot1_color;
				lightCosOuterConeAngle = spot1_cosOuterConeAngle;
				lightAttenuationCoeffs = spot1_attenuationCoeffs;
				lightSpecular = spot1_specular;

				lightDirection = lightPosition - vertexPosition;
				float distanceToLight = length(lightDirection);
				lightDirection /= distanceToLight;

				lightSpotDirection = lightDirection;
				lightSpotDirection = normalize(lightSpotDirection);
				float cosSpot = dot(-lightDirection, lightSpotDirection);

				if (lightCosOuterConeAngle < cosSpot)
				{
					vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
					float attenuation = any(lessThan(lightAttenuationCoeffs, vec3(0.0)))
						? 1.0
						: max(0.0, 1.0 - distanceToLight / dot(lightAttenuationCoeffs, distVec));

					float cutoff = cosSpot < lightCosInnerConeAngle && lightCosOuterConeAngle < lightCosInnerConeAngle
						? (cosSpot - lightCosOuterConeAngle) / (lightCosInnerConeAngle - lightCosOuterConeAngle)
						: 1.0;

					diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
						* lightColor
						* lightDiffuse * attenuation * cutoff;

					#if defined(SHININESS)
						specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
							* phong_fresnel(specular.rgb, lightDirection, eyeVector)
							* lightColor
							* (lightSpecular * attenuation * cutoff);
					#endif // SHININESS
				}
			#endif

			#if NUM_SPOT_LIGHTS > 2
				//--------------------
				lightDirection = spot2_direction;
				lightDiffuseCoeff = spot2_diffuse;
				lightPosition = spot2_position;
				lightCosInnerConeAngle = spot2_cosInnerConeAngle;
				lightColor = spot2_color;
				lightCosOuterConeAngle = spot2_cosOuterConeAngle;
				lightAttenuationCoeffs = spot2_attenuationCoeffs;
				lightSpecular = spot2_specular;

				lightDirection = lightPosition - vertexPosition;
				float distanceToLight = length(lightDirection);
				lightDirection /= distanceToLight;

				lightSpotDirection = lightDirection;
				lightSpotDirection = normalize(lightSpotDirection);
				float cosSpot = dot(-lightDirection, lightSpotDirection);

				if (lightCosOuterConeAngle < cosSpot)
				{
					vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
					float attenuation = any(lessThan(lightAttenuationCoeffs, vec3(0.0)))
						? 1.0
						: max(0.0, 1.0 - distanceToLight / dot(lightAttenuationCoeffs, distVec));

					float cutoff = cosSpot < lightCosInnerConeAngle && lightCosOuterConeAngle < lightCosInnerConeAngle
						? (cosSpot - lightCosOuterConeAngle) / (lightCosInnerConeAngle - lightCosOuterConeAngle)
						: 1.0;

					diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
						* lightColor
						* lightDiffuse * attenuation * cutoff;

					#if defined(SHININESS)
						specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
							* phong_fresnel(specular.rgb, lightDirection, eyeVector)
							* lightColor
							* (lightSpecular * attenuation * cutoff);
					#endif // SHININESS
				}
			#endif

			#if NUM_SPOT_LIGHTS > 3
				//--------------------
				lightDirection = spot3_direction;
				lightDiffuseCoeff = spot3_diffuse;
				lightPosition = spot3_position;
				lightCosInnerConeAngle = spot3_cosInnerConeAngle;
				lightColor = spot3_color;
				lightCosOuterConeAngle = spot3_cosOuterConeAngle;
				lightAttenuationCoeffs = spot3_attenuationCoeffs;
				lightSpecular = spot3_specular;

				lightDirection = lightPosition - vertexPosition;
				float distanceToLight = length(lightDirection);
				lightDirection /= distanceToLight;

				lightSpotDirection = lightDirection;
				lightSpotDirection = normalize(lightSpotDirection);
				float cosSpot = dot(-lightDirection, lightSpotDirection);

				if (lightCosOuterConeAngle < cosSpot)
				{
					vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
					float attenuation = any(lessThan(lightAttenuationCoeffs, vec3(0.0)))
						? 1.0
						: max(0.0, 1.0 - distanceToLight / dot(lightAttenuationCoeffs, distVec));

					float cutoff = cosSpot < lightCosInnerConeAngle && lightCosOuterConeAngle < lightCosInnerConeAngle
						? (cosSpot - lightCosOuterConeAngle) / (lightCosInnerConeAngle - lightCosOuterConeAngle)
						: 1.0;

					diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
						* lightColor
						* lightDiffuse * attenuation * cutoff;

					#if defined(SHININESS)
						specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
							* phong_fresnel(specular.rgb, lightDirection, eyeVector)
							* lightColor
							* (lightSpecular * attenuation * cutoff);
					#endif // SHININESS
				}
			#endif

			#if NUM_SPOT_LIGHTS > 4
				//--------------------
				lightDirection = spot4_direction;
				lightDiffuseCoeff = spot4_diffuse;
				lightPosition = spot4_position;
				lightCosInnerConeAngle = spot4_cosInnerConeAngle;
				lightColor = spot4_color;
				lightCosOuterConeAngle = spot4_cosOuterConeAngle;
				lightAttenuationCoeffs = spot4_attenuationCoeffs;
				lightSpecular = spot4_specular;

				lightDirection = lightPosition - vertexPosition;
				float distanceToLight = length(lightDirection);
				lightDirection /= distanceToLight;

				lightSpotDirection = lightDirection;
				lightSpotDirection = normalize(lightSpotDirection);
				float cosSpot = dot(-lightDirection, lightSpotDirection);

				if (lightCosOuterConeAngle < cosSpot)
				{
					vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
					float attenuation = any(lessThan(lightAttenuationCoeffs, vec3(0.0)))
						? 1.0
						: max(0.0, 1.0 - distanceToLight / dot(lightAttenuationCoeffs, distVec));

					float cutoff = cosSpot < lightCosInnerConeAngle && lightCosOuterConeAngle < lightCosInnerConeAngle
						? (cosSpot - lightCosOuterConeAngle) / (lightCosInnerConeAngle - lightCosOuterConeAngle)
						: 1.0;

					diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
						* lightColor
						* lightDiffuse * attenuation * cutoff;

					#if defined(SHININESS)
						specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
							* phong_fresnel(specular.rgb, lightDirection, eyeVector)
							* lightColor
							* (lightSpecular * attenuation * cutoff);
					#endif // SHININESS
				}
			#endif

			#if NUM_SPOT_LIGHTS > 5
				//--------------------
				lightDirection = spot5_direction;
				lightDiffuseCoeff = spot5_diffuse;
				lightPosition = spot5_position;
				lightCosInnerConeAngle = spot5_cosInnerConeAngle;
				lightColor = spot5_color;
				lightCosOuterConeAngle = spot5_cosOuterConeAngle;
				lightAttenuationCoeffs = spot5_attenuationCoeffs;
				lightSpecular = spot5_specular;

				lightDirection = lightPosition - vertexPosition;
				float distanceToLight = length(lightDirection);
				lightDirection /= distanceToLight;

				lightSpotDirection = lightDirection;
				lightSpotDirection = normalize(lightSpotDirection);
				float cosSpot = dot(-lightDirection, lightSpotDirection);

				if (lightCosOuterConeAngle < cosSpot)
				{
					vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
					float attenuation = any(lessThan(lightAttenuationCoeffs, vec3(0.0)))
						? 1.0
						: max(0.0, 1.0 - distanceToLight / dot(lightAttenuationCoeffs, distVec));

					float cutoff = cosSpot < lightCosInnerConeAngle && lightCosOuterConeAngle < lightCosInnerConeAngle
						? (cosSpot - lightCosOuterConeAngle) / (lightCosInnerConeAngle - lightCosOuterConeAngle)
						: 1.0;

					diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
						* lightColor
						* lightDiffuse * attenuation * cutoff;

					#if defined(SHININESS)
						specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
							* phong_fresnel(specular.rgb, lightDirection, eyeVector)
							* lightColor
							* (lightSpecular * attenuation * cutoff);
					#endif // SHININESS
				}
			#endif

			#if NUM_SPOT_LIGHTS > 6
				//--------------------
				lightDirection = spot6_direction;
				lightDiffuseCoeff = spot6_diffuse;
				lightPosition = spot6_position;
				lightCosInnerConeAngle = spot6_cosInnerConeAngle;
				lightColor = spot6_color;
				lightCosOuterConeAngle = spot6_cosOuterConeAngle;
				lightAttenuationCoeffs = spot6_attenuationCoeffs;
				lightSpecular = spot6_specular;

				lightDirection = lightPosition - vertexPosition;
				float distanceToLight = length(lightDirection);
				lightDirection /= distanceToLight;

				lightSpotDirection = lightDirection;
				lightSpotDirection = normalize(lightSpotDirection);
				float cosSpot = dot(-lightDirection, lightSpotDirection);

				if (lightCosOuterConeAngle < cosSpot)
				{
					vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
					float attenuation = any(lessThan(lightAttenuationCoeffs, vec3(0.0)))
						? 1.0
						: max(0.0, 1.0 - distanceToLight / dot(lightAttenuationCoeffs, distVec));

					float cutoff = cosSpot < lightCosInnerConeAngle && lightCosOuterConeAngle < lightCosInnerConeAngle
						? (cosSpot - lightCosOuterConeAngle) / (lightCosInnerConeAngle - lightCosOuterConeAngle)
						: 1.0;

					diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
						* lightColor
						* lightDiffuse * attenuation * cutoff;

					#if defined(SHININESS)
						specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
							* phong_fresnel(specular.rgb, lightDirection, eyeVector)
							* lightColor
							* (lightSpecular * attenuation * cutoff);
					#endif // SHININESS
				}
			#endif

			#if NUM_SPOT_LIGHTS > 7
				//--------------------
				lightDirection = spot7_direction;
				lightDiffuseCoeff = spot7_diffuse;
				lightPosition = spot7_position;
				lightCosInnerConeAngle = spot7_cosInnerConeAngle;
				lightColor = spot7_color;
				lightCosOuterConeAngle = spot7_cosOuterConeAngle;
				lightAttenuationCoeffs = spot7_attenuationCoeffs;
				lightSpecular = spot7_specular;

				lightDirection = lightPosition - vertexPosition;
				float distanceToLight = length(lightDirection);
				lightDirection /= distanceToLight;

				lightSpotDirection = lightDirection;
				lightSpotDirection = normalize(lightSpotDirection);
				float cosSpot = dot(-lightDirection, lightSpotDirection);

				if (lightCosOuterConeAngle < cosSpot)
				{
					vec3 distVec = vec3(1.0, distanceToLight, distanceToLight * distanceToLight);
					float attenuation = any(lessThan(lightAttenuationCoeffs, vec3(0.0)))
						? 1.0
						: max(0.0, 1.0 - distanceToLight / dot(lightAttenuationCoeffs, distVec));

					float cutoff = cosSpot < lightCosInnerConeAngle && lightCosOuterConeAngle < lightCosInnerConeAngle
						? (cosSpot - lightCosOuterConeAngle) / (lightCosInnerConeAngle - lightCosOuterConeAngle)
						: 1.0;

					diffuseAccum += phong_diffuseReflection(normalVector, lightDirection)
						* lightColor
						* lightDiffuse * attenuation * cutoff;

					#if defined(SHININESS)
						specularAccum += phong_specularReflection(normalVector, lightDirection, eyeVector, shininessCoeff)
							* phong_fresnel(specular.rgb, lightDirection, eyeVector)
							* lightColor
							* (lightSpecular * attenuation * cutoff);
					#endif // SHININESS
				}
			#endif
		#endif // NUM_SPOT_LIGHTS

	#endif // defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS

	#if defined(ENVIRONMENT_MAP_2D) || defined(ENVIRONMENT_CUBE_MAP)
		vec4 envmapColor = envmap_sampleEnvironmentMap(eyeVector, normalVector);
		float reflectivity = specular.a;

		#if defined(ENVIRONMENT_ALPHA)
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
