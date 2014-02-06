#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

uniform 	vec4 	diffuseColor;

void
main()
{
	vec4 color		= vec4(1.0);
	
	#ifdef DIFFUSE_COLOR
		color		= diffuseColor;
	#endif // DIFFUSE_COLOR
	
	gl_FragColor	= color;
}

#endif // FRAGMENT_SHADER
