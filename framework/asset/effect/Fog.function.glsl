uniform vec4 fogColor;
uniform float fogDensity;

uniform float fogStart;
uniform float fogEnd;

float
fog_Percent(vec4 fragCoord)
{
	float fragDist = fragCoord.z / fragCoord.w;
	float fogFactor = (fogEnd - fragDist) / (fogEnd - fogStart);

	return clamp(fogFactor, 0.0, 1.0);
}

vec4
fog_sampleFog(vec4 fragColor, vec4 fragCoord)
{
	float fragDist = fragCoord.z / fragCoord.w;

	float fogFactor = 0.0;

	const float LOG2 = 1.442695;

#ifdef FOG_LIN
	fogFactor = fogDensity * (1.0 - clamp((fogEnd - fragDist) / (fogEnd - fogStart), 0.0, 1.0));
#elif defined(FOG_EXP)
    fogFactor = fogDensity * (1.0 - clamp(1.0 / exp2((fragDist - fogStart) * LOG2 / (fogEnd - fogStart) * 4.0), 0.0, 1.0));
#elif defined(FOG_EXP2)
    fogFactor = fogDensity * (1.0 - clamp(1.0 / exp2((fragDist * fragDist - fogStart * fogStart) * LOG2 / (fogEnd - fogStart)), 0.0, 1.0));
#endif

    fogFactor = clamp(fogFactor, 0.0, 1.0);

	return mix(fragColor, fogColor, fogFactor);
}
