#ifdef GL_ES
	precision mediump float;
#endif

attribute vec3 aPosition;

uniform mat4 uModelToWorldMatrix;
uniform mat4 uWorldToScreenMatrix;

void main(void)
{
	gl_Position = uWorldToScreenMatrix * uModelToWorldMatrix * vec4(aPosition, 1.0);
}