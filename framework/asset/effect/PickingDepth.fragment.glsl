#ifdef FRAGMENT_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "Pack.function.glsl"

uniform vec3 uEyePosition;
uniform float uZFar;

varying vec3 vWorldPosition;

void main(void)
{
    float distance = distance(uEyePosition, vWorldPosition);

    distance = clamp(distance / uZFar, 0.0, 1.0);

    gl_FragColor = packFloat8bitRGBA(distance);
}

#endif // FRAGMENT_SHADER
