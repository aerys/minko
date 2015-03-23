#ifdef FRAGMENT_SHADER

#pragma include "TextureLod.extension.glsl"

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "Fog.function.glsl"
#pragma include "TextureLod.function.glsl"

uniform vec4 uDiffuseColor;
uniform sampler2D uDiffuseMap;
uniform samplerCube	uDiffuseCubeMap;

// alpha
uniform sampler2D uAlphaMap;
uniform float uAlphaThreshold;

// fog
uniform vec4 uFogColor;
uniform vec2 uFogBounds;

// texture lod
uniform float uDiffuseMapMaxAvailableLod;
uniform vec2 uDiffuseMapSize;

varying vec2 vVertexUV;
varying vec3 vVertexUVW;
varying vec4 vVertexScreenPosition;

void main(void)
{
	vec4 diffuse = uDiffuseColor;

	#if defined(VERTEX_UV)
		#if defined(DIFFUSE_CUBEMAP)
			diffuse	= textureCube(uDiffuseCubeMap, vVertexUVW);
		#elif defined(DIFFUSE_MAP)
			#ifdef DIFFUSE_MAP_LOD
				diffuse = texturelod_texture2D(uDiffuseMap, vVertexUV, uDiffuseMapSize, 0.0, uDiffuseMapMaxAvailableLod, diffuse);
			#else
				diffuse = texture2D(uDiffuseMap, vVertexUV);
			#endif
		#endif
	#endif // VERTEX_UV

	#if defined(VERTEX_UV) && defined(ALPHA_MAP)
		diffuse.a = texture2D(uAlphaMap, vVertexUV).r;
	#endif // VERTEX_UV && ALPHA_MAP

	#ifdef ALPHA_THRESHOLD
		if (diffuse.a < uAlphaThreshold)
			discard;
	#endif // ALPHA_THRESHOLD


	#ifdef FOG_TECHNIQUE
		diffuse.rgb = fog_sampleFog(diffuse.rgb, vVertexScreenPosition.z, uFogColor.xyz, uFogColor.a, uFogBounds.x, uFogBounds.y);
	#endif

	gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER
