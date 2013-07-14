float phong_lambert(vec3 normal, vec3 lightDirection)
{
	return max(dot(normal, lightDirection), 0.f);
}
