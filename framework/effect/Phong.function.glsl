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

// compute the world space to tangent space matrix using the model's normal and tangent
// @precondition worldNormal is expected to be normalized.
mat3 getWorldToTangentSpaceMatrix(vec3 worldNormal, vec3 worldTangent)
{
	worldTangent = normalize(worldTangent);

	mat3 matrix = mat3(
		worldTangent,	
		cross(worldNormal, worldTangent),	
		worldNormal
	);

	mat3 transpose = mat3(
		matrix[0][0], matrix[1][0], matrix[2][0],
		matrix[0][1], matrix[1][1], matrix[2][1],
		matrix[0][2], matrix[1][2], matrix[2][2]
	);

	return transpose;
}

vec2 phong_blinnNewellSphericalProjection(vec3 xyz)
{
	float PI = 3.14159265358979323846264;

	float u = (atan(xyz.x, xyz.z) + PI) / (2. * PI);
	float v = (asin(xyz.y) + PI / 2.) / PI;

	return vec2(u, v);
}

vec3 phong_cartesian3DToSpherical3D(vec3 xyz)
{
	float r 	= sqrt(xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z);
	float phi 	= acos(xyz.y);
	float theta = atan(xyz.z, xyz.x);

	return vec3(r, theta, phi);
}


vec2 phong_spherical3DToCartesian2D(float theta, float phi)
{
	return vec2(
		cos(theta) * cos(phi),
		sin(theta) * cos(phi)
	) * 0.5 + vec2(0.5, 0.5);
}
