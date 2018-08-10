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
uniform vec4 uClippingPlane;

varying float clipDist;

varying vec3 vertexPosition;
varying vec3 vertexNormal;

void main(void)
{
	if (clipDist + 0.00001 < -uClippingPlane.w * 2.0)
		discard;

	vec3 eyeVector = normalize(vertexPosition - uCameraPosition);
	
	float lambert = clamp(-dot(normalize(vertexNormal), normalize(eyeVector)), 0.0, 1.0);
	
	vec4 diffuse = vec4(lambert, lambert, lambert, 1.0) * 0.8 + 0.2;
	
	#ifdef FOG_ENABLED
		diffuse = fog_sampleFog(diffuse, gl_FragCoord);
	#endif // FOG_ENABLED

	gl_FragColor = diffuse;
}

#endif // FRAGMENT_SHADER