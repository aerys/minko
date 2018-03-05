#ifdef FRAGMENT_SHADER

// See https://developer.android.com/reference/android/graphics/SurfaceTexture.html
#extension GL_OES_EGL_image_external : require

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#ifdef DIFFUSE_MAP
uniform samplerExternalOES uDiffuseMap;
#endif

#ifdef VERTEX_UV
varying vec2 vVertexUV;
#endif

void main(void)
{
    vec4 diffuse = vec4(1.0, 0.0, 0.0, 0.0);

    #if defined(VERTEX_UV) && defined(DIFFUSE_MAP)
        diffuse = texture2D(uDiffuseMap, vVertexUV);
    #endif

    gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER
