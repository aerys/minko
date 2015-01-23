#if __VERSION__ > 120 || defined(GL_OES_standard_derivatives)

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

vec4 texturelod_texture2D(sampler2D tex, vec2 uv, vec2 texSize, float baseLod, float maxLod, vec4 defaultColor)
{
#if __VERSION__ <= 120
    #if defined(GL_OES_standard_derivatives) && defined(GL_EXT_shader_texture_lod)
        float requiredLod = texturelod_mipmapLevel(tex, uv, texSize);

        float maxTextureLod = floor(log2(texSize.x));
    
        if (maxLod >= maxTextureLod)
            return defaultColor;

        return texture2DLodEXT(tex, fract(uv), max(maxLod, requiredLod));
    #else
        return defaultColor;
    #endif
#else
    float requiredLod = texturelod_mipmapLevel(tex, uv, texSize);

    float maxTextureLod = floor(log2(texSize.x));

    if (maxLod >= maxTextureLod)
        return defaultColor;

    return textureLod(tex, fract(uv), max(maxLod, requiredLod));
#endif
}
