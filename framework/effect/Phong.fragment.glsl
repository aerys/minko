#ifdef FRAGMENT_SHADER

#ifdef NUM_AMBIENT_LIGHTS
	uniform AmbientLight ambientLights[NUM_AMBIENT_LIGHTS];
#endif // NUM_AMBIENT_LIGHTS

#ifdef PRECOMPUTED_AMBIENT
	uniform vec3 sumAmbients;
#endif // PRECOMPUTED_AMBIENT

#ifdef NUM_DIRECTIONAL_LIGHTS
	uniform DirectionalLight directionalLights[NUM_DIRECTIONAL_LIGHTS];
#endif // NUM_DIRECTIONAL_LIGHTS

#ifdef NUM_POINT_LIGHTS
	uniform PointLight pointLights[NUM_POINT_LIGHTS];
#endif // NUM_POINT_LIGHTS

#ifdef NUM_SPOT_LIGHTS
	uniform SpotLight spotLights[NUM_SPOT_LIGHTS];
#endif // NUM_SPOT_LIGHTS

uniform vec4 diffuseColor;
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D alphaMap;

uniform float shininess;
uniform float alphaThreshold;
uniform vec3 cameraPosition;

varying vec3 vertexPosition;
varying vec2 vertexUV;
varying vec3 vertexNormal;
varying vec3 vertexTangent;

void main(void)
{
	vec4 diffuse = diffuseColor;
	
	#ifdef DIFFUSE_MAP
		diffuse = texture2D(diffuseMap, vertexUV);
	#endif // DIFFUSE_MAP

	#ifdef ALPHA_MAP
		diffuse.a = texture2D(alphaMap, vertexUV).r;
	#endif // ALPHA_MAP

	#ifdef ALPHA_THRESHOLD
		if (diffuse.a < alphaThreshold)
			discard;
	#endif // ALPHA_THRESHOLD
	
	float specularIntensity = 1.0;
	
	#ifdef SPECULAR_MAP
		specularIntensity = texture2D(specularMap, vertexUV).r;
	#elif defined NORMAL_MAP
		specularIntensity = texture2D(normalMap, vertexUV).a;
	#endif // SPECULAR_MAP
	
	vec3 phong = vec3(0.);
	
	#ifdef PRECOMPUTED_AMBIENT
	//------------------------
		phong += sumAmbients;
	#else
	
		#ifdef NUM_AMBIENT_LIGHTS
			for (int i = 0; i < NUM_AMBIENT_LIGHTS; ++i)
			{
				phong += ambientLights[i].color * ambientLights[i].ambient;
			}
		#endif // NUM_AMBIENT_LIGHTS

	#endif // PRECOMPUTED_AMBIENT
	
	
	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS
		
		vec3 lightDirection	= vec3(0.0);
		float contribution	= 0.0;
		
		vec3 normal		= normalize(vertexNormal);
		vec3 eyeVector	= cameraPosition - vertexPosition;

		#ifdef NORMAL_MAP
			// warning: the normal vector must be normalized at this point!
			mat3 worldToTangentMatrix = getWorldToTangentSpaceMatrix(normal, vertexTangent);
			
			normal		= normalize(2.0*texture2D(normalMap, vertexUV).xyz - 1.0);
			eyeVector	= worldToTangentMatrix * eyeVector;
		#endif // NORMAL_MAP
		
		eyeVector	= normalize(eyeVector);
		
		#ifdef NUM_DIRECTIONAL_LIGHTS
		//---------------------------
		for (int i = 0; i < NUM_DIRECTIONAL_LIGHTS; ++i)
		{
			lightDirection	= normalize(-directionalLights[i].direction);
			#ifdef NORMAL_MAP
				lightDirection = worldToTangentMatrix * lightDirection;
			#endif // NORMAL_MAP
			
			contribution	= phong_diffuseReflection(normal, lightDirection)
				* directionalLights[i].diffuse;

			#ifdef SHININESS
				contribution += phong_specularReflection(
					normal,
					lightDirection,
					eyeVector,
					shininess
				) * directionalLights[i].specular * specularIntensity;
			#endif // SHININESS

			phong += contribution * directionalLights[i].color;
		}
		#endif // NUM_DIRECTIONAL_LIGHTS
		
		#ifdef NUM_POINT_LIGHTS
		//---------------------
		for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
		{
			lightDirection			= pointLights[i].position - vertexPosition;
			float distanceToLight 	= length(lightDirection);
			lightDirection 			/= distanceToLight;
			#ifdef NORMAL_MAP
				lightDirection = worldToTangentMatrix * lightDirection;
			#endif // NORMAL_MAP
			
			contribution	= phong_diffuseReflection(normal, lightDirection)
				* pointLights[i].diffuse;

			#ifdef SHININESS
				contribution += phong_specularReflection(
					normal,
					lightDirection,
					eyeVector,
					shininess
				) * pointLights[i].specular * specularIntensity;
			#endif // SHININESS
			
			float attenuation = pointLights[i].attenuationDistance > 0.0
				? max(0.0, 1.0 - distanceToLight / pointLights[i].attenuationDistance) 
				: 1.0;
				
			phong += attenuation * contribution * pointLights[i].color;
		}
		#endif // NUM_POINT_LIGHTS
		
		#ifdef NUM_SPOT_LIGHTS
		//--------------------
		for (int i = 0; i < NUM_SPOT_LIGHTS; ++i)
		{
			lightDirection			= spotLights[i].position - vertexPosition;
			float distanceToLight	= length(lightDirection);
			lightDirection			/= distanceToLight;
			
			vec3  spotDirection	= normalize(-spotLights[i].direction);						
			float cosSpot		= dot(-lightDirection, spotDirection);
			float cosOuter		= spotLights[i].cosOuterConeAngle;

			if (cosOuter < cosSpot)
			{
				#ifdef NORMAL_MAP
					lightDirection	= worldToTangentMatrix * lightDirection;
					spotDirection	= worldToTangentMatrix * spotDirection;
				#endif // NORMAL_MAP
			
				contribution	= phong_diffuseReflection(normal, lightDirection)
					* spotLights[i].diffuse;

				#ifdef SHININESS
					contribution += phong_specularReflection(
						normal,
						lightDirection,
						eyeVector,
						shininess
					) * spotLights[i].specular * specularIntensity;
				#endif // SHININESS
				
				float cosInner	= spotLights[i].cosInnerConeAngle;
				float cutoff	= cosSpot < cosInner && cosOuter < cosInner 
								? (cosSpot - cosOuter) / (cosInner - cosOuter) 
								: 1.0;
				contribution	*= cutoff;
				
				float attenuation = spotLights[i].attenuationDistance > 0.0
					? max(0.0, 1.0 - distanceToLight / spotLights[i].attenuationDistance)
					: 1.0;
				
				phong += attenuation * contribution * spotLights[i].color;
			}
		}
		#endif // NUM_SPOT_LIGHTS
		
	#endif // defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS
	
	diffuse = vec4(diffuse.rgb * phong, diffuse.a);
	
	gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER
