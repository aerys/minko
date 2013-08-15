struct DirectionalLight
{
	vec3 color;
	vec3 direction;
};

float phong_lambert(vec3 normal, vec3 lightDirection)
{
	return max(dot(normal, lightDirection), 0.);
}

vec3 phong_diffuseReflection(vec3 normal, vec3 lightDirection, vec3 lightColor)
{
	return lightColor * phong_lambert(normal, lightDirection);
}

vec3 phong_specularReflection(vec3 normal, vec3 lightDirection, vec3 viewVector, vec3 specularColor, float shininess)
{
	float NdotL = dot(normal, lightDirection);
	
	if (NdotL < 0.0)
	{
		return vec3(0.0);
	}
	else
	{
		vec3 reflected = 2.0 * NdotL * normal - lightDirection;

		float specular = pow(max(dot(reflected, viewVector), 0.0), shininess);

		return specular * specularColor;
	}
}
