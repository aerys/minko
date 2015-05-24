#ifdef VERTEX_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "Water.function.glsl"

attribute vec3 aPosition;
attribute vec2 aUV;

uniform float uWaveAmplitude[NUM_WAVES];
uniform vec2 uWaveOrigin[NUM_WAVES];
uniform float uWaveSpeed[NUM_WAVES];
uniform int uWaveType[NUM_WAVES];
uniform float uWaveLength[NUM_WAVES];
uniform float uWaveSharpness[NUM_WAVES];
uniform float uTime;
uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToScreenMatrix;

varying vec3 vVertexPosition;
varying vec2 vVertexUV;
varying vec3 vVertexNormal;
varying vec3 vVertexTangent;
varying vec4 vVertexScreenPosition;

void main(void)
{
    float t = uTime / 1000.;

	#if defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP
		vVertexUV = aUV + t * .001;
	#endif // defined DIFFUSE_MAP || defined NORMAL_MAP || defined SPECULAR_MAP

	vec4 worldPosition = vec4(aPosition, 1.0);
    vec4 worldWavePosition = vec4(0.);

	#ifdef MODEL_TO_WORLD
		worldPosition = uModelToWorldMatrix * worldPosition;
        worldWavePosition = worldPosition;
	#endif // MODEL_TO_WORLD

	#ifdef NUM_WAVES
        vec3 waveOffset = vec3(0.);
    	for (int i = 0; i < NUM_WAVES; ++i)
    	{
            water_Wave wave = water_Wave(
                uWaveType[i],
                uWaveOrigin[i],
                uWaveAmplitude[i],
                uWaveSpeed[i],
                uWaveLength[i],
                uWaveSharpness[i]
            );

            waveOffset += water_wavePosition(worldPosition.xyz, wave, t);
    	}

        worldWavePosition.xyz += waveOffset;
	#endif // NUM_WAVES

    vVertexPosition = worldWavePosition.xyz;

	#if defined NUM_DIRECTIONAL_LIGHTS || defined NUM_POINT_LIGHTS || defined NUM_SPOT_LIGHTS || defined ENVIRONMENT_MAP_2D || defined ENVIRONMENT_CUBE_MAP

		vVertexTangent = vec3(0.0, 0.0, 1.0);
		vVertexNormal = vec3(0.0, 1.0, 0.0);

		#ifdef NUM_WAVES
    		for (int i = 0; i < NUM_WAVES; ++i)
    		{
                water_Wave wave = water_Wave(
                    uWaveType[i],
                    uWaveOrigin[i],
                    uWaveAmplitude[i],
                    uWaveSpeed[i],
                    uWaveLength[i],
                    uWaveSharpness[i]
                );

                vVertexNormal += water_waveNormal(worldPosition.xyz, wave, t);
                #ifdef NORMAL_MAP
                    vVertexTangent += water_waveTangent(worldPosition.xyz, wave, t);
                #endif // NORMAL_MAP
    		}

    		vVertexNormal = normalize(vVertexNormal);
            #ifdef NORMAL_MAP
    		      vVertexTangent = normalize(vVertexTangent);
            #endif // NORMAL_MAP
		#endif // NUM_WAVES

	#endif // NUM_DIRECTIONAL_LIGHTS || NUM_POINT_LIGHTS || NUM_SPOT_LIGHTS || ENVIRONMENT_MAP_2D || ENVIRONMENT_CUBE_MAP

	vVertexScreenPosition = uWorldToScreenMatrix * worldWavePosition;

	gl_Position = vVertexScreenPosition;
}

#endif // VERTEX_SHADER
