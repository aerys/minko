float lambert(vec3 normal, vec3 lightDirection)
{
	return -dot(normalize(normal), lightDirection);
}
