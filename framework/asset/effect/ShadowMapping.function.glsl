#define SHADOW_MAPPING_TECHNIQUE_HARD			0
#define SHADOW_MAPPING_TECHNIQUE_ESM			1
#define SHADOW_MAPPING_TECHNIQUE_PCF			2
#define SHADOW_MAPPING_TECHNIQUE_PCF_POISSON	3

#ifndef SHADOW_MAPPING_TECHNIQUE
# define SHADOW_MAPPING_TECHNIQUE   SHADOW_MAPPING_TECHNIQUE_ESM
#endif

#define SHADOW_MAPPING_NEAR_ONE 0.999

float shadowMapping_random(vec4 seed)
{
    float dot_product = dot(seed, vec4(12.9898, 78.233, 45.164, 94.673));

    return fract(sin(dot_product) * 43758.5453);
}

bool shadowMapping_vertexIsInShadowMap(vec3 vertexLightPosition)
{
    return vertexLightPosition.z < SHADOW_MAPPING_NEAR_ONE && vertexLightPosition.z > -SHADOW_MAPPING_NEAR_ONE
        && vertexLightPosition.x < SHADOW_MAPPING_NEAR_ONE && vertexLightPosition.x > -SHADOW_MAPPING_NEAR_ONE
        && vertexLightPosition.y < SHADOW_MAPPING_NEAR_ONE && vertexLightPosition.y > -SHADOW_MAPPING_NEAR_ONE;
}

float shadowMapping_texture2DDepth(sampler2D depths, vec2 uv, float zNear, float zFar)
{
    float depth = unpackFloat8bitRGBA(texture2D(depths, uv));

    return (depth * (zFar - zNear)) + zNear;
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

    depth = exp(-c * min(compare - depth, 0.05));
    depth = clamp(depth, 0.0, 1.0);

    return depth;
}
