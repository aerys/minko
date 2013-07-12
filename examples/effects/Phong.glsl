float lambert(vec3 normal, vec3 lightDirection)
{
	return saturate(-dot(normalize(normal), lightDirection));
}
