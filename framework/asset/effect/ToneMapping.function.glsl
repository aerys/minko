#pragma include "Math.function.glsl"

#define TONE_MAPPING_TECHNIQUE_LINEAR           1
#define TONE_MAPPING_TECHNIQUE_REINHARD         2
#define TONE_MAPPING_TECHNIQUE_UNCHARTED2       3
#define TONE_MAPPING_TECHNIQUE_OPTIMIZEDCINEON  4

#ifndef TONE_MAPPING_TECHNIQUE
# define TONE_MAPPING_TECHNIQUE TONE_MAPPING_TECHNIQUE_UNCHARTED2
#endif

vec3 toneMapping_linear(vec3 color, float exposure)
{
    return exposure * color;
}

vec3 toneMapping_reinhard(vec3 color, float exposure)
{
    color *= exposure;

    return saturate(color / (vec3(1.0) + color));
}

#define Uncharted2Helper(x) max(((x * (0.15 * x + 0.10 * 0.50) + 0.20 * 0.02) / (x * (0.15 * x + 0.50) + 0.20 * 0.30 )) - 0.02 / 0.30, vec3(0.0))

vec3 toneMapping_uncharted2(vec3 color, float exposure)
{
    color *= exposure;

    return saturate(Uncharted2Helper(color) / Uncharted2Helper(vec3(1.0)));
}

vec3 toneMapping_optimizedCineon(vec3 color, float exposure)
{
    color *= exposure;
    color = max(vec3(0.0), color - 0.004);

    return pow((color * (6.2 * color + 0.5)) / (color * (6.2* color + 1.7) + 0.06), vec3(2.2));
}

vec3 toneMapping_toneMap(vec3 color, float exposure)
{
#if TONE_MAPPING_TECHNIQUE == TONE_MAPPING_TECHNIQUE_LINEAR
    return toneMapping_linear(color);
#elif TONE_MAPPING_TECHNIQUE == TONE_MAPPING_TECHNIQUE_REINHARD
    return toneMapping_reinhard(color, exposure);
#elif TONE_MAPPING_TECHNIQUE == TONE_MAPPING_TECHNIQUE_UNCHARTED2
    return toneMapping_uncharted2(color, exposure);
#elif TONE_MAPPING_TECHNIQUE == TONE_MAPPING_TECHNIQUE_OPTIMIZEDCINEON
    return toneMapping_optimizedCineon(color, exposure);
#endif
}
