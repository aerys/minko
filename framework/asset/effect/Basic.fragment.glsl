#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include "Fog.function.glsl"

uniform vec4 uDiffuseColor;
uniform sampler2D uDiffuseMap;
uniform samplerCube	uDiffuseCubeMap;

// alpha
uniform sampler2D uAlphaMap;
uniform float uAlphaThreshold;

varying vec2 vVertexUV;
varying vec3 vVertexUVW;

void main(void)
{
	vec4 diffuse = uDiffuseColor;

	#if defined(DIFFUSE_CUBEMAP)
		diffuse	= textureCube(uDiffuseCubeMap, vVertexUVW);
	#elif defined(DIFFUSE_MAP)
		diffuse = texture2D(uDiffuseMap, vVertexUV);
	#endif

	#ifdef ALPHA_MAP
		diffuse.a = texture2D(uAlphaMap, vVertexUV).r;
	#endif // ALPHA_MAP

	#ifdef ALPHA_THRESHOLD
		if (diffuse.a < uAlphaThreshold)
			discard;
	#endif // ALPHA_THRESHOLD

	#ifdef FOG_ENABLED
		diffuse = fog_sampleFog(diffuse, gl_FragCoord);
	#endif // FOG_ENABLED

	gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER
