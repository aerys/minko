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

vec4
pop_quantify(vec4 position, float popLod, float popFullPrecisionLod, vec3 popMinBound, vec3 popMaxBound)
{
	vec4 quantizedPosition = pop_quantify(position, popLod, popMinBound, popMaxBound);

	return step(popLod, popFullPrecisionLod) * quantizedPosition +
		   step(popFullPrecisionLod, popLod) * position;
}

float
pop_error(vec4 position, vec4 quantizedPosition, float popLod, vec3 popMinBound, vec3 popMaxBound, float popErrorBound)
{
	float errorRate = length(position - quantizedPosition) / popErrorBound;

	return clamp(errorRate, 0.0, 1.0);
}

vec4 pop_blend(vec4 position, float popLod, float popBlendingLod, float popFullPrecisionLod, vec3 popMinBound, vec3 popMaxBound)
{
	float intPopBlendingLod = floor(popBlendingLod);

	vec4 quantizedPosition = pop_quantify(position, popLod, popMinBound, popMaxBound);
	vec4 blendingQuantizedPosition = pop_quantify(position, intPopBlendingLod, popMinBound, popMaxBound);

	vec4 blendedQuantizedPosition = mix(blendingQuantizedPosition, quantizedPosition, fract(popBlendingLod));

	return step(intPopBlendingLod, popFullPrecisionLod) * blendedQuantizedPosition +
		   step(popFullPrecisionLod, intPopBlendingLod) * position;
}
