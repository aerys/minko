attribute vec3 position;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToScreenMatrix;

void main(void)
{
	//gl_Position =  vec4(position, 1.0) * modelToWorldMatrix * worldToScreenMatrix;
	gl_Position =  worldToScreenMatrix * modelToWorldMatrix * vec4(position, 1.0);
}
