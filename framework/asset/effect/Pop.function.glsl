vec4
pop_quantify(vec4 position, float popLod, vec3 popMinBound, vec3 popMaxBound)
{
	float segmentCount = pow(float(2.0), popLod);

	vec3 boxSize = popMaxBound - popMinBound;
	vec3 offset = position.xyz - popMinBound;

	const float minBoxSize = 1.0e-7;
	boxSize.x = max(boxSize.x, minBoxSize);
	boxSize.y = max(boxSize.y, minBoxSize);
	boxSize.z = max(boxSize.z, minBoxSize);

	float indexX = floor(offset.x * segmentCount / boxSize.x);
	float indexY = floor(offset.y * segmentCount / boxSize.y);
	float indexZ = floor(offset.z * segmentCount / boxSize.z);

	return vec4(vec3((indexX + 0.5) * boxSize.x / segmentCount,
					 (indexY + 0.5) * boxSize.y / segmentCount,
					 (indexZ + 0.5) * boxSize.z / segmentCount) + popMinBound,
				position.w);
}

float
pop_error(vec4 position, vec4 quantizedPosition, float popLod, vec3 popMinBound, vec3 popMaxBound, float popErrorBound)
{
	float errorRate = length(position - quantizedPosition) / popErrorBound;

	return clamp(errorRate, 0.0, 1.0);
}
