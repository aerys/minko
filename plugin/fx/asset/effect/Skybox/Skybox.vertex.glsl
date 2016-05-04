#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

attribute vec3 aPosition;

uniform float uZFar;
uniform mat4 uWorldToScreenMatrix;
uniform vec3 uEyePosition;

varying vec3 vDirection;

void main()
{
    vec3 pos = normalize(aPosition) * uZFar + uEyePosition;

    vDirection = aPosition;

    gl_Position = uWorldToScreenMatrix * vec4(pos, 1.0);
}
