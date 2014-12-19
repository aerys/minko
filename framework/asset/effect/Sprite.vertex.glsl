#ifdef VERTEX_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

attribute vec3 	aPosition;

uniform mat4 	uModelToWorldMatrix;
uniform mat4	uViewMatrix;
uniform mat4	uProjectionMatrix;

#ifdef DIFFUSE_MAP
	attribute vec2 aUV;

	varying vec2 vUV;
#endif

void main()
{
	vec4 pos = uViewMatrix * uModelToWorldMatrix * vec4(0., 0., 0., 1.);
	vec3 scale = mat3(uModelToWorldMatrix) * vec3(1.0);

	pos += vec4(aPosition * length(scale), 0.);
	
	#ifdef DIFFUSE_MAP
		vUV = aUV;
	#endif

    gl_Position = uProjectionMatrix * pos;
}

#endif
