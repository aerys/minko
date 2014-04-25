float w(float waveLength)
{
	return 6.28000020980835 / waveLength;
}

float phi(float speed, float w)
{
	return w * speed;
}

vec4 addDirectionalWave(vec4 position, vec2 direction, float amplitude, float speed, float waveLength, float sharpness, float time)
{
	direction = normalize(direction);

	float frequency = w(waveLength);
	
	float timer		= time;

	float angle = dot(direction.xy, position.xz);

	float y = amplitude * sin(
					angle * frequency + 
					timer * phi(speed, frequency));

	vec2 sharpnessOffset = vec2(1.0, 1.0);

	sharpnessOffset *= (direction.xy * sharpness * amplitude * cos(angle * frequency + timer * phi(speed, frequency)));

	return vec4(0.0, y, 0.0, 0.0);
}

vec4 addCircularWave(vec4 position, vec2 origin, float amplitude, float speed, float waveLength, float sharpness, float time)
{
	vec2 direction = normalize(position.xz - origin.xy);

	return addDirectionalWave(position, direction, amplitude, speed, waveLength, sharpness, time);
}

vec3 addNormalDirectionalWave(vec4 position, vec2 direction, float amplitude, float speed, float waveLength, float sharpness, float time)
{
	vec2 nDirection = normalize(direction);

	float angle		= dot(nDirection.xy, position.xz);
	float frequency = w(waveLength);
	float waveling  = frequency * angle + time * phi(speed, frequency);
	float cosWaveling  = cos(waveling) * amplitude * frequency;
	float sinWaveling = sin(waveling) * amplitude * frequency;

	//float derivativX = dot(nDirection.xy, position.x);
	//float derivativY = dot(nDirection.xy, position.y);

	return vec3(nDirection.x * cosWaveling, 1.0, cosWaveling * nDirection.y);
}

vec3 addNormalCircularWave(vec4 position, vec2 origin, float amplitude, float speed, float waveLength, float sharpness, float time)
{
	vec2 direction = normalize(position.xz - origin.xy);

	return addNormalDirectionalWave(position, direction, amplitude, speed, waveLength, sharpness, time);
}

float fresnelFactor(vec3 normalVector, vec3 localView, float fresnelMultiplier, float fogPercent, float fresnelPow)
{
	float fresnel = dot(normalVector, localView) * fresnelMultiplier;

	float fresnelTerm = 1.0 - fresnel;

	fresnelTerm = pow(fresnelTerm, fresnelPow);
	fresnelTerm = clamp(fresnelTerm, 0.0, 1.0);
	fresnelTerm = fresnelTerm * 0.95 + 0.05; // avoid null fresnelTerm

	return fresnelTerm;
}