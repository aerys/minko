#ifdef FOG_TECHNIQUE

#define FOG_TECHNIQUE_LIN	1
#define FOG_TECHNIQUE_EXP	2
#define FOG_TECHNIQUE_EXP2	3

vec4
fog_sampleFog(vec3 	fragColor,
			  float fragDist,
			  vec3 	fogColor,
			  float fogDensity,
			  float fogStart,
			  float fogEnd)
{
	float fogFactor = 0.0;

	const float LOG2 = 1.442695;

#if FOG_TECHNIQUE == FOG_TECHNIQUE_LIN
	fogFactor = fogDensity * (1.0 - clamp((fogEnd - fragDist) / (fogEnd - fogStart), 0.0, 1.0));
#elif FOG_TECHNIQUE == FOG_TECHNIQUE_EXP
    fogFactor = fogDensity * (1.0 - clamp(1.0 / exp2((fragDist - fogStart) * LOG2 / (fogEnd - fogStart) * 4.0), 0.0, 1.0));
#elif FOG_TECHNIQUE == FOG_TECHNIQUE_EXP2
    fogFactor = fogDensity * (1.0 - clamp(1.0 / exp2((fragDist * fragDist - fogStart * fogStart) * LOG2 / (fogEnd - fogStart)), 0.0, 1.0));
#endif

    fogFactor = clamp(fogFactor, 0.0, 1.0);

	return mix(fragColor, fogColor, fogFactor);
}

#endif // FOG_TECHNIQUE
