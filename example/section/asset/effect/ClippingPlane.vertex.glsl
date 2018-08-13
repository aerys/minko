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
attribute vec3 aNormal;
attribute float aPopProtected;

#ifdef SKINNING_NUM_BONES
attribute vec4 aBoneWeightsA;
attribute vec4 aBoneWeightsB;
#endif

uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToScreenMatrix;

uniform 	float 	uPopLod;
uniform 	float 	uPopBlendingLod;
uniform 	float 	uPopFullPrecisionLod;
uniform 	vec3 	uPopMinBound;
uniform 	vec3 	uPopMaxBound;
#ifdef CLIPPING_PLANE_0
uniform		vec4	uClippingPlane0;
#endif
#ifdef CLIPPING_PLANE_1
uniform		vec4	uClippingPlane1;
#endif

varying vec3 vertexPosition;
varying vec3 vertexNormal;
#ifdef CLIPPING_PLANE_0
varying float clipDist0;
#endif
#ifdef CLIPPING_PLANE_1
varying float clipDist1;
#endif

void main(void)
{
	vec4 pos = vec4(aPosition, 1.0);

	#ifdef SKINNING_NUM_BONES
		pos = skinning_moveVertex(pos, aBoneWeightsA, aBoneWeightsB);
	#endif // SKINNING_NUM_BONES

	#ifdef POP_LOD_ENABLED
		float popProtected = 0.0;

		#ifdef VERTEX_POP_PROTECTED
			popProtected = aPopProtected;
		#endif // VERTEX_POP_PROTECTED

		#ifdef POP_BLENDING_ENABLED
			pos = pop_blend(pos, aNormal, uPopLod, uPopBlendingLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound, popProtected);
		#else
			pos = pop_quantize(pos, aNormal, uPopLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound, popProtected);
		#endif // POP_BLENDING_ENABLED
	#endif // POP_LOD_ENABLED

	#ifdef MODEL_TO_WORLD
		pos = uModelToWorldMatrix * pos;
	#endif

	vertexPosition = pos.xyz;
	vertexNormal = aNormal;

	#ifdef SKINNING_NUM_BONES
		vertexNormal = skinning_moveVertex(vec4(aNormal, 0.0), aBoneWeightsA, aBoneWeightsB).xyz;
	#endif // SKINNING_NUM_BONES

	#ifdef MODEL_TO_WORLD
		vertexNormal = mat3(uModelToWorldMatrix) * vertexNormal;
	#endif // MODEL_TO_WORLD

	vertexNormal = normalize(vertexNormal);

	gl_Position = uWorldToScreenMatrix * pos;

#ifdef CLIPPING_PLANE_0
	// Compute the distance between the vertex and the clip plane
	clipDist0 = dot(pos.xyz, uClippingPlane0.xyz) - uClippingPlane0.w;
#endif
#ifdef CLIPPING_PLANE_1
	// Compute the distance between the vertex and the clip plane
	clipDist1 = dot(pos.xyz, uClippingPlane1.xyz) - uClippingPlane1.w;
#endif
}

#endif // VERTEX_SHADER
