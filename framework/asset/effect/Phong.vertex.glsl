#ifdef VERTEX_SHADER

#ifdef GL_FRAGMENT_PRECISION_HIGH
    precision highp float;
#else
    precision mediump float;
#endif

#pragma include "Skinning.function.glsl"

attribute 	vec3 	aPosition;
attribute 	vec2 	aUV;
attribute 	vec3 	aNormal;
attribute 	vec3 	aTangent;
attribute 	vec4	aBoneIdsA;
attribute 	vec4	aBoneIdsB;
attribute 	vec4	aBoneWeightsA;
attribute 	vec4	aBoneWeightsB;

#ifdef SKINNING_NUM_BONES
uniform 	mat4	uBoneMatrices[SKINNING_NUM_BONES];
#endif
uniform 	mat4 	uModelToWorldMatrix;
uniform 	mat4 	uWorldToScreenMatrix;
uniform 	vec2 	uUVScale;
uniform 	vec2 	uUVOffset;
uniform 	mat4 	uLightWorldToScreenMatrix;

varying 	vec3 	vertexPosition;
varying 	vec4 	vertexScreenPosition;
varying 	vec2 	vertexUV;
varying 	vec3 	vertexNormal;
varying 	vec3 	vertexTangent;

void main(void)
{
	#if defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP || defined ALPHA_MAP
		vertexUV = uUVScale * aUV + uUVOffset;
	#endif // defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP || defined ALPHA_MAP

	vec4 worldPosition = vec4(aPosition, 1.0);

	#ifdef SKINNING_NUM_BONES
		worldPosition = skinning_moveVertex(worldPosition, uBoneMatrices, aBoneIdsA, aBoneIdsB, aBoneWeightsA, aBoneWeightsB);
	#endif // SKINNING_NUM_BONES

	#ifdef MODEL_TO_WORLD
		worldPosition 	= uModelToWorldMatrix * worldPosition;
	#endif // MODEL_TO_WORLD

	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS || defined ENVIRONMENT_MAP_2D || defined ENVIRONMENT_CUBE_MAP

		vertexPosition = worldPosition.xyz;
		vertexNormal = aNormal;

		#ifdef SKINNING_NUM_BONES
			vertexNormal = skinning_moveVertex(vec4(aNormal, 0.0), uBoneMatrices, aBoneIdsA, aBoneIdsB, aBoneWeightsA, aBoneWeightsB).xyz;
		#endif // SKINNING_NUM_BONES

		#ifdef MODEL_TO_WORLD
			vertexNormal = mat3(uModelToWorldMatrix) * vertexNormal;
		#endif // MODEL_TO_WORLD
		vertexNormal = normalize(vertexNormal);

		#ifdef NORMAL_MAP
			vertexTangent = aTangent;
			#ifdef MODEL_TO_WORLD
				vertexTangent = mat3(uModelToWorldMatrix) * vertexTangent;
			#endif // MODEL_TO_WORLD
			vertexTangent = normalize(vertexTangent);
		#endif // NORMAL_MAP

	#endif // NUM_DIRECTIONAL_LIGHTS || NUM_POINT_LIGHTS || NUM_SPOT_LIGHTS || ENVIRONMENT_MAP_2D || ENVIRONMENT_CUBE_MAP

	vec4 screenPosition = uWorldToScreenMatrix * worldPosition;

	vertexScreenPosition = screenPosition;

	gl_Position = screenPosition;
}

#endif // VERTEX_SHADER
