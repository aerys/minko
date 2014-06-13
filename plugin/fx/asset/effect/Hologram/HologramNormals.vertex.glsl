#ifdef VERTEX_SHADER

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToScreenMatrix;

attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;
attribute vec3 tangent;

varying vec2 vertexUV;
varying vec3 worldNormal;
varying vec3 worldTangent;

#pragma include('../Skinning.function.glsl')

void main(void)
{
	vec4 pos    = vec4(position, 1.0);
	vec4 n 		= vec4(normal, 0.0); 

	#ifdef NUM_BONES
		pos 	= skinning_moveVertex(pos);
		n 		= skinning_moveVertex(n);
	#endif // NUM_BONES
				
	worldNormal	= n.xyz;
	worldTangent = tangent;

	#ifdef MODEL_TO_WORLD
		pos 		= modelToWorldMatrix * pos;
		worldNormal	= mat3(modelToWorldMatrix) * worldNormal;
		worldTangent = mat3(modelToWorldMatrix) * worldTangent;
	#endif // MODEL_TO_WORLD

	#ifdef NORMAL_MAP
		vertexUV = uv;
	#endif

	gl_Position =  worldToScreenMatrix * pos;
}

#endif
