#ifdef FRAGMENT_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif


void main(void)
{
    vec4 diffuse = vec4(1.0, 0.0, 0.0, 1.0);

    gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER
