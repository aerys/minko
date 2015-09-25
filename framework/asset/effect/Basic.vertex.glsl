#ifdef VERTEX_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "Skinning.function.glsl"
#pragma include "Pop.function.glsl"

attribute vec3 aPosition;
attribute vec2 aUV;

#ifdef VERTEX_UV1
attribute vec2 aUV1;
#endif

#ifdef SKINNING_NUM_BONES
# if SKINNING_NUM_BONES != 0
attribute vec4 aBoneWeightsA;
attribute vec4 aBoneWeightsB;
# endif
#endif

#ifdef VERTEX_POP_PROTECTED
attribute float aPopProtected;
#endif

uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToScreenMatrix;

#ifdef UV_SCALE
uniform vec2 uUVScale;
#endif
#ifdef UV_OFFSET
uniform vec2 uUVOffset;
#endif

#ifdef POP_LOD_ENABLED
uniform float uPopLod;
#ifdef POP_BLENDING_ENABLED
uniform float uPopBlendingLod;
#endif
uniform float uPopFullPrecisionLod;
uniform vec3 uPopMinBound;
uniform vec3 uPopMaxBound;
#endif

varying vec2 vVertexUV;
varying vec2 vVertexUV1;
varying vec3 vVertexUVW;
varying vec4 vVertexScreenPosition;

void main(void)
{
	#if defined(VERTEX_UV) && (defined(DIFFUSE_MAP) || defined(ALPHA_MAP) || (defined(LIGHT_MAP) && !defined(VERTEX_UV1)))
        vec2 uvScale = vec2(1.0);
        vec2 uvOffset = vec2(0.0);

        #ifdef UV_SCALE
            uvScale = uUVScale;
        #endif // UV_SCALE

        #ifdef UV_OFFSET
            uvOffset = uUVOffset;
        #endif // UV_OFFSET

		vec2 uv = aUV;

		#if defined(UV_SCALE)
			uv *= uvScale;
		#endif

		#if defined(UV_OFFSET)
			uv += uvOffset;
		#endif

		vVertexUV = uv;
	#endif

    #if defined (VERTEX_UV1) && defined (LIGHT_MAP)
        vVertexUV1 = aUV1;
    #endif // VERTEX_UV1 && LIGHT_MAP

	#ifdef DIFFUSE_CUBEMAP
		vVertexUVW = aPosition;
	#endif

	vec4 pos = vec4(aPosition, 1.0);

	#ifdef SKINNING_NUM_BONES
        #if SKINNING_NUM_BONES != 0
		  pos = skinning_moveVertex(pos, aBoneWeightsA, aBoneWeightsB);
        #endif
	#endif

    #ifdef POP_LOD_ENABLED
        float popProtected = 0.0;

        #ifdef VERTEX_POP_PROTECTED
            popProtected = aPopProtected;
        #endif // VERTEX_POP_PROTECTED

        #ifdef POP_BLENDING_ENABLED
            pos = pop_blend(pos, vec3(0.0), uPopLod, uPopBlendingLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound, popProtected);
        #else
            pos = pop_quantize(pos, vec3(0.0), uPopLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound, popProtected);
        #endif // POP_BLENDING_ENABLED
    #endif // POP_LOD_ENABLED

	#ifdef MODEL_TO_WORLD
		pos = uModelToWorldMatrix * pos;
	#endif

    vec4 screenPos = uWorldToScreenMatrix * pos;

    vVertexScreenPosition = screenPos;
	gl_Position = screenPos;
}

#endif // VERTEX_SHADER
