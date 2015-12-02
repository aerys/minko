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

#ifdef SKINNING_NUM_BONES
# if SKINNING_NUM_BONES != 0
attribute vec4 aBoneWeightsA;
attribute vec4 aBoneWeightsB;
# endif
#endif

attribute float aMergingMask;

attribute float aPopProtected;

uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToViewMatrix;
uniform mat4 uPickingProjection;

uniform 	float 	uPopLod;
uniform 	float 	uPopBlendingLod;
uniform 	float 	uPopFullPrecisionLod;
uniform 	vec3 	uPopMinBound;
uniform 	vec3 	uPopMaxBound;

varying 	vec3 	vWorldPosition;
varying 	float 	vMergingMask;

void main(void)
{
	vMergingMask = 0.0;

	#if defined(VERTEX_MERGING_MASK)
		vMergingMask = aMergingMask;
	#endif

	#if defined(HAS_POSITION)
		vec4 pos = vec4(aPosition, 1.0);

		#ifdef SKINNING_NUM_BONES
		# if SKINNING_NUM_BONES != 0
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
	
		vWorldPosition = pos.xyz;

		gl_Position = uPickingProjection * (uWorldToViewMatrix * pos);
		
	#else //HAS_POSITION
		gl_Position = vec4(0.0, 0.0, 1.1, 1.0);
	#endif //HAS_POSITION
}

#endif // VERTEX_SHADER
