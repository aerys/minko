float phong_lambert(vec3 normal, vec3 lightDirection)
{
	return max(dot(normal, lightDirection), 0.f);
}

vec3 phong_diffuseReflection(vec3 normal, vec3 lightDirection, vec3 lightColor)
{
	return lightColor * phong_lambert(normal, lightDirection);
}

vec3 phong_specularReflection(vec3 normal, vec3 lightDirection, vec3 viewVector, vec3 specularColor, float shininess)
{
	vec3 r = reflect(-lightDirection, normal);

	r = 2 * dot(normal, lightDirection) * normal - lightDirection;

	float specular = pow(max(dot(r, viewVector), 0.), shininess);

	return specular * specularColor;
}
