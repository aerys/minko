
uniform sampler2D	environmentMap2d;
uniform int			environmentType2d;

uniform samplerCube	environmentCubemap;

vec3 
envmap_cartesian3DToSpherical3D(vec3 xyz)
{
	float r 	= sqrt(xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z);
	float phi 	= acos(xyz.y);
	float theta = atan(xyz.z, xyz.x);

	return vec3(r, theta, phi);
}


vec2 
envmap_spherical3DToCartesian2D(float theta, float phi)
{
	return vec2(
		cos(theta) * cos(phi),
		sin(theta) * cos(phi)
	) * 0.5 + vec2(0.5, 0.5);
}

vec2 
envmap_blinnNewellProjection(vec3 xyz)
{
	float PI = 3.14159265358979323846264;

	float u = (atan(xyz.x, xyz.z) + PI) / (2. * PI);
	float v = (asin(xyz.y) + PI / 2.) / PI;

	return vec2(u, v);
}


vec4
envmap_sampleEnvironmentMap(vec3 eyeDir, vec3 normalDir)
{
	// Both 'eyeDir' and 'normalDir' must be normalized at this point.

	vec4 envmapColor = vec4(0.0, 0.0, 0.0, 0.0);

	#if defined(ENVIRONMENT_CUBE_MAP)

		vec3 	reflectedDir	= reflect(eyeDir, normalDir);

		envmapColor 			= textureCube(environmentCubemap, reflectedDir);

	#elif defined(ENVIRONMENT_MAP_2D)

		vec3 	reflectedDir	= reflect(eyeDir, normalDir);
		int 	mappingType		= 0; // probe mapping by default (cf minko/Common.hpp)
		vec2 	envmapUV 		= vec2(0.0);

		#if defined(ENVIRONMENT_TYPE_2D)

			mappingType = environmentType2d;

		#endif  //ENVIRONMENT_TYPE_2D


		if (mappingType == 1) // blinn-newell
			envmapUV 				= envmap_blinnNewellProjection(reflectedDir);
		else
		{
			vec3 sphericalCoords	= envmap_cartesian3DToSpherical3D(reflectedDir);
			envmapUV 				= envmap_spherical3DToCartesian2D(reflectedDir.y, reflectedDir.z);
		}

		envmapColor = texture2D(environmentMap2d, envmapUV);

	#endif  // ENVIRONMENT_MAP_2D

	return envmapColor;
}
