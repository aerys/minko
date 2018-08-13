#ifdef FRAGMENT_SHADER

#ifdef GL_ES
    #ifdef GL_FRAGMENT_PRECISION_HIGH
        precision highp float;
    #else
        precision mediump float;
    #endif
#endif

#pragma include "TextureLod.extension.glsl"

#pragma include "Fog.function.glsl"
#pragma include "TextureLod.function.glsl"

uniform vec4 uDiffuseColor;
uniform sampler2D uDiffuseMap;
uniform samplerCube	uDiffuseCubeMap;

// alpha
uniform sampler2D uAlphaMap;
uniform float uAlphaThreshold;

// fog
uniform vec4 uFogColor;
uniform vec2 uFogBounds;

// texture lod
uniform float uDiffuseMapMaxAvailableLod;
uniform vec2 uDiffuseMapSize;

varying vec2 vVertexUV;
varying vec3 vVertexUVW;
varying vec4 vVertexScreenPosition;
varying vec3 vPosition;

void main(void)
{
	vec4 stripesColor = vec4(0.0, 0.25, 0.35, 1.0);
	vec4 diffuse = mix(uDiffuseColor, stripesColor, (abs(1.0 / (sin((vPosition.x + vPosition.y + vPosition.z) * 8.0) - 1.0)) - 0.5) / 300.0);


	gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER
