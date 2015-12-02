#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "Sobel.function.glsl"

uniform sampler2D uTextureSampler;
uniform sampler2D uDepthTextureSampler;
uniform float uBorderThickness;
uniform vec3 uBorderColor;
uniform vec2 uTexcoordOffset;

varying vec2 vUV;

void main(void)
{
    float step = uTexcoordOffset.x;
    bool sobel = sobel(uDepthTextureSampler, vUV, step, uBorderThickness);

    if (sobel)
        gl_FragColor = vec4(uBorderColor, 1.0);
    else
        gl_FragColor = texture2D(uTextureSampler, vUV);
}