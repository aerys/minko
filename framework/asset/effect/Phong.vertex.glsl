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

#ifdef VERTEX_UV1
attribute 	vec2	aUV1;
#endif

#ifdef SKINNING_NUM_BONES
attribute vec4 aBoneWeightsA;
attribute vec4 aBoneWeightsB;
#endif

#ifdef VERTEX_COLOR
attribute	float	aColor;
#endif

#ifdef VERTEX_POP_PROTECTED
attribute	float 	aPopProtected;
#endif

uniform 	mat4 	uModelToWorldMatrix;
uniform 	mat4 	uWorldToScreenMatrix;

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

varying 	vec3 	vVertexPosition;
varying 	vec4 	vVertexScreenPosition;
varying 	vec2 	vVertexUV;
varying 	vec2 	vVertexUV1;
varying 	vec3 	vVertexNormal;
varying 	vec3 	vVertexTangent;
varying		vec4	vVertexColor;

void main(void)
{
	#if defined VERTEX_UV && (defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP || defined ALPHA_MAP || (defined LIGHT_MAP && !defined VERTEX_UV1))
		vec2 uvScale = vec2(1.0);
		vec2 uvOffset = vec2(0.0);

		#ifdef UV_SCALE
			uvScale = uUVScale;
		#endif // UV_SCALE

		#ifdef UV_OFFSET
			uvOffset = uUVOffset;
		#endif // UV_OFFSET

		vVertexUV = uvScale * aUV + uvOffset;
	#endif // defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP || defined ALPHA_MAP

    #if defined (VERTEX_UV1) && defined (LIGHT_MAP)
        vVertexUV1 = aUV1;
    #endif // VERTEX_UV1 && LIGHT_MAP

	#if defined VERTEX_COLOR
		vVertexColor = vec4(packFloat8bitRGB(aColor), 1.0);
	#endif // VERTEX_COLOR

	vec4 worldPosition = vec4(aPosition, 1.0);

	#ifdef SKINNING_NUM_BONES
		worldPosition = skinning_moveVertex(worldPosition, aBoneWeightsA, aBoneWeightsB);
	#endif // SKINNING_NUM_BONES

	#ifdef POP_LOD_ENABLED
		float popProtected = 0.0;

		#ifdef VERTEX_POP_PROTECTED
			popProtected = aPopProtected;
		#endif // VERTEX_POP_PROTECTED

		#ifdef POP_BLENDING_ENABLED
			worldPosition = pop_blend(worldPosition, aNormal, uPopLod, uPopBlendingLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound, popProtected);
		#else
			worldPosition = pop_quantize(worldPosition, aNormal, uPopLod, uPopFullPrecisionLod, uPopMinBound, uPopMaxBound, popProtected);
		#endif // POP_BLENDING_ENABLED
	#endif // POP_LOD_ENABLED

	#ifdef MODEL_TO_WORLD
		worldPosition 	= uModelToWorldMatrix * worldPosition;
	#endif // MODEL_TO_WORLD

	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS || defined ENVIRONMENT_MAP_2D || defined ENVIRONMENT_CUBE_MAP

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

	#endif // NUM_DIRECTIONAL_LIGHTS || NUM_POINT_LIGHTS || NUM_SPOT_LIGHTS || ENVIRONMENT_MAP_2D || ENVIRONMENT_CUBE_MAP

	vec4 screenPosition = uWorldToScreenMatrix * worldPosition;

	vVertexScreenPosition = screenPosition;

	gl_Position = screenPosition;
}

#endif // VERTEX_SHADER
