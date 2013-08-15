struct DirectionalLight
{
	vec3 color;
	float diffuse;
	float specular;
	vec3 direction;	
};

float phong_diffuseReflection(vec3 normal, vec3 lightDirection)
{
	return max(dot(normal, lightDirection), 0.);
}

float phong_specularReflection(vec3 normal, vec3 lightDirection, vec3 viewVector, float shininess)
{
	float NdotL = dot(normal, lightDirection);
	
	if (NdotL < 0.0)
	{
		return 0.0;
	}
	else
	{
		vec3 reflected = 2.0 * NdotL * normal - lightDirection;

		return pow(max(dot(reflected, viewVector), 0.0), shininess);
	}
}
