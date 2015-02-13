#ifdef VERTEX_SHADER

#ifdef GL_ES
# ifdef MINKO_PLATFORM_IOS
    precision highp float;
# else
    precision mediump float;
# endif
#endif

attribute vec3 aStartPosition;
attribute vec3 aStopPosition;
attribute vec3 aWeights;

uniform	mat4 uModelToWorldMatrix;
uniform	mat4 uWorldToScreenMatrix;
uniform	vec4 uViewport;
uniform	float uLineThickness;

void
main()
{
	float wStart = aWeights.x;
	float wStop = aWeights.y;
	float lineSpread = aWeights.z;

	vec4 startPos = vec4(aStartPosition, 1.0);
	vec4 stopPos = vec4(aStopPosition, 1.0);

	#ifdef MODEL_TO_WORLD
		startPos = uModelToWorldMatrix * startPos;
		stopPos	= uModelToWorldMatrix * stopPos;
	#endif // MODEL_TO_WORLD

	startPos = uWorldToScreenMatrix * startPos;
	stopPos = uWorldToScreenMatrix * stopPos;

	vec4 pos = wStart * startPos + wStop * stopPos;
	float posW = pos.w;

	// account for perspective division for screen-space offsetting
	startPos /= startPos.w;
	stopPos /= stopPos.w;

	vec3 normal	= normalize(vec3(
		startPos.y - stopPos.y,
		stopPos.x - startPos.x,
		0.0
	));
	normal *= lineSpread;
	normal *= uLineThickness;
	normal /= vec3(uViewport.zw, 1.0); // ( 1/uViewport.width, 1/uViewport.height, 1 )

	pos	/= posW;
	pos.xyz += normal;

	// restore perspective division
	gl_Position	= vec4(pos.xyz * posW, posW);
}

#endif // VERTEX_SHADER
