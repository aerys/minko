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
#ifdef CLIPPING_PLANE_0
uniform		vec4	uClippingPlane0;
#endif
#ifdef CLIPPING_PLANE_1
uniform		vec4	uClippingPlane1;
#endif
#ifdef CLIPPING_PLANE_2
uniform		vec4	uClippingPlane2;
#endif

varying vec2 vVertexUV;
varying vec3 vVertexUVW;
varying vec4 vVertexScreenPosition;
varying vec3 vPosition;
#ifdef CLIPPING_PLANE_0
varying float clipDist0;
#endif
#ifdef CLIPPING_PLANE_1
varying float clipDist1;
#endif
#ifdef CLIPPING_PLANE_2
varying float clipDist2;
#endif

void main(void)
{
	#if defined(VERTEX_UV)
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

	vPosition = pos.xyz;

#ifdef CLIPPING_PLANE_0
	// Compute the distance between the vertex and the clip plane
	clipDist0 = dot(pos.xyz, uClippingPlane0.xyz) - uClippingPlane0.w;
#endif
#ifdef CLIPPING_PLANE_1
	// Compute the distance between the vertex and the clip plane
	clipDist1 = dot(pos.xyz, uClippingPlane1.xyz) - uClippingPlane1.w;
#endif
#ifdef CLIPPING_PLANE_2
	// Compute the distance between the vertex and the clip plane
	clipDist2 = dot(pos.xyz, uClippingPlane2.xyz) - uClippingPlane2.w;
#endif
}

#endif // VERTEX_SHADER
