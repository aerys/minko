#ifdef GL_ES
	precision mediump float;
#endif

vec4 
fxaa_texture2D(sampler2D textureSampler, vec2 uv, vec2 uvOffset) 
{
	// The parameters are hardcoded for now, but could be
	// made into uniforms to control from the program.
	float 	FXAA_SPAN_MAX	= 8.0;
	float 	FXAA_REDUCE_MUL	= 1.0/8.0;
	float 	FXAA_REDUCE_MIN	= 1.0/128.0;
	vec3	luma			= vec3(0.299, 0.587, 0.114);

	vec4	rgbaNW	= texture2D(textureSampler, uv + (vec2(-1.0, -1.0) * uvOffset));
	vec4	rgbaNE	= texture2D(textureSampler, uv + (vec2(+1.0, -1.0) * uvOffset));
	vec4	rgbaSW	= texture2D(textureSampler, uv + (vec2(-1.0, +1.0) * uvOffset));
	vec4	rgbaSE	= texture2D(textureSampler, uv + (vec2(+1.0, +1.0) * uvOffset));
	vec4	rgbaM	= texture2D(textureSampler, uv);
	
	float	lumaNW	= dot(rgbaNW.rgb, luma);
	float	lumaNE	= dot(rgbaNE.rgb, luma);
	float	lumaSW	= dot(rgbaSW.rgb, luma);
	float	lumaSE	= dot(rgbaSE.rgb, luma);
	float	lumaM	= dot(rgbaM.rgb, luma);
	
	float lumaMin	= min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax	= max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	
	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	
	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN); 
	float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	
	dir = min(vec2(FXAA_SPAN_MAX,  FXAA_SPAN_MAX), 
		max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * uvOffset;
		
	vec3 rgbA = (1.0/2.0) * (
		texture2D(textureSampler, uv + dir * (1.0/3.0 - 0.5)).xyz +
		texture2D(textureSampler, uv + dir * (2.0/3.0 - 0.5)).xyz
	);
	vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
		texture2D(textureSampler, uv + dir * (0.0/3.0 - 0.5)).xyz +
		texture2D(textureSampler, uv + dir * (3.0/3.0 - 0.5)).xyz
	);
	float lumaB = dot(rgbB, luma);

	vec4 ret = rgbaM;
  
	if((lumaB < lumaMin) || (lumaB > lumaMax))
		ret.xyz	= rgbA;
	else 
		ret.xyz	= rgbB;
		
	return ret;
}
