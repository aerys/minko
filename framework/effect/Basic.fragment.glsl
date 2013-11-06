#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4        diffuseColor;
uniform sampler2D   diffuseMap;

varying vec2 vertexUV;

void main(void)
{
	#ifdef DIFFUSE_MAP
		gl_FragColor = texture2D(diffuseMap, vertexUV);
	#else
		gl_FragColor = diffuseColor;
	#endif
}

#endif // FRAGMENT_SHADER
