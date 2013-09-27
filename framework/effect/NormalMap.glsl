
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

	mat3 transpose = mat3(matrix[0][0], matrix[1][0], matrix[2][0],
						matrix[0][1], matrix[1][1], matrix[2][1],
						matrix[0][2], matrix[1][2], matrix[2][2]);

	return transpose;
}	