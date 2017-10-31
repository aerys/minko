#ifdef FRAGMENT_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

uniform vec4 uPickingColor;

varying float vMergingMask;

void main(void)
{
#ifndef PICKING_COLOR
	discard;
#else

	vec4 color = uPickingColor;
    color.a = vMergingMask / 255.0;

	gl_FragColor = color;
#endif
}

#endif // FRAGMENT_SHADER
