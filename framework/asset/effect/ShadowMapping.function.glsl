#pragma include "Pack.function.glsl"

#define SHADOW_MAPPING_TECHNIQUE_HARD			0
#define SHADOW_MAPPING_TECHNIQUE_ESM			1
#define SHADOW_MAPPING_TECHNIQUE_PCF			2
#define SHADOW_MAPPING_TECHNIQUE_PCF_POISSON	3

#ifndef SHADOW_MAPPING_TECHNIQUE
# define SHADOW_MAPPING_TECHNIQUE   SHADOW_MAPPING_TECHNIQUE_ESM
#endif

#define SHADOW_MAPPING_NEAR_ONE         0.99
#define SHADOW_MAPPING_MAX_NUM_CASCADES 4

float shadowMapping_random(vec4 seed)
{
    float dot_product = dot(seed, vec4(12.9898, 78.233, 45.164, 94.673));

    return fract(sin(dot_product) * 43758.5453);
}

bool shadowMapping_vertexIsInShadowMap(vec3 vertexLightPosition)
{
    return all(bvec3(step(vertexLightPosition, vec3(SHADOW_MAPPING_NEAR_ONE))))
        && all(bvec3(step(vec3(-SHADOW_MAPPING_NEAR_ONE), vertexLightPosition)));
}

float shadowMapping_texture2DDepth(sampler2D depths, vec2 uv, float zNear, float zFar)
{
    float depth = unpackFloat8bitRGBA(texture2D(depths, uv));

    //return (depth * (zFar - zNear)) + zNear;
    return (depth - 0.5) / 0.5;
}

float shadowMapping_texture2DCompare(sampler2D depths, vec2 uv, float compare, float zNear, float zFar)
{
    float depth = shadowMapping_texture2DDepth(depths, uv, zNear, zFar);

    return step(compare, depth);
}

float shadowMapping_texture2DShadowLerp(sampler2D depths, vec2 size, vec2 uv, float compare, float zNear, float zFar)
{
    vec2 texelSize = vec2(1.0) / size;
    vec2 f = fract(uv * size + 0.5);
    vec2 centroidUV = floor(uv * size + 0.5) / size;

    float lb = shadowMapping_texture2DCompare(depths, centroidUV + texelSize * vec2(0.0, 0.0), compare, zNear, zFar);
    float lt = shadowMapping_texture2DCompare(depths, centroidUV + texelSize * vec2(0.0, 1.0), compare, zNear, zFar);
    float rb = shadowMapping_texture2DCompare(depths, centroidUV + texelSize * vec2(1.0, 0.0), compare, zNear, zFar);
    float rt = shadowMapping_texture2DCompare(depths, centroidUV + texelSize * vec2(1.0, 1.0), compare, zNear, zFar);
    float a = mix(lb, lt, f.y);
    float b = mix(rb, rt, f.y);
    float c = mix(a, b, f.x);

    return c;
}

float shadowMapping_PCF(sampler2D depths, vec2 size, vec2 uv, float compare, float zNear, float zFar)
{
    float result = 0.0;
    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            vec2 off = vec2(x, y) / size;

            result += shadowMapping_texture2DCompare(depths, uv + off, compare, zNear, zFar);
        }
    }
    return result / 25.0;
}

float shadowMapping_PCFPoisson(sampler2D depths, vec2 size, vec2 uv, float compare, float zNear, float zFar, sampler2D randoms, vec2 randomUV, float spread)
{
    float result = 0.0;
    for (int x = 0; x < 10; x++)
    {
        vec2 off = (texture2D(randoms, fract(vec2(shadowMapping_random(vec4(randomUV, vec2(x)))))).xy - .5) / size * spread;
        // vec2 off = normalize(shadowMapping_random(vec4(randomUV, vec2(x))) - 0.5) / size * spread;

        // result += shadowMapping_texture2DShadowLerp(depths, size, uv + off, compare, zNear, zFar);
        result += shadowMapping_texture2DCompare(depths, uv + off, compare, zNear, zFar);
    }

    return result / 10.0;
}

float shadowMapping_ESM(sampler2D depths, vec2 uv, float compare, float zNear, float zFar, float c)
{
    float depth = shadowMapping_texture2DDepth(depths, uv, zNear, zFar);

    // depth = exp(-c * min(compare - depth, 0.05));
	depth = exp(c * depth) * exp(-c * compare);
    depth = clamp(depth, 0.0, 1.0);

    return depth;
}

vec4 shadowMapping_getCascadeWeights(float depth, vec4 splitNear, vec4 splitFar)
{
	vec4 near = step(splitNear, vec4(depth));
	vec4 far = step(depth, splitFar);

	return near * far;
}

vec4 shadowMapping_getCascadeViewport(vec4 weights)
{
	vec2 offset = vec2(0.0, 0.5) * weights.x
		+ vec2(0.5, 0.5) * weights.y
		+ vec2(0.0, 0.0) * weights.z
		+ vec2(0.5, 0.0) * weights.w;

	return vec4(offset, 0.5, 0.5);
}

mat4 shadowMapping_getCascadeViewProjection(vec4 weights, mat4 viewProj[SHADOW_MAPPING_MAX_NUM_CASCADES])
{
	return viewProj[0] * weights.x + viewProj[1] * weights.y + viewProj[2] * weights.z + viewProj[3] * weights.w;
}

float shadowMapping_getCascadeZ(vec4 weights, float z[SHADOW_MAPPING_MAX_NUM_CASCADES])
{
	return z[0] * weights.x + z[1] * weights.y + z[2] * weights.z + z[3] * weights.w;
}
