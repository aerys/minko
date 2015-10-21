#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

attribute vec3 aPosition;

varying vec2 vUV;

void main(void)
{
    vec4 position = vec4(aPosition, 1) * vec4(1., -1., 1., 0.5);
    vUV = position.xy + 0.5;

    gl_Position = position;
}