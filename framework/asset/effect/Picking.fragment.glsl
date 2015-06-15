#ifdef FRAGMENT_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

uniform vec4 uPickingColor;

void main(void)
{
#ifndef PICKING_COLOR
	discard;
#else
	gl_FragColor = uPickingColor;
#endif
}

#endif // FRAGMENT_SHADER
