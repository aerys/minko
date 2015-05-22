#ifndef _MATH_FUNCTION_GLSL_
#define _MATH_FUNCTION_GLSL_

#define PI 3.1415926535897932384626433832795

float square(float x)
{
    return x * x;
}

int square(int x)
{
    return x * x;
}

vec2 square(vec2 x)
{
    return x * x;
}

vec3 square(vec3 x)
{
    return x * x;
}

vec4 square(vec4 x)
{
    return x * x;
}

float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}

vec2 saturate(vec2 x)
{
    return clamp(x, 0.0, 1.0);
}

vec3 saturate(vec3 x)
{
    return clamp(x, 0.0, 1.0);
}

vec4 saturate(vec4 x)
{
    return clamp(x, 0.0, 1.0);
}

#endif // _MATH_FUNCTION_GLSL_
