#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("Fog.function.glsl")

uniform vec4        diffuseColor;
uniform sampler2D   diffuseMap;
uniform samplerCube	diffuseCubeMap;

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
	
	gl_FragColor = fog_sampleFog(gl_FragColor, gl_FragCoord);
}

#endif // FRAGMENT_SHADER
