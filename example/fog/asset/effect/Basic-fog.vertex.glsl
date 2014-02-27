#ifdef VERTEX_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("Skinning.function.glsl")

attribute vec3 position;
attribute vec2 uv;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToScreenMatrix;
uniform mat4 viewMatrix;
#if defined(FOG_COLOR)
	uniform vec4 fogColor;
	uniform float fogDensity;
#endif


varying vec2 vertexUV;
varying vec3 vertexUVW;

#ifdef FOG_COLOR
	varying float fogFactor;
#endif

void main(void)
{
	#ifdef DIFFUSE_MAP
		vertexUV = uv;
	#endif

	#ifdef DIFFUSE_CUBEMAP
		vertexUVW = position;
	#endif 

	vec4 pos = vec4(position, 1.0);

	#ifdef NUM_BONES
		pos = skinning_moveVertex(pos);
	#endif // NUM_BONES
	
	#ifdef MODEL_TO_WORLD
		pos = modelToWorldMatrix * pos;
	#endif

	#ifdef FOG_COLOR
		vec4 eyeSpacePosition = viewMatrix * pos;

		float fragDist = length(eyeSpacePosition);

		const float LOG2 = 1.442695;

		fogFactor = clamp(exp2(-fogColor.r * fogColor.r * fragDist * fragDist * LOG2), 0.0f, 1.0f);
	#endif
	
	gl_Position =  worldToScreenMatrix * pos;
}

#endif // VERTEX_SHADER
