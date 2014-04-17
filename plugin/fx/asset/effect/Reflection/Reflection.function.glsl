uniform sampler2D reflectionMap;

vec4 getReflectionColor(vec3 reflectionPosition)
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