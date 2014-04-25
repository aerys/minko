#ifdef VERTEX_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("Water.function.glsl")

attribute vec3 position;
attribute vec2 uv;
attribute vec3 normal;
attribute vec3 tangent;

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
	
		vertexNormal	= normal;		

		#ifdef MODEL_TO_WORLD
			vertexNormal 	= mat3(modelToWorldMatrix) * vertexNormal;
		#endif // MODEL_TO_WORLD
		
		vec3 tangentWave = tangent;
		
		#ifdef NUMWAVES
		vertexNormal = vec3(0.0, 0.0, 0.0);
		for (int i = 0; i < NUMWAVES; ++i)
		{
			vec3 normalWave = vec3(0.0, 0.0, 0.0);
			tangentWave = vec3(0.0, 0.0, 0.0);
			if (waveType[i] < 0.5)
				normalWave = addNormalDirectionalWave(worldPosition, vec2(waveOrigins[i * 2], waveOrigins[i * 2 + 1]), waveAmplitudes[i], waveSpeed[i], waveLength[i], waveSharpness[i], frameId);
			else
				normalWave = addNormalCircularWave(worldPosition, vec2(waveOrigins[i * 2], waveOrigins[i * 2 + 1]), waveAmplitudes[i], waveSpeed[i], waveLength[i], waveSharpness[i], frameId);
			
			vertexNormal += normalWave;

			tangentWave += vec3(0.0, -normalWave.z, 1.0);
		}

		#endif // NUMWAVES

		vertexNormal 	= normalize(vertexNormal);
		tangentWave		= normalize(tangentWave);

		#ifdef NORMAL_MAP
			vertexTangent = tangentWave;
			#ifdef MODEL_TO_WORLD
				vertexTangent = mat3(modelToWorldMatrix) * vertexTangent;
			#endif // MODEL_TO_WORLD
			vertexTangent = normalize(vertexTangent);
		#endif // NORMAL_MAP
		
	#endif // NUM_DIRECTIONAL_LIGHTS || NUM_POINT_LIGHTS || NUM_SPOT_LIGHTS || ENVIRONMENT_MAP_2D || ENVIRONMENT_CUBE_MAP

	gl_Position =  worldToScreenMatrix * worldPosition;

	vertexScreenPosition = gl_Position;

}

#endif // VERTEX_SHADER