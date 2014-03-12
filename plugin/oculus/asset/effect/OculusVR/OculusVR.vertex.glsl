#ifdef GL_ES
precision mediump float;
#endif

attribute 	vec3	aPosition;
attribute 	vec2	aUv;

varying		vec2 	vUv;

void main(void)
{
	vUv = aUv;

	gl_Position = vec4(aPosition, 1) * vec4(1, -1, 1, .5);
}
