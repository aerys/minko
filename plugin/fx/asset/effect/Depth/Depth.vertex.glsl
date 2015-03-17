#ifdef VERTEX_SHADER

#ifdef GL_ES
# ifdef GL_FRAGMENT_PRECISION_HIGH
    precision highp float;
# else
    precision mediump float;
# endif
#endif

#pragma include "../Skinning.function.glsl"

attribute vec3 aPosition;

#ifdef SKINNING_NUM_BONES
attribute vec4 aBoneIdsA;
attribute vec4 aBoneIdsB;
attribute vec4 aBoneWeightsA;
attribute vec4 aBoneWeightsB;
uniform mat4 uBoneMatrices[SKINNING_NUM_BONES];
#endif

uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToScreenMatrix;

varying vec3 vWorldPosition;

void main(void)
{
	vec4 pos = vec4(aPosition, 1.0);

	#ifdef NUM_BONES
		pos.xyz = skinning_moveVertex(pos.xyz, uBoneMatrices, aBoneIdsA, aBoneIdsB, aBoneWeightsA, aBoneWeightsB);
	#endif // NUM_BONES

    vWorldPosition = pos.xyz;
	#ifdef MODEL_TO_WORLD
		pos = uModelToWorldMatrix * pos;
	#endif

	gl_Position =  uWorldToScreenMatrix * pos;
}

#endif // VERTEX_SHADER
