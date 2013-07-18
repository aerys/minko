highp float phong_lambert(highp vec3 normal, highp vec3 lightDirection)
{
	return max(dot(normal, lightDirection), 0.);
}

highp vec3 phong_diffuseReflection(highp vec3 normal, highp vec3 lightDirection, highp vec3 lightColor)
{
	return lightColor * phong_lambert(normal, lightDirection);
}

highp vec3 phong_specularReflection(highp vec3 normal, highp vec3 lightDirection, highp vec3 viewVector, highp vec3 specularColor, highp float shininess)
{
	highp vec3 r = reflect(-lightDirection, normal);

	r = 2. * dot(normal, lightDirection) * normal - lightDirection;

	highp float specular = pow(max(dot(r, viewVector), 0.), shininess);

	return specular * specularColor;
}
