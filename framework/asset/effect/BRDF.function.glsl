#ifndef _BRDF_FUNCTION_GLSL_
#define _BRDF_FUNCTION_GLSL_

#pragma include "Math.function.glsl"

// Physically based shading model
// parameterized with the below options
// Microfacet specular = D * G * F / (4 * NoL * NoV) = D * Vis * F
// Vis = G / (4 * NoL * NoV)

// Diffuse model
#define BRDF_DIFFUSE_LAMBERT        0
#define BRDF_DIFFUSE_BURLEY         1
#define BRDF_DIFFUSE_OREN_NAYAR     2
#define BRDF_DIFFUSE                BRDF_DIFFUSE_LAMBERT

// Microfacet distribution function
#define BRDF_SPECULAR_D_BLINN       0
#define BRDF_SPECULAR_D_BECKMANN    1
#define BRDF_SPECULAR_D_GGX         2
#define BRDF_SPECULAR_D             BRDF_SPECULAR_D_GGX

// Geometric attenuation or shadowing
#define BRDF_SPECULAR_G_IMPLICIT    0
#define BRDF_SPECULAR_G_NEUMANN     1
#define BRDF_SPECULAR_G_KELEMEN     2
#define BRDF_SPECULAR_G_SCHLICK     3
#define BRDF_SPECULAR_G_SMITH       4
#define BRDF_SPECULAR_G             BRDF_SPECULAR_G_SCHLICK

// Fresnel
#define BRDF_SPECULAR_F_NONE        0
#define BRDF_SPECULAR_F_SCHLICK     1
#define BRDF_SPECULAR_F_FRESNEL     2
#define BRDF_SPECULAR_F             BRDF_SPECULAR_F_SCHLICK

vec3 brdf_diffuseLambert(vec3 diffuseColor)
{
    return diffuseColor / PI;
}

// [Burley 2012, "Physically-Based Shading at Disney"]
vec3 brdf_diffuseBurley(vec3 diffuseColor, float roughness, float NoV, float NoL, float VoH)
{
    float FD90 = 0.5 + 2.0 * VoH * VoH * roughness;
    float FdV = 1.0 + (FD90 - 1.0) * pow(1.0 - NoV, 5.0);
    float FdL = 1.0 + (FD90 - 1.0) * pow(1.0 - NoL, 5.0);

    return diffuseColor * (1.0 / PI * FdV * FdL);
}

// [Gotanda 2012, "Beyond a Simple Physically Based Blinn-Phong Model in Real-Time"]
vec3 brdf_diffuseOrenNayar(vec3 diffuseColor, float roughness, float NoV, float NoL, float VoH)
{
    float VoL = 2.0 * VoH - 1.0;
    float m = roughness * roughness;
    float m2 = m * m;
    float C1 = 1.0 - 0.5 * m2 / (m2 + 0.33);
    float Cosri = VoL - NoV * NoL;
    float C2 = 0.45 * m2 / (m2 + 0.09) * Cosri * (Cosri >= 0.0 ? min(1.0, NoL / NoV ) : NoL);

    return diffuseColor / PI * (NoL * C1 + C2);
}

// [Blinn 1977, "Models of light reflection for computer synthesized pictures"]
float brdf_D_Blinn(float roughness, float NoH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float n = 2.0 / m2 - 2.0;

    return (n + 2.0) / (2.0 * PI) * pow(max(abs(NoH), 0.000001), n);
}
// [Beckmann 1963, "The scattering of electromagnetic waves from rough surfaces"]
float brdf_D_Beckmann(float roughness, float NoH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float NoH2 = NoH * NoH;

    return exp((NoH2 - 1.0) / (m2 * NoH2)) / (PI * m2 * NoH2 * NoH2);
}

// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float brdf_D_GGX(float roughness, float NoH)
{
    float m = roughness * roughness;
    float m2 = m * m;
    float d = (NoH * NoH) * (m2 - 1.0) + 1.0;

    return m2 / (PI * d * d);
}

vec3 brdf_F_None(vec3 specularColor)
{
    return specularColor;
}

// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
// [Lagarde 2012, "Spherical Gaussian approximation for Blinn-Phong, Phong and Fresnel"]
vec3 brdf_F_Schlick(vec3 specularColor, float VoH)
{
    float Fc = pow(1.0 - VoH, 5.0);

    return Fc + (1.0 - Fc) * specularColor;
}

