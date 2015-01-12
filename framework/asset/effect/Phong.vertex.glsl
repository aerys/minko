#ifdef VERTEX_SHADER

#ifdef GL_ES
# ifdef MINKO_PLATFORM_IOS
	precision highp float;
# else
	precision mediump float;
# endif
#endif

#pragma include("Skinning.function.glsl")

attribute vec3 position;
attribute vec2 uv;
attribute vec3 normal;
attribute vec3 tangent;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToScreenMatrix;
uniform vec2 uvScale;
uniform vec2 uvOffset;

varying vec3 vertexPosition;
varying vec2 vertexUV;
varying vec3 vertexNormal;
varying vec3 vertexTangent;

void main(void)
{
	#if defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP || defined ALPHA_MAP
		vertexUV = uvScale * uv + uvOffset;
	#endif // defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP || defined ALPHA_MAP

	vec4 worldPosition 	= vec4(position, 1.0);

	#ifdef NUM_BONES
		worldPosition	= skinning_moveVertex(worldPosition);
	#endif // NUM_BONES

	#ifdef MODEL_TO_WORLD
		worldPosition 	= modelToWorldMatrix * worldPosition;
	#endif // MODEL_TO_WORLD

	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS || defined ENVIRONMENT_MAP_2D || defined ENVIRONMENT_CUBE_MAP

		vertexPosition	= worldPosition.xyz;		
		vertexNormal	= normal;

		#ifdef NUM_BONES
			vertexNormal	= skinning_moveVertex(vec4(normal, 0.0)).xyz;
		#endif // NUM_BONES

		#ifdef MODEL_TO_WORLD
			vertexNormal 	= mat3(modelToWorldMatrix) * vertexNormal;
		#endif // MODEL_TO_WORLD
		vertexNormal 	= normalize(vertexNormal);

		#ifdef NORMAL_MAP
			vertexTangent = tangent;
			#ifdef MODEL_TO_WORLD
				vertexTangent = mat3(modelToWorldMatrix) * vertexTangent;
			#endif // MODEL_TO_WORLD
			vertexTangent = normalize(vertexTangent);
		#endif // NORMAL_MAP

	#endif // NUM_DIRECTIONAL_LIGHTS || NUM_POINT_LIGHTS || NUM_SPOT_LIGHTS || ENVIRONMENT_MAP_2D || ENVIRONMENT_CUBE_MAP

	gl_Position =  worldToScreenMatrix * worldPosition;
}

#endif // VERTEX_SHADER
