#ifdef VERTEX_SHADER

#ifdef GL_ES
# ifdef GL_VERTEX_PRECISION_HIGH
    precision highp float;
# else
    precision mediump float;
# endif
#endif

attribute vec3 aPosition;

uniform vec2 uPosition;
uniform vec2 uSize;
uniform vec4 uViewport;

varying vec2 vUV;

void main(void)
{
    vec2 s = uSize * 2.0 / uViewport.zw;
    vec2 pos = (aPosition.xy * s) + vec2(-1.0 + s.x * 0.5, 1.0 - s.y * 0.5);

    pos = vec2(uPosition.x, -uPosition.y) / uViewport.zw + pos;
    vUV = aPosition.xy + vec2(0.5);

    gl_Position = vec4(pos, 0.0, 1.0);
}

#endif



#ifdef FRAGMENT_SHADER

#ifdef GL_ES
# ifdef GL_FRAGMENT_PRECISION_HIGH
    precision highp float;
# else
    precision mediump float;
# endif
#endif

varying vec2 vUV;

uniform sampler2D uTexture;

void main(void)
{
    gl_FragColor = texture2D(uTexture, vUV);
}

#endif
