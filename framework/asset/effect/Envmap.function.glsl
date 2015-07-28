#ifndef _ENVMAP_FUNCTION_GLSL_
#define _ENVMAP_FUNCTION_GLSL_

#pragma include "Math.function.glsl"

vec4
envmap_sampleEnvironmentCubeMap(samplerCube environmentCubemap,
							    vec3 eyeDir,
							    vec3 normalDir)
{
	return textureCube(environmentCubemap, reflect(eyeDir, normalDir));
}

vec4
envmap_sampleEnvironmentMap2D(sampler2D environmentMap2d,
							  vec3 eyeDir,
							  vec3 normalDir)
{
	return texture2D(environmentMap2d, normalToLatLongUV(reflect(eyeDir, normalDir)));
}

#endif
