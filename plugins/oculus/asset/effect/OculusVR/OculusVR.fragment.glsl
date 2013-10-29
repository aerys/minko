#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uTex;
uniform vec2 uLensCenter;
uniform vec2 uScreenCenter;
uniform float uOffset;

varying vec2 vUv;

const vec2 Scale = vec2(0.1469278, 0.2350845);
const vec2 ScaleIn = vec2(4, 2.5);
const vec4 HmdWarpParam = vec4(1, 0.22, 0.24, 0);

vec2 HmdWarp(vec2 in01, vec2 LensCenter)
{
   vec2 theta = (in01 - LensCenter) * ScaleIn; // Scales to [-1, 1]
   float rSq = theta.x * theta.x + theta.y * theta.y;
   vec2 rvector = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq +
	  HmdWarpParam.z * rSq * rSq +
	  HmdWarpParam.w * rSq * rSq * rSq);
	  
   return LensCenter + Scale * rvector;
}

void main(void)
{
	vec2 tc = HmdWarp(vUv.xy, uLensCenter);
	
	if ((vUv.x - uScreenCenter.x > 0.25 || vUv.x - uScreenCenter.x < -0.25)
		|| any(bvec2(clamp(tc, uScreenCenter - vec2(0.25,0.5), uScreenCenter + vec2(0.25,0.5)) - tc)))
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	else
		gl_FragColor = texture2D(uTex, vec2((tc.x + uOffset) * 2., tc.y));
}
