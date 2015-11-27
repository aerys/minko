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
#endif
#endif

attribute float aPopProtected;

#ifdef MODEL_TO_WORLD
uniform mat4 uModelToWorldMatrix;
#endif
uniform mat4 uWorldToViewMatrix;
uniform mat4 uPickingProjection;

#ifdef POP_LOD_ENABLED
uniform 	float 	uPopLod;
#ifdef POP_BLENDING_ENABLED
uniform 	float 	uPopBlendingLod;
#endif
uniform 	float 	uPopFullPrecisionLod;
uniform 	vec3 	uPopMinBound;
uniform 	vec3 	uPopMaxBound;
#endif

void main(void)
{
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
	
		gl_Position = uPickingProjection * (uWorldToViewMatrix * pos);
		
	#else //HAS_POSITION
		gl_Position = vec4(0.0, 0.0, 1.1, 1.0);
	#endif //HAS_POSITION
}

#endif // VERTEX_SHADER
