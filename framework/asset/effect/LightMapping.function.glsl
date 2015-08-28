#ifndef _LIGHTMAPPING_FUNCTION_GLSL_
#define _LIGHTMAPPING_FUNCTION_GLSL_

vec4 lightMapping_overlay(vec4 diffuseColor, vec4 lightMapSample)
{
    if (lightMapSample.r < 0.5)
        return vec4(2.0) * diffuseColor * lightMapSample;

    return vec4(1.0) - vec4(2.0) * (vec4(1.0) - diffuseColor) * (vec4(1.0) - lightMapSample);
}

vec4 lightMapping_multiply(vec4 diffuseColor, vec4 lightMapSample)
{
    return diffuseColor * lightMapSample;
}

#endif // _LIGHTMAPPING_FUNCTION_GLSL_
