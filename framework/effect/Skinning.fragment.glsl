#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4        diffuseColor;
uniform sampler2D   diffuseMap;

varying vec2 		vertexUV;
varying	vec3		vertexNormal;

void main(void)
{
	#ifdef HAS_NORMAL
		vec3	normalVector = normalize(vertexNormal);
		
		gl_FragColor = vec4(normalVector * 0.5 + vec3(0.5), 1.0);
		return;
	#endif // HAS_NORMAL

	#ifdef NUM_BONES
		gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	#else
		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	#endif // NUM_BONES
	return;

	#ifdef DIFFUSE_MAP
		gl_FragColor = texture2D(diffuseMap, vertexUV);
	#else
		gl_FragColor = diffuseColor;
	#endif
}

#endif // FRAGMENT_SHADER
