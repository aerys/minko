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

#ifdef SKINNING_NUM_BONES
attribute vec4 aBoneWeightsA;
attribute vec4 aBoneWeightsB;
#endif

attribute float aPopProtected;

uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToScreenMatrix;
uniform vec2 uUVScale;
uniform vec2 uUVOffset;

uniform float uPopLod;
uniform float uPopBlendingLod;
uniform float uPopFullPrecisionLod;
uniform vec3 uPopMinBound;
uniform vec3 uPopMaxBound;

varying vec2 vVertexUV;
varying vec3 vVertexUVW;
varying vec4 vVertexScreenPosition;

void main(void)
{
	#if defined(VERTEX_UV) && (defined(DIFFUSE_MAP) || defined(ALPHA_MAP))
		vec2 uv = aUV;

		#if defined(UV_SCALE)
			uv *= uUVScale;
		#endif

		#if defined(UV_OFFSET)
			uv += uUVOffset;
		#endif

		vVertexUV = uv;
	#endif

	#ifdef DIFFUSE_CUBEMAP
		vVertexUVW = aPosition;
	#endif

	vec4 pos = vec4(aPosition, 1.0);

	#ifdef SKINNING_NUM_BONES
		pos = skinning_moveVertex(pos, aBoneWeightsA, aBoneWeightsB);
	#endif

    #ifdef POP_LOD_ENABLED
        float popProtected = 0.0;

        #ifdef VERTEX_POP_PROTECTED
            popProtected = aPopProtected;
        #endif // VERTEX_POP_PROTECTED

        #ifdef POP_BLENDING_ENABLED
            pos = pop_blend(pos, vec3(0.0), uPopLod, uPopBlendingLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound);
        #else
            pos = pop_quantize(pos, vec3(0.0), uPopLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound);
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
