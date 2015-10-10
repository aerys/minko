#ifdef VERTEX_SHADER

#ifdef GL_ES
# ifdef MINKO_PLATFORM_IOS
    precision highp float;
# else
    precision mediump float;
# endif
#endif

#pragma include "Skinning.function.glsl"

attribute vec3 aPosition;

uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToScreenMatrix[4];

varying vec4 vPosition;

void main(void)
{
    vec4 worldPosition = vec4(aPosition, 1.0);

    #ifdef NUM_BONES
        worldPosition = skinning_moveVertex(worldPosition);
    #endif // NUM_BONES

    #ifdef MODEL_TO_WORLD
        worldPosition = uModelToWorldMatrix * worldPosition;
    #endif // MODEL_TO_WORLD

    vec4 screenPosition = uWorldToScreenMatrix[SHADOW_CASCADE_INDEX] * worldPosition;

    vPosition = screenPosition;

    gl_Position = screenPosition;
}
#endif

#ifdef FRAGMENT_SHADER

#ifdef GL_ES
# ifdef MINKO_PLATFORM_IOS
    precision highp float;
# else
    precision mediump float;
# endif
#endif

#pragma include "Pack.function.glsl"

uniform float uZNear;
uniform float uZFar;

varying vec4 vPosition;

void main(void)
{
    gl_FragColor = packFloat8bitRGBA(vPosition.z * 0.5 + 0.5);
}
#endif
