#ifdef FRAGMENT_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

uniform vec4 uDiffuseColor;

void
main()
{
	vec4 color = vec4(1.0);

	#ifdef DIFFUSE_COLOR
		color = uDiffuseColor;
	#endif // DIFFUSE_COLOR

	gl_FragColor = color;
}

#endif // FRAGMENT_SHADER
