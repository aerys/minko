#ifdef GL_ES
	precision mediump float;
#endif

uniform sampler2D	uTex;
uniform vec2		uLensCenter;
uniform vec2		uScreenCorner;
uniform vec2		uScreenCenter;
uniform float		uOffset;
uniform vec2		uScaleIn;
uniform vec2		uScale;
uniform	vec4		uDistortionK;
uniform vec2		uScalePriorDistortion;
uniform vec2		uScaleAfterDistortion;
uniform vec2		uPixelOffset;

varying vec2 vUv;

float
distort(float r)
{
	float	r2	= r * r;
	float	r4	= r2 * r2;
	float	r6	= r4 * r2;
	
	return r * (
		uDistortionK.x 
		+ r2 * uDistortionK.y
		+ r4 * uDistortionK.z
		+ r6 * uDistortionK.w
	);
}

vec2 
distortUV(vec2 uv)
{
   vec2		vec				= (uv - uLensCenter) * uScalePriorDistortion; // in [-1, 1]
   float	vecLength		= length(vec);
   vec		/= vecLength;
   vec2		distortedVec	= distort(vecLength) * vec;
   
   return uLensCenter + distortedVec * uScaleAfterDistortion;
}

void 
main(void)
{
	vec2 uv				= distortUV(vUv); // same as in the Oculus Rift SDK
	
	if (abs(uScreenCenter.x - vUv.x) > 0.25 
		|| any(bvec2(clamp(uv, uScreenCenter - vec2(0.25, 0.5), uScreenCenter + vec2(0.25, 0.5)) - uv)))
		gl_FragColor 	= vec4(0.0, 0.0, 0.0, 1.0);
	else
	{
		// scale the texture coordinates to handle the fact
		// the split screens take the whole screen.
		vec2 finalUV	= vec2(2.0, 1.0) * (uv - uScreenCorner);
		
		gl_FragColor	= texture2D(uTex, finalUV);
		//gl_FragColor	= fxaa_texture2D(uTex, finalUV, uPixelOffset);
	}
}
