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

vec3 phong_fresnel(vec3 specularColor, vec3 lightDirection, vec3 eyeVector)
{
	vec3	halfVector	= normalize(eyeVector + lightDirection);
	float 	k			= 1.0 - max(0.0, dot(eyeVector, halfVector));
	float 	kk			= k * k;
	
	// Schlick's approximation
	return specularColor + (vec3(1.0) - specularColor) * kk * kk * k;
}


// compute the world space to tangent space matrix using the model's normal and tangent
// @precondition worldNormal is expected to be normalized.

mat3 phong_getTangentToWorldSpaceMatrix(vec3 worldNormal, vec3 worldTangent)
{
	worldTangent = normalize(worldTangent);

	return mat3(
		worldTangent,	
		cross(worldNormal, worldTangent),	// bi-tangent
		worldNormal
	);
}

mat3 phong_getWorldToTangentSpaceMatrix(vec3 worldNormal, vec3 worldTangent)
{
	mat3 matrix = phong_getTangentToWorldSpaceMatrix(worldNormal, worldTangent);

	mat3 transpose = mat3(
		matrix[0][0], matrix[1][0], matrix[2][0],
		matrix[0][1], matrix[1][1], matrix[2][1],
		matrix[0][2], matrix[1][2], matrix[2][2]
	);

	return transpose;
}
