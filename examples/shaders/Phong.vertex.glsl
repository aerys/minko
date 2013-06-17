attribute vec3 position;
attribute vec3 normal;

uniform mat4 modelToWorldMatrix;
uniform mat4 worldToScreenMatrix;

varying vec4 worldNormal;

void main(void)
{
	worldNormal = normalize(modelToWorldMatrix * vec4(normal, 1.0));

	gl_Position =  worldToScreenMatrix * modelToWorldMatrix * vec4(position, 1.0);
}
