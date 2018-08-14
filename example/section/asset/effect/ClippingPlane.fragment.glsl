#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	#ifdef GL_FRAGMENT_PRECISION_HIGH
    	precision highp float;
	#else
    	precision mediump float;
	#endif
#endif

#pragma include "Fog.function.glsl"

uniform vec3 uCameraPosition;
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

varying vec3 vertexPosition;
varying vec3 vertexNormal;

void main(void)
{
#ifdef CLIPPING_PLANE_0
	if (clipDist0 + 0.00001 < -uClippingPlane0.w * 2.0)
		discard;
#endif
#ifdef CLIPPING_PLANE_1
	if (clipDist1 + 0.00001 < -uClippingPlane1.w * 2.0)
		discard;
#endif
#ifdef CLIPPING_PLANE_2
	if (clipDist2 + 0.00001 < -uClippingPlane2.w * 2.0)
		discard;
#endif
#ifdef CLIPPING_PLANE_3
	if (clipDist3 + 0.00001 < -uClippingPlane3.w * 2.0)
		discard;
#endif
#ifdef CLIPPING_PLANE_4
	if (clipDist4 + 0.00001 < -uClippingPlane4.w * 2.0)
		discard;
#endif
#ifdef CLIPPING_PLANE_5
	if (clipDist5 + 0.00001 < -uClippingPlane5.w * 2.0)
		discard;
#endif

	vec3 eyeVector = normalize(vertexPosition - uCameraPosition);

	float lambert = clamp(-dot(normalize(vertexNormal), normalize(eyeVector)), 0.0, 1.0);

	vec4 diffuse = vec4(lambert, lambert, lambert, 1.0) * 0.8 + 0.2;

	#ifdef FOG_ENABLED
		diffuse = fog_sampleFog(diffuse, gl_FragCoord);
	#endif // FOG_ENABLED

	gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER