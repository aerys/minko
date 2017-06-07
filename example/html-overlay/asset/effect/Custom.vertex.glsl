#ifdef VERTEX_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

attribute vec3 aPosition;
attribute vec2 aUV;

uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToScreenMatrix;

varying vec2 vVertexUV;

void main(void)
{
    #if defined(VERTEX_UV) && defined(DIFFUSE_MAP)
        vVertexUV = aUV;
    #endif

    vec4 pos = vec4(aPosition, 1.0);

    #ifdef MODEL_TO_WORLD
        pos = uModelToWorldMatrix * pos;
    #endif

    gl_Position = uWorldToScreenMatrix * pos;
}

#endif // VERTEX_SHADER
