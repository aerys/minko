#ifdef FRAGMENT_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "Pack.function.glsl"

uniform vec3 uPickingOrigin;
uniform float uZFar;

varying vec3 vWorldPosition;
varying float vMergingMask;

void main(void)
{
    float distance = distance(uPickingOrigin, vWorldPosition);

    distance = clamp(distance / uZFar, 0.0, 1.0);

    vec4 color = vec4(packFloat8bitRGB(distance), 0.0);

    color.a = vMergingMask / 255.0;

    gl_FragColor = color;
}

#endif // FRAGMENT_SHADER
