#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4		diffuseColor;
uniform sampler2D	spritesheet;
	
varying vec2		vUV;
varying vec3		vColor;
varying float		vTime;

void main(void)
{			
	vec4 color = diffuseColor;

	#if defined(PARTICLE_COLOR)

		color.rgb = vColor;

	#endif // defined(PARTICLE_COLOR)

	#if defined(SPRITE_SHEET)

		color *= texture2D(spritesheet, vUV);

	#endif // defined(SPRITE_SHEET)

	//vec4 color = vec4(vColor, 1.0);
	gl_FragColor = color;
	return;

	// #ifdef DIFFUSE_MAP
	// 	color *= texture2D(spritesheet, vertexUV);
	// #endif
	
	// #ifdef TECHNIQUE_DIFFUSE_COLOR
	// 	color *= vec4(diffuseColor);
	// #endif
	
	// // fake ColorOverTime
	// color *= 1.0 - pow(vTime, 4.);
	
	// gl_FragColor = color;
}