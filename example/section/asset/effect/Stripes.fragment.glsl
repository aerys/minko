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

uniform float uClippingPlaneId;

varying vec2 vVertexUV;
varying vec3 vVertexUVW;
varying vec4 vVertexScreenPosition;
varying vec3 vPosition;
#ifdef CLIPPING_PLANE_0
uniform vec4 uClippingPlane0;
varying float clipDist0;
#endif
#ifdef CLIPPING_PLANE_1
uniform vec4 uClippingPlane1;
varying float clipDist1;
#endif
#ifdef CLIPPING_PLANE_2
uniform vec4 uClippingPlane2;
varying float clipDist2;
#endif
#ifdef CLIPPING_PLANE_3
uniform vec4 uClippingPlane3;
varying float clipDist3;
#endif
#ifdef CLIPPING_PLANE_4
uniform vec4 uClippingPlane4;
varying float clipDist4;
#endif
#ifdef CLIPPING_PLANE_5
uniform vec4 uClippingPlane5;
varying float clipDist5;
#endif

void main(void)
{
#ifdef CLIPPING_PLANE_0
	if (uClippingPlaneId != 0.0 && clipDist0 - 0.00001 < -uClippingPlane0.w * 2.0)
		discard;
#endif
#ifdef CLIPPING_PLANE_1
	if (uClippingPlaneId != 1.0 && clipDist1 - 0.00001 < -uClippingPlane1.w * 2.0)
		discard;
#endif
#ifdef CLIPPING_PLANE_2
	if (uClippingPlaneId != 2.0 && clipDist2 - 0.00001 < -uClippingPlane2.w * 2.0)
		discard;
#endif
#ifdef CLIPPING_PLANE_3
	if (uClippingPlaneId != 3.0 && clipDist3 - 0.00001 < -uClippingPlane3.w * 2.0)
		discard;
#endif
#ifdef CLIPPING_PLANE_4
	if (uClippingPlaneId != 4.0 && clipDist4 - 0.00001 < -uClippingPlane4.w * 2.0)
		discard;
#endif
#ifdef CLIPPING_PLANE_5
	if (uClippingPlaneId != 5.0 && clipDist5 - 0.00001 < -uClippingPlane5.w * 2.0)
		discard;
#endif

	vec4 stripesColor = vec4(0.0, 0.25, 0.35, 1.0);
	vec4 diffuse = mix(uDiffuseColor, stripesColor, (abs(1.0 / (sin((vPosition.x + vPosition.y + vPosition.z) * 8.0) - 1.0)) - 0.5) / 300.0);


	gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER