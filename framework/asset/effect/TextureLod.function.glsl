#ifndef _TEXTURELOD_FUNCTION_GLSL_
#define _TEXTURELOD_FUNCTION_GLSL_

#if __VERSION__ >= 110 || defined GL_OES_standard_derivatives

float texturelod_mipmapLevel(sampler2D tex, vec2 uv, vec2 texSize)
{
#if __VERSION__ >= 400
    return textureQueryLod(tex, uv).x;
#else
    vec2 dx = dFdx(uv * texSize.x);
    vec2 dy = dFdy(uv * texSize.y);

    float d = max(dot(dx, dx), dot(dy, dy));

    return max(0.5 * log2(d), 0.0);
#endif
}

#endif

vec4 texturelod_texture(sampler2D tex, vec2 uv, float lod)
{
#if __VERSION__ < 130
    #if defined GL_OES_standard_derivatives && (defined GL_ES && defined GL_EXT_shader_texture_lod) || (!defined GL_ES && defined GL_ARB_shader_texture_lod)
        #if defined GL_ES
            return texture2DLodEXT(tex, uv, lod);
        #else
            return texture2DLod(tex, uv, lod);
        #endif
    #else
        return vec4(0.0);
    #endif
#else
    return textureLod(tex, uv, lod);
#endif
}

vec4 texturelod_texture2D(sampler2D tex, vec2 uv, vec2 texSize, float baseLod, float maxLod, vec4 defaultColor)
{
    if (maxLod == baseLod)
        return texture2D(tex, uv);

#if __VERSION__ < 130
    #if defined GL_OES_standard_derivatives
        float requiredLod = texturelod_mipmapLevel(tex, uv, texSize);
        float maxTextureLod = floor(log2(texSize.x));

        if (maxLod >= maxTextureLod)
            return defaultColor;

        #if defined GL_ES && defined GL_EXT_shader_texture_lod
            // WebGL 1.0 (html5)
            return texture2DLodEXT(tex, fract(uv), max(maxLod, requiredLod));
        #elif defined GL_ES
            // OpenGL ES 2.0 (android, ios)
            // Unlike `texture2DLod*` functions, `texture2D` accepts
            // a LOD bias. We thus subtract the `requiredLod` to the result.
            return texture2D(tex, fract(uv), max(maxLod, requiredLod) - requiredLod);
        #elif defined GL_ARB_shader_texture_lod
            // Desktop
            return texture2DLod(tex, fract(uv), max(maxLod, requiredLod));
        #endif
    #else
        return defaultColor;
    #endif
#else
    float requiredLod = texturelod_mipmapLevel(tex, uv, texSize);

    float maxTextureLod = floor(log2(texSize.x));

    if (maxLod >= maxTextureLod)
        return defaultColor;

    return textureLod(tex, uv, max(maxLod, requiredLod));
#endif
}

vec4 texturelod_mix(sampler2D   tex,
                    vec2        uv,
                    vec2        texSize,
                    float       maxLod,
                    float       mixStartLod,
                    float       mixStartTime,
                    float       mixPeriod,
                    float       time,
                    vec4        defaultColor)
{
    float maxTextureLod = floor(log2(texSize.x));

    if (maxLod >= maxTextureLod)
        return defaultColor;

    float lodDistance = mixStartLod - maxLod;
    float mixAlpha = clamp((time - mixStartTime) / (mixPeriod * lodDistance), 0.0, 1.0);
    float mixedMaxLod = mix(mixStartLod, maxLod, mixAlpha);

    return texturelod_texture2D(tex, uv, texSize, 0.0, mixedMaxLod, defaultColor);
}

#endif
