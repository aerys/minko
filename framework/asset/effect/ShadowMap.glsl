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
uniform mat4 uWorldToScreenMatrix;

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

    vec4 screenPosition = uWorldToScreenMatrix * worldPosition;

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

float linearDepth(float depth, float zNear, float zFar)
{
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}


float linearDepthOrtho(float depth, float zNear, float zFar)
{
    // depth (depth - zNear) / (zFar - zNear);
    // return zNear + depth * (zFar - zNear)
    return depth * 0.5 + 0.5;
}

void main(void)
{
    // gl_FragColor = pack(linearDepthOrtho(vPosition.z, uZNear, uZFar));
    float depth = linearDepthOrtho(vPosition.z, uZNear, uZFar);
    // float c = 10.0;

    gl_FragColor = packFloat8bitRGBA(depth);
}
#endif
