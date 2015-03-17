vec4
pop_quantify(vec4 position, vec3 normal, float popLod, vec3 popMinBound, vec3 popMaxBound)
{
	float segmentCount = pow(float(2.0), popLod);

	vec3 boxSize = popMaxBound - popMinBound;
	vec3 offset = position.xyz - popMinBound;

	vec3 minBoxSize = vec3(1.0e-7);
	boxSize = max(boxSize, minBoxSize);

	vec3 popIndex = floor(offset * segmentCount / boxSize);

	vec3 quantizedPosition = (popIndex + 0.5) * boxSize / segmentCount + popMinBound;

	return vec4(quantizedPosition, position.w);
}

vec4
pop_quantify(vec4 position, vec3 normal, float popLod, float popFullPrecisionLod, vec3 popMinBound, vec3 popMaxBound)
{
	vec4 quantizedPosition = pop_quantify(position, normal, popLod, popMinBound, popMaxBound);

	return popLod >= popFullPrecisionLod
		? position
		: quantizedPosition;
}

float
pop_error(vec4 position, vec4 quantizedPosition, float popLod, vec3 popMinBound, vec3 popMaxBound, float popErrorBound)
{
	float errorRate = length(position - quantizedPosition) / popErrorBound;

	return clamp(errorRate, 0.0, 1.0);
}

vec4 pop_blend(vec4 position, vec3 normal, float popLod, float popBlendingLod, float popFullPrecisionLod, vec3 popMinBound, vec3 popMaxBound)
{
	float intPopBlendingLod = floor(popBlendingLod);

	vec4 quantizedPosition = pop_quantify(position, normal, popLod, popFullPrecisionLod, popMinBound, popMaxBound);
	vec4 blendingQuantizedPosition = pop_quantify(position, normal, intPopBlendingLod, popFullPrecisionLod, popMinBound, popMaxBound);

	vec4 blendedQuantizedPosition = mix(blendingQuantizedPosition, quantizedPosition, fract(popBlendingLod));

	return intPopBlendingLod >= popFullPrecisionLod
		? position
		: blendedQuantizedPosition;
}