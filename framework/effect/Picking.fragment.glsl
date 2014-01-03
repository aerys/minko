#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4        pickingColor;

void main(void)
{
	gl_FragColor = pickingColor;
}

#endif // FRAGMENT_SHADER
