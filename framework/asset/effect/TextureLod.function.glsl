#if __VERSION__ > 120 || defined(GL_OES_standard_derivatives)

float texturelod_mipmapLevel(vec2 uv, vec2 texSize)
{
  vec2 dx = dFdx(uv * texSize.x);
  vec2 dy = dFdy(uv * texSize.y);
  
  float d = max(dot (dx, dx), dot(dy, dy));
  
  return 0.5 * log2(d);
}

#endif

vec4 texturelod_texture2D(sampler2D tex, vec2 uv, vec2 texSize, float baseLevel, float maxLevel, vec4 defaultColor)
{
#if __VERSION__ <= 120
    #if defined(GL_OES_standard_derivatives) && defined(GL_EXT_shader_texture_lod)
        float level = max(texturelod_mipmapLevel(uv, texSize), maxLevel);

        return texture2DLodEXT(tex, fract(uv), level);
    #else
        return defaultColor;
    #endif
#else
    float level = max(texturelod_mipmapLevel(uv, texSize), maxLevel);

    return textureLod(tex, fract(uv), level);
#endif
}
