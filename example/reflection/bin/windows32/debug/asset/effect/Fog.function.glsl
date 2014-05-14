uniform vec4 fogColor;
uniform float fogDensity;

uniform float fogStart;
uniform float fogEnd;

vec4
fog_sampleFog(vec4 fragColor, vec4 fragCoord)
{
	float fragDist = fragCoord.z / fragCoord.w;

	float fogFactor = 1.0;
	
#ifdef FOG_LIN
	fogFactor = (fogEnd - fragDist) / (fogEnd - fogStart);
#elif defined(FOG_EXP)
	const float LOG2 = 1.442695;

	fogFactor = clamp(exp2(-fogDensity * fragDist * LOG2), 0.0, 1.0);
#elif defined(FOG_EXP2)
	const float LOG2 = 1.442695;

	fogFactor = clamp(exp2(-fogDensity * fogDensity * fragDist * fragDist * LOG2), 0.0, 1.0);
#endif

	return mix(fogColor, fragColor, fogFactor);
}