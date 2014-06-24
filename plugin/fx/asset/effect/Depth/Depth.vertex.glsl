#ifdef VERTEX_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("../Skinning.function.glsl")

attribute vec3 position;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToScreenMatrix;

varying vec3 worldPosition;

void main(void)
{
	vec4 pos = vec4(position, 1.0f);

	#ifdef NUM_BONES
		pos = skinning_moveVertex(pos);
	#endif // NUM_BONES
	
	pos = modelToWorldMatrix * pos;
	
	worldPosition = pos.xyz;

	gl_Position =  worldToScreenMatrix * pos;
}

#endif // VERTEX_SHADER
