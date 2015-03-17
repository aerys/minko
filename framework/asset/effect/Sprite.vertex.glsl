#ifdef VERTEX_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

attribute vec3 aPosition;
attribute vec2 aUV;

uniform mat4 uModelToWorldMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

varying vec2 vUV;

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
