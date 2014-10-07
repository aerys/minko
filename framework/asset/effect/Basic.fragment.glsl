#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include "Fog.function.glsl"

uniform vec4        diffuseColor;
uniform sampler2D   diffuseMap;
uniform samplerCube	diffuseCubeMap;

// alpha
uniform sampler2D 	alphaMap;
uniform float 		alphaThreshold;

varying vec2 vVertexUV;
varying vec3 vVertexUVW;

void main(void)
{
	vec4 diffuse = diffuseColor;

	#if defined(DIFFUSE_CUBEMAP)
		diffuse	= textureCube(diffuseCubeMap, vVertexUVW);
	#elif defined(DIFFUSE_MAP)
		diffuse = texture2D(diffuseMap, vVertexUV);
	#endif

	#ifdef ALPHA_MAP
		diffuse.a = texture2D(alphaMap, vVertexUV).r;
	#endif // ALPHA_MAP

	#ifdef ALPHA_THRESHOLD
		if (diffuse.a < alphaThreshold)
			discard;
	#endif // ALPHA_THRESHOLD

	#ifdef FOG_ENABLED
		diffuse = fog_sampleFog(diffuse, gl_FragCoord);
	#endif // FOG_ENABLED

	gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER
