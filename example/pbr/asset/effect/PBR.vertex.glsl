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
#pragma include "Pack.function.glsl"

attribute 	vec3 	aPosition;
attribute 	vec2 	aUV;
attribute 	vec3 	aNormal;
attribute 	vec3 	aTangent;
attribute 	vec4	aBoneWeightsA;
attribute 	vec4	aBoneWeightsB;
attribute	float	aColor;

uniform 	mat4 	uModelToWorldMatrix;
uniform 	mat4 	uWorldToScreenMatrix;
uniform 	vec2 	uUVScale;
uniform 	vec2 	uUVOffset;
uniform 	mat4 	uLightWorldToScreenMatrix;

uniform 	float 	uPopLod;
uniform 	float 	uPopBlendingLod;
uniform 	float 	uPopFullPrecisionLod;
uniform 	vec3 	uPopMinBound;
uniform 	vec3 	uPopMaxBound;

varying 	vec3 	vVertexPosition;
varying 	vec4 	vVertexScreenPosition;
varying 	vec2 	vVertexUV;
varying 	vec3 	vVertexNormal;
varying 	vec3 	vVertexTangent;
varying		vec4	vVertexColor;

void main(void)
{
	#if defined VERTEX_UV && (defined ALBEDO_MAP || defined NORMAL_MAP || defined SPECULAR_MAP || defined ALPHA_MAP)
		vVertexUV = uUVScale * aUV + uUVOffset;
	#endif // defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP || defined ALPHA_MAP

	#if defined VERTEX_COLOR
		vVertexColor = vec4(packFloat8bitRGB(aColor), 1.0);
	#endif // VERTEX_COLOR

	vec4 worldPosition = vec4(aPosition, 1.0);

	#ifdef SKINNING_NUM_BONES
		worldPosition = skinning_moveVertex(worldPosition, aBoneWeightsA, aBoneWeightsB);
	#endif // SKINNING_NUM_BONES

	#ifdef POP_LOD_ENABLED
		#ifdef POP_BLENDING_ENABLED
			worldPosition = pop_blend(worldPosition, aNormal, uPopLod, uPopBlendingLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound);
		#else
			worldPosition = pop_quantify(worldPosition, aNormal, uPopLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound);
		#endif // POP_BLENDING_ENABLED
	#endif // POP_LOD_ENABLED

	#ifdef MODEL_TO_WORLD
		worldPosition = uModelToWorldMatrix * worldPosition;
	#endif // MODEL_TO_WORLD

	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS || defined ENVIRONMENT_MAP_2D || defined IRRADIANCE_MAP || defined RADIANCE_MAP
		vVertexPosition = worldPosition.xyz;
		vVertexNormal = aNormal;

		#ifdef SKINNING_NUM_BONES
			vVertexNormal = skinning_moveVertex(vec4(aNormal, 0.0), aBoneWeightsA, aBoneWeightsB).xyz;
		#endif // SKINNING_NUM_BONES

		#ifdef MODEL_TO_WORLD
			vVertexNormal = mat3(uModelToWorldMatrix) * vVertexNormal;
		#endif // MODEL_TO_WORLD
		vVertexNormal = normalize(vVertexNormal);

		#ifdef NORMAL_MAP
			vVertexTangent = aTangent;
			#ifdef MODEL_TO_WORLD
				vVertexTangent = mat3(uModelToWorldMatrix) * vVertexTangent;
			#endif // MODEL_TO_WORLD
			vVertexTangent = normalize(vVertexTangent);
		#endif // NORMAL_MAP
	#endif // NUM_DIRECTIONAL_LIGHTS || NUM_POINT_LIGHTS || NUM_SPOT_LIGHTS || ENVIRONMENT_MAP_2D || ENVIRONMENT_CUBE_MAP || IRRADIANCE_CUBEMAP

	vec4 screenPosition = uWorldToScreenMatrix * worldPosition;

	vVertexScreenPosition = screenPosition;

	gl_Position = screenPosition;
}

#endif // VERTEX_SHADER