vec3 brdf_F_Fresnel(vec3 specularColor, float VoH)
{
    vec3 specularColorSqrt = sqrt(clamp(vec3(0.0), vec3(0.99), specularColor));
    vec3 n = (1.0 + specularColorSqrt) / (1.0 - specularColorSqrt);
    vec3 g = sqrt(n * n + VoH * VoH - 1.0);

    return vec3(0.5 * square((g - VoH) / (g + VoH)) * (1.0 + square(((g + VoH) * VoH - 1.0) / ((g - VoH) * VoH + 1.0))));
}

float brdf_G_Implicit()
{
    return 0.25;
}

// [Neumann et al. 1999, "Compact metallic reflectance models"]
float brdf_G_Neumann(float NoV, float NoL)
{
    return 1.0 / (4.0 * max(NoL, NoV));
}
// [Kelemen 2001, "A microfacet based coupled specular-matte brdf model with importance sampling"]
float brdf_G_Kelemen(float VoH)
{
    return 1.0 / (4.0 * VoH * VoH);
}

// Tuned to match behavior of vis_Smith
// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
float brdf_G_Schlick(float roughness, float NoV, float NoL)
{
    float k = square(roughness) * 0.5;
    float vis_SchlickV = NoV * (1.0 - k) + k;
    float vis_SchlickL = NoL * (1.0 - k) + k;

    return 0.25 / (vis_SchlickV * vis_SchlickL);
}

// Smith term for GGX
// [Smith 1967, "Geometrical shadowing of a random rough surface"]
float brdf_G_Smith(float roughness, float NoV, float NoL)
{
    float a = square(roughness);
    float a2 = a*a;
    float vis_SmithV = NoV + sqrt(NoV * (NoV - NoV * a2) + a2);
    float vis_SmithL = NoL + sqrt(NoL * (NoL - NoL * a2) + a2);

    return 1.0 / (vis_SmithV * vis_SmithL);
}

vec3 brdf_diffuse(vec3 diffuseColor, float roughness, float NoV, float NoL, float VoH)
{
    #if BRDF_DIFFUSE == BRDF_DIFFUSE_LAMBERT
        return brdf_diffuseLambert(diffuseColor);
    #elif BRDF_DIFFUSE == BRDF_DIFFUSE_BURLEY
        return brdf_diffuseBurley(diffuseColor, roughness, NoV, NoL, VoH);
    #elif BRDF_DIFFUSE == BRDF_DIFFUSE_OREN_NAYAR
        return brdf_diffuseOrenNayar(diffuseColor, roughness, NoV, NoL, VoH);
    #endif
}

float brdf_distribution(float roughness, float NoH)
{
    #if BRDF_SPECULAR_D == BRDF_SPECULAR_D_BLINN
        return brdf_D_Blinn(roughness, NoH);
    #elif BRDF_SPECULAR_D == BRDF_SPECULAR_D_BECKMANN
        return brdf_D_Beckmann(roughness, NoH);
    #elif BRDF_SPECULAR_D == BRDF_SPECULAR_D_GGX
        return brdf_D_GGX(roughness, NoH);
    #endif
}

float brdf_geometricVisibility(float roughness, float NoV, float NoL, float VoH)
{
    #if BRDF_SPECULAR_G == BRDF_SPECULAR_G_IMPLICIT
        return brdf_G_Implicit();
    #elif BRDF_SPECULAR_G == BRDF_SPECULAR_G_NEUMANN
        return brdf_G_Neumann(NoV, NoL);
    #elif BRDF_SPECULAR_G == BRDF_SPECULAR_G_KELEMEN
        return brdf_G_Kelemen(VoH);
    #elif BRDF_SPECULAR_G == BRDF_SPECULAR_G_SMITH
        return brdf_G_Smith(roughness, NoV, NoL);
    #elif BRDF_SPECULAR_G == BRDF_SPECULAR_G_SCHLICK
        return brdf_G_Schlick(roughness, NoV, NoL);
    #endif
}

vec3 brdf_fresnel(vec3 specularColor, float VoH)
{
    #if BRDF_SPECULAR_F == BRDF_SPECULAR_F_NONE
        return brdf_F_None(specularColor);
    #elif BRDF_SPECULAR_F == BRDF_SPECULAR_F_SCHLICK
        return brdf_F_Schlick(specularColor, VoH);
    #elif BRDF_SPECULAR_F == BRDF_SPECULAR_F_FRESNEL
        return brdf_F_Fresnel(specularColor, VoH);
    #endif
}

#endif // _BRDF_FUNCTION_GLSL_
