#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("Depth.function.glsl")

uniform vec3 		cameraPosition;

varying vec3 		worldPosition;

void main(void)
{

	float eyeToVertex	= sqrt(pow(cameraPosition.x - worldPosition.x, 2) + pow(cameraPosition.y - worldPosition.y, 2) + pow(cameraPosition.z - worldPosition.z, 2));
	float depth			= eyeToVertex / 100.0f;

	gl_FragColor = pack(depth);
}

#endif // FRAGMENT_SHADER
