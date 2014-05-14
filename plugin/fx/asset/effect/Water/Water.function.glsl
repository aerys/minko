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

	vec2 sharpnessOffset = vec2(1.f, 1.f);

	sharpnessOffset *= (direction.xy * sharpness * amplitude * cos(angle * frequency + timer * phi(speed, frequency)));

	return vec4(0, y, 0, 0);
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

	return vec3(nDirection.x * cosWaveling, 1, cosWaveling * nDirection.y);
}

vec3 addNormalCircularWave(vec4 position, vec2 origin, float amplitude, float speed, float waveLength, float sharpness, float time)
{
	vec2 direction = normalize(position.xz - origin.xy);

	return addNormalDirectionalWave(position, direction, amplitude, speed, waveLength, sharpness, time);
}