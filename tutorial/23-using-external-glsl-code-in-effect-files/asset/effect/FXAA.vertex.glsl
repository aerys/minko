#ifdef GL_ES
precision mediump float;
#endif
 
attribute vec3 aPosition;
attribute vec2 aUv;
 
varying vec4 vertTexcoord;
varying vec4 vertColor;
 
void main(void)
{
  #if defined(VERTEX_RGB)
    vertColor = vec4(aRgb, 1);
  #elif defined(VERTEX_RGBA)
    vertColor = aRgba;
  #else
    vertColor = vec4(1);
  #end
 
  vertTexcoord = vec4(aUv.x, 1 - aUv.y, 0, 0);
 
  gl_Position = vec4(aPosition, 1) * vec4(1., 1., 1., .5);
}