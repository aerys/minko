uniform vec4 fogColor;
uniform float fogDensity;

uniform float fogStart;
uniform float fogEnd;

vec4
fog_sampleFog(vec4 fragColor, vec4 fragCoord)
{
	float fragDist = fragCoord.z / fragCoord.w;

	float fogFactor = 1.0f;
	
#ifdef FOG_LIN
	fogFactor = (fogEnd - fragDist) / (fogEnd - fogStart);
#elif defined(FOG_EXP)
	const float LOG2 = 1.442695f;

	fogFactor = clamp(exp2(-fogDensity * fragDist * LOG2), 0.0f, 1.0f);
#elif defined(FOG_EXP2)
	const float LOG2 = 1.442695f;

	fogFactor = clamp(exp2(-fogDensity * fogDensity * fragDist * fragDist * LOG2), 0.0f, 1.0f);
#endif

	return mix(fogColor, fragColor, fogFactor);
}