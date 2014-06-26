#ifdef VERTEX_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("Skinning.function.glsl")

attribute vec3 position;
attribute vec2 uv;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToScreenMatrix;
uniform mat4 worldToViewMatrix;
uniform mat4 pickingProjection;
uniform mat4 cameraProjection;

varying vec2 vertexUV;

void main(void)
{
	#ifdef DIFFUSE_MAP
		vertexUV = uv;
	#endif
	
	#if defined(HAS_POSITION)
		vec4 pos = vec4(position, 1.0);

		#ifdef NUM_BONES
			pos = skinning_moveVertex(pos);
		#endif // NUM_BONES
	
		#ifdef MODEL_TO_WORLD
			pos = modelToWorldMatrix * pos;
		#endif
	
		gl_Position =  pickingProjection * (worldToViewMatrix * pos);
		
	#else //HAS_POSITION
		gl_Position =  vec4(0.0, 0.0, 1.1, 1.0);
	#endif //HAS_POSITION
}

#endif // VERTEX_SHADER
