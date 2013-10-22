#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4 uDiffuseColor;

#ifdef DIFFUSE_MAP
	uniform sampler2D uDiffuseMap;
	
	varying vec2 vUV;
#endif

#ifdef DIFFUSE_TINT
	uniform vec4 uDiffuseTint;
#endif

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
