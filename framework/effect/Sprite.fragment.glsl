#ifdef FRAGMENT_SHADER

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
	#ifdef DIFFUSE_MAP
		gl_FragColor = texture2D(uDiffuseMap, vUV);
	#else
		gl_FragColor = uDiffuseColor;
	#endif
	
	#ifdef DIFFUSE_TINT
		gl_FragColor *= uDiffuseTint;
	#endif
}

#endif
