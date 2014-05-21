#ifdef VERTEX_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("../Skinning.function.glsl")

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
varying vec3 screenPosition;

void main(void)
{
	vertexUV = uvScale * uv + uvOffset;
	
	vec4 worldPosition 	= vec4(position, 1.0);
	
	#ifdef NUM_BONES
		worldPosition	= skinning_moveVertex(worldPosition);
	#endif // NUM_BONES
	
	#ifdef MODEL_TO_WORLD
		worldPosition 	= modelToWorldMatrix * worldPosition;
	#endif // MODEL_TO_WORLD
	
	vertexPosition	= worldPosition.xyz;
		
	vertexNormal	= normal;		

	#ifdef NUM_BONES
		vertexNormal	= skinning_moveVertex(vec4(normal, 0.0)).xyz;
	#endif // NUM_BONES
		
	#ifdef MODEL_TO_WORLD
		vertexNormal 	= mat3(modelToWorldMatrix) * vertexNormal;
	#endif // MODEL_TO_WORLD
	vertexNormal 	= normalize(vertexNormal);
	
	gl_Position =  worldToScreenMatrix * worldPosition;

	screenPosition = gl_Position.xyz / gl_Position.w;
}

#endif // VERTEX_SHADER