#ifdef VERTEX_SHADER

#ifdef GL_ES
# ifdef MINKO_PLATFORM_IOS
	precision highp float;
# else
	precision mediump float;
# endif
#endif

attribute	vec3	startPosition;
attribute	vec3	stopPosition;
attribute	vec3	weights;

uniform		mat4	modelToWorldMatrix;
uniform		mat4	worldToScreenMatrix;
uniform		vec4	viewport;
uniform		float	lineThickness;

void
main()
{
	float	wStart		= weights.x;
	float	wStop		= weights.y;
	float	lineSpread	= weights.z;
	
	vec4	startPos	= vec4(startPosition,	1.0);
	vec4	stopPos		= vec4(stopPosition,	1.0);
	
	#ifdef MODEL_TO_WORLD
		startPos		= modelToWorldMatrix * startPos;
		stopPos			= modelToWorldMatrix * stopPos;
	#endif // MODEL_TO_WORLD
	
	startPos			= worldToScreenMatrix * startPos;
	stopPos				= worldToScreenMatrix * stopPos;
	
	vec4	pos			= wStart * startPos + wStop * stopPos;
	float	posW		= pos.w;
	
	// account for perspective division for screen-space offsetting
	startPos			/= startPos.w;
	stopPos				/= stopPos.w;
	
	vec3	normal		= normalize(vec3(
		startPos.y - stopPos.y,
		stopPos.x - startPos.x,
		0.0
	));
	normal 				*= lineSpread;
	normal				*= lineThickness;
	normal				/= vec3(viewport.zw, 1.0); // ( 1/viewport.width, 1/viewport.height, 1 )
	
	pos					/= posW;
	pos.xyz				+= normal;
	
	// restore perspective division
	gl_Position	= vec4(pos.xyz * posW, posW);
}

#endif // VERTEX_SHADER
