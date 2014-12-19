#ifdef VERTEX_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("Water.function.glsl")

attribute vec3 position;
attribute vec2 uv;
attribute vec3 normal;

uniform float waveAmplitudes[NUMWAVES];
uniform float waveOrigins[NUMWAVES * 2];
uniform float waveSpeed[NUMWAVES];
uniform float waveType[NUMWAVES];
uniform float waveLength[NUMWAVES];
uniform float waveSharpness[NUMWAVES];
uniform float frameId;
uniform mat4 modelToWorldMatrix;
uniform mat4 worldToScreenMatrix;

varying vec3 vertexPosition;
varying vec2 vertexUV;
varying vec3 vertexNormal;
varying vec3 vertexTangent;
varying vec4 vertexScreenPosition;

void main(void)
{
	#if defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP
		vertexUV = uv;
	#endif // defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP

	vec4 worldPosition 	= vec4(position, 1.0);

	
	#ifdef MODEL_TO_WORLD
		worldPosition 	= modelToWorldMatrix * worldPosition;
	#endif // MODEL_TO_WORLD

	#ifdef NUMWAVES
	for (int i = 0; i < NUMWAVES; ++i)
	{
		if (waveType[i] < 0.5)
			worldPosition += addDirectionalWave(worldPosition, vec2(waveOrigins[i * 2], waveOrigins[i * 2 + 1]), waveAmplitudes[i], waveSpeed[i], waveLength[i], waveSharpness[i], frameId);
		else
			worldPosition += addCircularWave(worldPosition, vec2(waveOrigins[i * 2], waveOrigins[i * 2 + 1]), waveAmplitudes[i], waveSpeed[i], waveLength[i], waveSharpness[i], frameId);
	}
	#endif // NUMWAVES
	
	vertexPosition = worldPosition.xyz;

	
	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS || defined ENVIRONMENT_MAP_2D || defined ENVIRONMENT_CUBE_MAP
	
		vertexTangent = vec3(0.0, 0.0, 1.0);
		vertexNormal = vec3(0.0, 1.0, 0.0);

		#ifdef NUMWAVES

		for (int i = 0; i < NUMWAVES; ++i)
		{
			vec3 normalWave		= vec3(0.0, 0.0, 0.0);
			vec3 tangentWave	= vec3(0.0, 0.0, 0.0);

			if (waveType[i] < 0.5)
			{
				normalWave = addNormalDirectionalWave(worldPosition, vec2(waveOrigins[i * 2], waveOrigins[i * 2 + 1]), waveAmplitudes[i], waveSpeed[i], waveLength[i], waveSharpness[i], frameId);
				tangentWave = addTangentDirectionalWave(worldPosition, vec2(waveOrigins[i * 2], waveOrigins[i * 2 + 1]), waveAmplitudes[i], waveSpeed[i], waveLength[i], waveSharpness[i], frameId);
			}
			else
			{
				normalWave = addNormalCircularWave(worldPosition, vec2(waveOrigins[i * 2], waveOrigins[i * 2 + 1]), waveAmplitudes[i], waveSpeed[i], waveLength[i], waveSharpness[i], frameId);
				tangentWave = addTangentCircularWave(worldPosition, vec2(waveOrigins[i * 2], waveOrigins[i * 2 + 1]), waveAmplitudes[i], waveSpeed[i], waveLength[i], waveSharpness[i], frameId);
			}

			vertexNormal += normalWave;
			vertexTangent += tangentWave;
		}

		#endif // NUMWAVES

		vertexNormal 	= normalize(vertexNormal);
		vertexTangent	= normalize(vertexTangent);

	#endif // NUM_DIRECTIONAL_LIGHTS || NUM_POINT_LIGHTS || NUM_SPOT_LIGHTS || ENVIRONMENT_MAP_2D || ENVIRONMENT_CUBE_MAP

	#ifdef NORMAL_MAP
		//vertexTangent = tangent;
		#ifdef MODEL_TO_WORLD
			vertexTangent = mat3(modelToWorldMatrix) * vertexTangent;
		#endif // MODEL_TO_WORLD
		vertexTangent = normalize(vertexTangent);
	#endif // NORMAL_MAP

	gl_Position =  worldToScreenMatrix * worldPosition;

	vertexScreenPosition = gl_Position;

}

#endif // VERTEX_SHADER
