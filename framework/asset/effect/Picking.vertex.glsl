#ifdef VERTEX_SHADER

#ifdef GL_FRAGMENT_PRECISION_HIGH
    precision highp float;
#else
    precision mediump float;
#endif

#pragma include "Skinning.function.glsl"

attribute vec3 aPosition;
attribute vec2 aUV;

#ifdef SKINNING_NUM_BONES
attribute vec4 aBoneIdsA;
attribute vec4 aBoneIdsB;
attribute vec4 aBoneWeightsA;
attribute vec4 aBoneWeightsB;
uniform mat4 uBoneMatrices[SKINNING_NUM_BONES];
#endif

uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToViewMatrix;
uniform mat4 uPickingProjection;

varying vec2 vertexUV;

void main(void)
{
	#ifdef DIFFUSE_MAP
		vertexUV = aUV;
	#endif
	
	#if defined(HAS_POSITION)
		vec4 pos = vec4(aPosition, 1.0);

		#ifdef NUM_BONES
			pos = skinning_moveVertex(pos);
		#endif // NUM_BONES

		#ifdef SKINNING_NUM_BONES
			pos = skinning_moveVertex(pos, uBoneMatrices, aBoneIdsA, aBoneIdsB, aBoneWeightsA, aBoneWeightsB);
		#endif
	
		#ifdef MODEL_TO_WORLD
			pos = uModelToWorldMatrix * pos;
		#endif
	
		gl_Position = uPickingProjection * (uWorldToViewMatrix * pos);
		
	#else //HAS_POSITION
		gl_Position = vec4(0.0, 0.0, 1.1, 1.0);
	#endif //HAS_POSITION
}

#endif // VERTEX_SHADER
