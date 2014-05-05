vec4 getReflectionColor(vec4 reflectionPosition, sampler2D reflectionMap)
{
	// Transform the projective reflection texcoords to NDC space
	// and scale and offset xy to correctly sample a DX texture
	vec4 reflectionTexCoord = reflectionPosition;
	reflectionTexCoord.xyz /= reflectionTexCoord.w;
	
	reflectionTexCoord.x = 0.5f * reflectionTexCoord.x + 0.5f;
	reflectionTexCoord.y = -0.5f * reflectionTexCoord.y + 0.5f;
	
	// reflect more based on distance from the camera
	reflectionTexCoord.z = .1f / reflectionTexCoord.z;
	
	return texture2D(reflectionMap, reflectionTexCoord.xy);
}

vec4 getDuDvReflectionColor(vec4 reflectionPosition, vec2 uv, sampler2D reflectionMap, vec2 offset)
{
	// Transform the projective reflection texcoords to NDC space
	// and scale and offset xy to correctly sample a DX texture
	vec4 reflectionTexCoord = reflectionPosition;
	reflectionTexCoord.xyz /= reflectionTexCoord.w;
	
	reflectionTexCoord.x = 0.5f * reflectionTexCoord.x + 0.5f;
	reflectionTexCoord.y = -0.5f * reflectionTexCoord.y + 0.5f;
	
	// reflect more based on distance from the camera
	reflectionTexCoord.z = .1f / reflectionTexCoord.z;

	return texture2D(reflectionMap, clamp(reflectionTexCoord.xy + offset, 0.0, 1.0));
}