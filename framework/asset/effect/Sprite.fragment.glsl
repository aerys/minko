#ifdef FRAGMENT_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

uniform vec4 uDiffuseColor;
uniform sampler2D uDiffuseMap;
uniform vec4 uDiffuseTint;

varying vec2 vUV;

void main()
{
	vec4 diffuse = vec4(0.);

	#ifdef DIFFUSE_MAP
		diffuse = texture2D(uDiffuseMap, vUV);
	#else
		diffuse = uDiffuseColor;
	#endif

	#ifdef DIFFUSE_TINT
		diffuse *= uDiffuseTint;
	#endif

	gl_FragColor = diffuse;
}

#endif
