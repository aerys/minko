uniform vec4 diffuseColor;
uniform vec3 lightDirection;

varying vec4 worldNormal;

void main(void)
{
	gl_FragColor = diffuseColor;
}
