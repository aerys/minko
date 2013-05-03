uniform mat4 projectionMatrix;

void main(void)
{
	gl_FragColor = projectionMatrix * vec4(1., 0., 0., 1.);
}