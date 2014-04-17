#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4        diffuseColor;
uniform sampler2D   diffuseMap;
uniform samplerCube	diffuseCubeMap;

uniform bool useDiscard;

varying vec3 vPosition;

varying vec2 vertexUV;
varying vec3 vertexUVW;

void main(void)
{
	#if defined(DIFFUSE_CUBEMAP)
		gl_FragColor	= textureCube(diffuseCubeMap, vertexUVW);
	#elif defined(DIFFUSE_MAP)
		gl_FragColor 	= texture2D(diffuseMap, vertexUV);
	#else
		gl_FragColor 	= diffuseColor;
	#endif

	if (vPosition.y < 0)
		discard;
}

#endif // FRAGMENT_SHADER
