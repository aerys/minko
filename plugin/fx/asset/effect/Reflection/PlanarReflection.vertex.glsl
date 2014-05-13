#ifdef VERTEX_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("../Skinning.function.glsl")

attribute vec3 position;
attribute vec2 uv;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToScreenMatrix;

uniform mat4 View;
uniform mat4 Projection;

uniform mat4 ReflectionMatrix;
uniform mat4 ReflectedProjection;

uniform mat4 ReflectedViewMatrix;

varying vec3 vPosition;

varying vec2 vertexUV;
varying vec3 vertexUVW;

void main(void)
{
	#ifdef DIFFUSE_MAP
		vertexUV = uv;
	#endif

	#ifdef DIFFUSE_CUBEMAP
		vertexUVW = position;
	#endif 
	
	#if defined(HAS_POSITION)
		vec4 pos = vec4(position, 1.0);

		#ifdef NUM_BONES
			pos = skinning_moveVertex(pos);
		#endif // NUM_BONES
	
		#ifdef MODEL_TO_WORLD
			pos = modelToWorldMatrix * pos;
		#endif

		gl_Position = Projection * ReflectedViewMatrix * pos;
	
		vPosition = pos.xyz / pos.w;
	#else //HAS_POSITION
		gl_Position = vec4(0.0, 0.0, 1.1, 1.0);

		vPosition = vec3(0.0, -1.0, 0.0);
	#endif //HAS_POSITION
}

#endif // VERTEX_SHADER
