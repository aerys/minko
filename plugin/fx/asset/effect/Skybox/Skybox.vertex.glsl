#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "Math.function.glsl"

attribute vec3 aPosition;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

varying vec3 vDirection;

void main() {
    float w = 10000.0;

    vec4 position = vec4(aPosition, 0.5) * vec4(w, -w, w, w);
    mat4 inverseProjection = inverse(uProjectionMatrix);
    mat3 inverseModelview = transpose(mat3(uViewMatrix));
    vec3 unprojected = (inverseProjection * position).xyz;
    vDirection = inverseModelview * unprojected;

    position.z = position.w - EPSILON;

    gl_Position = position;
}