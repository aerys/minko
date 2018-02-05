#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "Math.function.glsl"

attribute vec3 aPosition;

uniform mat4 uViewMatrix;

uniform float uZNear;
uniform float uZFar;
uniform float uAspectRatio;
uniform float uFov;

varying vec3 vDirection;

void main() {
    float zpos = (uZNear + uZFar) * 0.5;
    float height = tan(uFov * 0.5) * zpos;
    float width = height * uAspectRatio;

    vec4 position = vec4(aPosition.xy, 1.0, 1.0) * vec4(2.0, -2.0, 1.0, 1.0);
    mat3 inverseModelview = transpose(mat3(uViewMatrix));
    vDirection = inverseModelview * (position.xyz * vec3(width, height, -zpos)); 

    position.z =  (1000.0 - EPSILON) / 1000.0;

    gl_Position = position;
}