#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4        pickingColor;

void main(void)
{
#ifndef PICKING_COLOR
	discard;
#else
	gl_FragColor = pickingColor;
#endif
}

#endif // FRAGMENT_SHADER
