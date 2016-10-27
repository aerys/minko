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
    vec4 position = vec4(aPosition, 1.0) * vec4(1.0, -1.0, 1.0, 0.5);
    mat4 inverseProjection = inverse(uProjectionMatrix);
    mat3 inverseModelview = transpose(mat3(uViewMatrix));
    vec3 unprojected = (inverseProjection * position).xyz;
    vDirection = inverseModelview * unprojected;

    position.z =  position.w * ((1000.0 - EPSILON) / 1000.0);

    gl_Position = position;
}