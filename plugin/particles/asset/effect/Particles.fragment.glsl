#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4		diffuseColor;
uniform sampler2D	spritesheet;

uniform vec4		colorOverTimeStart;
uniform vec4 		colorOverTimeEnd;
uniform vec4		colorBySpeedStart;
uniform vec4 		colorBySpeedEnd;

varying vec2		vUV;
varying vec3		vColor;
varying float		vTime;
varying float		vVelocity;

#pragma include('Particles.function.glsl')

void main(void)
{			
	vec4 color = diffuseColor;

	#if defined(PARTICLE_COLOR)

		color.rgb = vColor;

	#endif // defined(PARTICLE_COLOR)

	#if defined(SPRITE_SHEET)

		color *= texture2D(spritesheet, vUV);

	#endif // defined(SPRITE_SHEET)

	#if defined(COLOR_OVER_TIME)

		color.rgb *= particles_linearlyInterpolateFloat3(
			vTime,
			colorOverTimeStart.w,
			colorOverTimeStart.xyz,
			colorOverTimeEnd.w,
			colorOverTimeEnd.xyz
		);

	#endif // defined(COLOR_OVER_TIME)

	#if defined(COLOR_BY_SPEED)

		color.rgb *= particles_linearlyInterpolateFloat3(
			vVelocity,
			colorBySpeedStart.w,
			colorBySpeedStart.xyz,
			colorBySpeedEnd.w,
			colorBySpeedEnd.xyz
		);

	#endif // defined(COLOR_BY_SPEED)

	gl_FragColor = color;
	return;
}