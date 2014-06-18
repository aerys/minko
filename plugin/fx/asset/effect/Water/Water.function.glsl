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
	
	float angle		= dot(direction.xy, position.xz);
	float waveling	= angle * frequency + time * phi(speed, frequency);
	float y			= amplitude * sin(waveling);

	return vec4(sharpness * amplitude * direction.x * cos(waveling), y, sharpness * amplitude * direction.y * cos(waveling), 0.0);
}

vec4 addCircularWave(vec4 position, vec2 origin, float amplitude, float speed, float waveLength, float sharpness, float time)
{
	vec2 direction = normalize(position.xz - origin.xy);

	return addDirectionalWave(position, direction, amplitude, speed, waveLength, sharpness, time);
}

vec3 addNormalDirectionalWave(vec4 position, vec2 direction, float amplitude, float speed, float waveLength, float sharpness, float time)
{
	vec2 nDirection = normalize(direction);
	float frequency = w(waveLength);
	float WA = frequency * amplitude;
	float angle		= dot(nDirection.xy, position.xz);
	float waveling  = frequency * angle + time * phi(speed, frequency);
	
	//float derivativX = dot(nDirection.xy, position.x);
	//float derivativY = dot(nDirection.xy, position.y);

	return vec3(-nDirection.x * WA * cos(waveling), -sharpness * WA * sin(waveling), -nDirection.y * WA * cos(waveling));
}

vec3 addNormalCircularWave(vec4 position, vec2 origin, float amplitude, float speed, float waveLength, float sharpness, float time)
{
	vec2 direction = normalize(position.xz - origin.xy);

	return addNormalDirectionalWave(position, direction, amplitude, speed, waveLength, sharpness, time);
}

vec3 addTangentDirectionalWave(vec4 position, vec2 direction, float amplitude, float speed, float waveLength, float sharpness, float time)
{
	vec2 nDirection = normalize(direction);
	float frequency = w(waveLength);
	float WA		= frequency * amplitude;
	float angle		= dot(nDirection.xy, position.xz);
	float waveling  = frequency * angle + time * phi(speed, frequency);
	
	return vec3(-sharpness * nDirection.x * nDirection.y * WA * sin(waveling), 
				nDirection.y * WA * cos(waveling), 
				-sharpness * nDirection.y * nDirection.y * WA * sin(waveling));
}

vec3 addTangentCircularWave(vec4 position, vec2 origin, float amplitude, float speed, float waveLength, float sharpness, float time)
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