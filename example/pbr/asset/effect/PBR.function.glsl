#ifndef _PBR_FUNCTION_GLSL_
#define _PBR_FUNCTION_GLSL_

#pragma include "BRDF.function.glsl"

vec3 pbr_diffuse(vec3 diffuseColor, float roughness, float NoV, float NoL, float VoH)
{
    return brdf_diffuse(diffuseColor, roughness, NoV, NoL, VoH) * NoL;
}

vec3 pbr_specular(vec3 specularColor, float roughness, float NoL, float NoV, float NoH, float VoH)
{
    // f = D * F * G / (4 * (N.L) * (N.V));
    return brdf_distribution(roughness, NoH)
         * brdf_fresnel(specularColor, VoH)
         * brdf_geometricVisibility(roughness, NoV, NoL, VoH)//;
        //  * (4.0 * NoL * NoV);
        * NoL;
}

#endif // _PBR_FUNCTION_GLSL_
