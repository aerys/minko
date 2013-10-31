#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uTex;
uniform vec2 uLensCenter;
uniform vec2 uScreenCenter;
uniform float uOffset;
uniform vec2 uScaleIn;
uniform vec2 uScale;
uniform vec4 uHmdWarpParam;

varying vec2 vUv;

vec2 HmdWarp(vec2 in01, vec2 LensCenter)
{
   vec2 theta = (in01 - LensCenter) * uScaleIn; // Scales to [-1, 1]
   float rSq = theta.x * theta.x + theta.y * theta.y;
   vec2 rvector = theta * (uHmdWarpParam.x + uHmdWarpParam.y * rSq +
	  uHmdWarpParam.z * rSq * rSq +
	  uHmdWarpParam.w * rSq * rSq * rSq);
	  
   return LensCenter + uScale * rvector;
}

void main(void)
{
	vec2 tc = HmdWarp(vUv, uLensCenter);
	
	if (any(bvec2(clamp(tc, uScreenCenter - vec2(0.25,0.5), uScreenCenter + vec2(0.25,0.5)) - tc)))
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	else
		gl_FragColor = texture2D(uTex, vec2((tc.x + uOffset) * 2, tc.y));

	/*
	vec2 tc = HmdWarp(vUv.xy, uLensCenter);
	
	if ((vUv.x - uScreenCenter.x > 0.25 || vUv.x - uScreenCenter.x < -0.25)
		|| any(bvec2(clamp(tc, uScreenCenter - vec2(0.25,0.5), uScreenCenter + vec2(0.25,0.5)) - tc)))
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	else
		gl_FragColor = texture2D(uTex, vec2((tc.x + uOffset) * 2., tc.y));
	*/
}
