#ifdef FOG_TECHNIQUE

#ifndef _FOG_FUNCTION_GLSL_
#define _FOG_FUNCTION_GLSL_

#pragma include "Math.function.glsl"

#define FOG_TECHNIQUE_LIN	1
#define FOG_TECHNIQUE_EXP	2
#define FOG_TECHNIQUE_EXP2	3

vec3
fog_sampleFog(vec3 	fragColor,
			  float fragDist,
			  vec3 	fogColor,
			  float fogDensity,
			  float fogStart,
			  float fogEnd)
{
	float fogFactor = 0.0;
	const float LOG2 = 1.442695;
	float dist = (fragDist - fogStart) / (fogEnd - fogStart);

#if FOG_TECHNIQUE == FOG_TECHNIQUE_LIN
	fogFactor = clamp(dist * fogDensity, 0.0, 1.0);
#elif FOG_TECHNIQUE == FOG_TECHNIQUE_EXP
    fogFactor = 1.0 - clamp(1.0 / exp(dist * fogDensity), 0.0, 1.0);
#elif FOG_TECHNIQUE == FOG_TECHNIQUE_EXP2
    fogFactor = 1.0 - clamp(1.0 / exp(dist * fogDensity * dist * fogDensity), 0.0, 1.0);
#endif

	return mix(fragColor, fogColor, saturate(fogFactor));
}

#endif // _FOG_FUNCTION_GLSL_

#endif // FOG_TECHNIQUE
