#ifdef GL_ES
 precision mediump float;
#endif

attribute vec3 aPosition;

uniform mat4 uModelToWorldMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main(void)
{
  gl_Position = uProjectionMatrix * uViewMatrix * uModelToWorldMatrix * vec4(aPosition, 1.0);
}