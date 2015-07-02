#ifndef _LIGHTMAPPING_FUNCTION_GLSL_
#define _LIGHTMAPPING_FUNCTION_GLSL_

vec4 lightMapping_overlay(vec4 diffuseColor, vec4 lightMapSample)
{
    vec4 light = vec4(lightMapSample.r);

    if (light.r < 0.5)
        return vec4(2.0) * diffuseColor * light;

    return vec4(1.0) - vec4(2.0) * (vec4(1.0) - diffuseColor) * (vec4(1.0) - light);
}

vec4 lightMapping_multiply(vec4 diffuseColor, vec4 lightMapSample)
{
    return diffuseColor * vec4(lightMapSample.r);
}

#endif // _LIGHTMAPPING_FUNCTION_GLSL_
