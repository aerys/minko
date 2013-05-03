attribute vec4 position;

uniform mat4 worldViewMatrix;
uniform mat4 projectionMatrix;

uniform vec4 offset;

void main(void)
{
	gl_Position = worldViewMatrix * projectionMatrix * (position + offset);
}