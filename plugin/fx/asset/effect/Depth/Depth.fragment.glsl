#ifdef FRAGMENT_SHADER

#ifdef GL_ES
# ifdef GL_FRAGMENT_PRECISION_HIGH
    precision highp float;
# else
    precision mediump float;
# endif
#endif

#pragma include "../Pack.function.glsl"

uniform vec3 uCameraPosition;
varying vec3 vWorldPosition;

void main(void)
{
	// FIXME
	float maxDistance = 100.f;
	float eyeToVertex = sqrt(pow(uCameraPosition.x - vWorldPosition.x, 2) + pow(uCameraPosition.y - vWorldPosition.y, 2) + pow(uCameraPosition.z - vWorldPosition.z, 2));
	float depth = eyeToVertex / maxDistance;

	gl_FragColor = packFloat8bitRGBA(depth);
}

#endif // FRAGMENT_SHADER
