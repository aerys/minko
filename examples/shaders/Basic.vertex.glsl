attribute vec4 position;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToScreenMatrix;

void main(void)
{
	gl_Position = modelToWorldMatrix * worldToScreenMatrix * position;
	//gl_Position = position * modelToWorldMatrix * worldToScreenMatrix;
}
