#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

uniform sampler2D overlaySampler;

varying vec2 vertTexcoord;
uniform vec2 overlayRatio;

void main() {
  vec2 overlayUv = vec2(vertTexcoord.x, 1.0 - vertTexcoord.y) * overlayRatio.xy;
  vec4 overlayColor = texture2D(overlaySampler, overlayUv);
  
  overlayColor = vec4(overlayColor.z, overlayColor.y, overlayColor.x, overlayColor.w);

  gl_FragColor = overlayColor;
}