#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

void main(void)
{
	gl_FragColor = vec4(1, 1, 1, 1);
}

#endif // FRAGMENT_SHADER
