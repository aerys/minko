#ifdef VERTEX_SHADER

#ifdef GL_ES
# ifdef MINKO_PLATFORM_IOS
	precision highp float;
# else
	precision mediump float;
# endif
#endif

#pragma include "Skinning.function.glsl"

attribute vec3 aPosition;
attribute vec2 aUV;

uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToScreenMatrix;
uniform vec2 uUVScale;
uniform vec2 uUVOffset;

varying vec2 vVertexUV;
varying vec3 vVertexUVW;

void main(void)
{
	#if defined(DIFFUSE_MAP) || defined(ALPHA_MAP)
		vec2 uv = aUV;

		#if defined(UV_SCALE)
			uv *= uUVScale;
		#endif

		#if defined(UV_OFFSET)
			uv += uUVOffset;
		#endif

		vVertexUV = uv;
	#endif

	#ifdef DIFFUSE_CUBEMAP
		vertexUVW = aPosition;
	#endif

	vec4 pos = vec4(aPosition, 1.0);

	#ifdef NUM_BONES
		pos = skinning_moveVertex(pos);
	#endif

	#ifdef MODEL_TO_WORLD
		pos = uModelToWorldMatrix * pos;
	#endif

	gl_Position = uWorldToScreenMatrix * pos;
}

#endif // VERTEX_SHADER
