/*
GPU Gems - Effective Water Simulation from Physical Models
http.developer.nvidia.com/GPUGems/gpugems_ch01.html
*/

#ifndef _WATER_FUNCTION_GLSL_
#define _WATER_FUNCTION_GLSL_

#define WATER_WAVE_DIRECTIONAL	0
#define WATER_WAVE_CIRCULAR		1

struct water_Wave {
	int type;
	vec2 momentum;
	float amplitude;
	float speed;
	float length;
	float sharpness;
};

float water_w(float waveLength)
{
	return 6.28000020980835 / waveLength;
}

float water_phi(float speed, float w)
{
	return w * speed;
}

vec3 water_addDirectionalWave(vec3 position, water_Wave wave, float time)
{
	vec2 direction = normalize(wave.momentum);
	float frequency = water_w(wave.length);
	float angle		= dot(direction.xy, position.xz);
	float waveling	= angle * frequency + time * water_phi(wave.speed, frequency);

	return vec3(
		wave.sharpness * wave.amplitude * direction.x * cos(waveling),
		wave.amplitude * sin(waveling),
		wave.sharpness * wave.amplitude * direction.y * cos(waveling)
	);
}

vec3 water_addCircularWave(vec3 position, water_Wave wave, float time)
{
	wave.momentum = normalize(position.xz - wave.momentum.xy);

	return water_addDirectionalWave(position, wave, time);
}

vec3 water_wavePosition(vec3 position, water_Wave wave, float time)
{
	if (wave.type == WATER_WAVE_DIRECTIONAL)
		return water_addDirectionalWave(position, wave, time);
	else
		return water_addCircularWave(position, wave, time);
}

vec3 water_addNormalDirectionalWave(vec3 position, water_Wave wave, float time)
{
	vec2 direction = normalize(wave.momentum);
	float frequency = water_w(wave.length);
	float wa = frequency * wave.amplitude;
	float angle = dot(direction.xy, position.xz);
	float waveling = angle * frequency + time * water_phi(wave.speed, frequency);

	//float derivativX = dot(nDirection.xy, position.x);
	//float derivativY = dot(nDirection.xy, position.y);

	return vec3(
		-direction.x * cos(waveling) * wa,
		-wave.sharpness * sin(waveling) * wa,
		-direction.y * cos(waveling) * wa
	);
}

vec3 water_addNormalCircularWave(vec3 position, water_Wave wave, float time)
{
	wave.momentum = normalize(position.xz - wave.momentum.xy);

	return water_addNormalDirectionalWave(position, wave, time);
}

vec3 water_waveNormal(vec3 position, water_Wave wave, float time)
{
	if (wave.type == WATER_WAVE_DIRECTIONAL)
		return water_addNormalDirectionalWave(position, wave, time);
	else
		return water_addNormalCircularWave(position, wave, time);
}

vec3 water_addTangentDirectionalWave(vec3 position, water_Wave wave, float time)
{
	vec2 direction 	= normalize(wave.momentum);
	float frequency = water_w(wave.length);
	float wa		= frequency * wave.amplitude;
	float angle		= dot(direction.xy, position.xz);
	float waveling  = frequency * angle + time * water_phi(wave.speed, frequency);

	return vec3(
		-wave.sharpness * direction.x * direction.y * wa * sin(waveling),
		direction.y * wa * cos(waveling),
		1.0 - wave.sharpness * direction.y * direction.y * wa * sin(waveling)
	);
}

vec3 water_addTangentCircularWave(vec3 position, water_Wave wave, float time)
{
	wave.momentum = normalize(position.xz - wave.momentum.xy);

	return water_addTangentDirectionalWave(position, wave, time);
}

vec3 water_waveTangent(vec3 position, water_Wave wave, float time)
{
	if (wave.type == WATER_WAVE_DIRECTIONAL)
		return water_addTangentDirectionalWave(position, wave, time);
	else
		return water_addTangentCircularWave(position, wave, time);
}

float water_fresnelFactor(vec3 normalVector, vec3 localView, float fresnelMultiplier, float fogPercent, float fresnelPow)
{
	float fresnel = dot(normalVector, localView) * fresnelMultiplier;
	float fresnelTerm = 1.0 - fresnel;

	fresnelTerm = pow(fresnelTerm, fresnelPow);
	fresnelTerm = clamp(fresnelTerm, 0.0, 1.0);
	fresnelTerm = fresnelTerm * 0.95 + 0.05; // avoid null fresnelTerm

	return fresnelTerm;
}

#endif
