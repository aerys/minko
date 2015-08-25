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
#pragma include "LightMapping.function.glsl"

uniform vec4 uDiffuseColor;

#ifdef DIFFUSE_MAP
uniform sampler2D uDiffuseMap;
#endif

#ifdef LIGHT_MAP
uniform sampler2D uLightMap;
#endif

#ifdef DIFFUSE_CUBEMAP
uniform samplerCube	uDiffuseCubeMap;
#endif

// alpha
#ifdef ALPHA_MAP
uniform sampler2D uAlphaMap;
#endif

#ifdef ALPHA_THRESHOLD
uniform float uAlphaThreshold;
#endif

// fog
#ifdef FOG_TECHNIQUE
uniform vec4 uFogColor;
uniform vec2 uFogBounds;
#endif

// texture lod
#ifdef DIFFUSE_MAP_LOD
uniform float uDiffuseMapMaxAvailableLod;
uniform vec2 uDiffuseMapSize;
#endif

#ifdef LIGHT_MAP_LOD
uniform float uLightMapMaxAvailableLod;
uniform vec2 uLightMapSize;
#endif

varying vec2 vVertexUV;
varying vec2 vVertexUV1;
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

    #if (defined (VERTEX_UV) || defined(VERTEX_UV1)) && defined(LIGHT_MAP)
        vec2 lightMapUV = vec2(0.0);

        #ifdef VERTEX_UV1
            lightMapUV = vVertexUV1;
        #else
            lightMapUV = vVertexUV;
        #endif

        #ifdef LIGHT_MAP_LOD
            diffuse = lightMapping_multiply(diffuse, texturelod_texture2D(uLightMap, lightMapUV, uLightMapSize, 0.0, uLightMapMaxAvailableLod, vec4(1.0)));
        #else
            diffuse = lightMapping_multiply(diffuse, texture2D(uLightMap, lightMapUV));
        #endif
    #endif // (VERTEX_UV || VERTEX_UV1) && LIGHT_MAP

	#ifdef FOG_TECHNIQUE
		diffuse.rgb = fog_sampleFog(diffuse.rgb, vVertexScreenPosition.z, uFogColor.xyz, uFogColor.a, uFogBounds.x, uFogBounds.y);
	#endif

	gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER
