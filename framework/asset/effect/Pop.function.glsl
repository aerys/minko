#ifndef _POP_FUNCTION_GLSL_
#define _POP_FUNCTION_GLSL_

vec4
pop_quantize(vec4 	position,
			 vec3 	normal,
			 float 	popLod,
			 vec3 	popMinBound,
			 vec3 	popMaxBound)
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
pop_quantize(vec4 	position,
			 vec3 	normal,
			 float 	popLod,
			 float 	popFullPrecisionLod,
			 vec3 	popMinBound,
			 vec3 	popMaxBound,
			 float 	protected)
{
	vec4 quantizedPosition = pop_quantize(position, normal, popLod, popMinBound, popMaxBound);

	return mix(quantizedPosition, position, float(popLod >= popFullPrecisionLod || protected != 0.0));
}

float
pop_error(vec4 	position,
		  vec4 	quantizedPosition,
		  float popLod,
		  vec3 	popMinBound,
		  vec3 	popMaxBound,
		  float popErrorBound)
{
	float errorRate = length(position - quantizedPosition) / popErrorBound;

	return clamp(errorRate, 0.0, 1.0);
}

vec4 pop_mix(vec4 	position,
			 vec3 	normal,
			 float 	popPreviousLod,
			 float 	popLod,
			 float 	mixAlpha,
			 float 	popFullPrecisionLod,
			 vec3 	popMinBound,
			 vec3 	popMaxBound,
			 float 	protected)
{
	vec4 sourcePosition = pop_quantize(
		position,
		normal,
		popPreviousLod,
		popFullPrecisionLod,
		popMinBound,
		popMaxBound,
		protected
	);

	vec4 targetPosition = pop_quantize(
		position,
		normal,
		popLod,
		popFullPrecisionLod,
		popMinBound,
		popMaxBound,
		protected
	);

	return mix(sourcePosition, targetPosition, mixAlpha);
}

vec4 pop_mix(vec4 	position,
			 vec3 	normal,
			 float 	startLod,
			 float 	targetLod,
			 float 	startTime,
			 float 	time,
			 float 	period,
			 float 	popFullPrecisionLod,
			 vec3 	popMinBound,
			 vec3 	popMaxBound,
			 float 	protected)
{
    float lodDistance = targetLod - startLod;

    if (lodDistance <= 0.0)
    	return pop_quantize(position, normal, targetLod, popFullPrecisionLod, popMinBound, popMaxBound, protected);

    float lodRangeAlpha = clamp((time - startTime) / (period * lodDistance), 0.0, 1.0);

    float mixCurrentLod = mix(startLod, targetLod, lodRangeAlpha);
    float mixStartLod = floor(mixCurrentLod);
    float mixTargetLod = ceil(mixCurrentLod);
    float mixAlpha = clamp(mod(time - startTime, period) / period, 0.0, 1.0);

    return pop_mix(position, normal, mixStartLod, mixTargetLod, mixAlpha, popFullPrecisionLod, popMinBound, popMaxBound, protected);
}

#endif
