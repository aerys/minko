#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4        diffuseColor;
uniform sampler2D   diffuseMap;
uniform samplerCube	diffuseCubeMap;

uniform vec4		fogColor;
uniform float		fogDensity;

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

	#if defined(FOG)
		float fragDist = gl_FragCoord.z / gl_FragCoord.w;

		const float LOG2 = 1.442695;

		float fogFactor = clamp(exp2(-fogDensity * fogDensity * fragDist * fragDist * LOG2), 0.0f, 1.0f);

		gl_FragColor = mix(fogColor, gl_FragColor, fogFactor);
	#endif
}

#endif // FRAGMENT_SHADER
