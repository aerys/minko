#ifdef GL_ES
	precision mediump float;
#endif

attribute vec2	offset;
attribute vec3	position;
attribute float	size;
attribute vec3	color;
attribute float	time;
attribute vec3	oldPosition;
attribute float	rotation;
attribute float	spriteIndex;

uniform mat4	modelToWorldMatrix;
uniform mat4	viewMatrix;
uniform mat4	projectionMatrix;
uniform vec2	spritesheetSize;

uniform float 	timeStep;
uniform vec4	sizeOverTime;
uniform	vec4	sizeBySpeed;

varying vec2	vUV;
varying vec3	vColor;
varying float	vTime;
varying float	vVelocity;

#pragma include('Particles.function.glsl')

void main(void)
{
	vec2	particleUV			= vec2(0.0);
	float	particleTime 		= 0.0;
	float 	particleVelocity 	= 0.0;
	vec3	particleColor		= vec3(1.0);


	#if defined(PARTICLE_TIME)

		particleTime = time;

	#endif // defined(PARTICLE_TIME)

	#if defined(PARTICLE_OLD_POSITION)

		particleVelocity = particles_velocity(position, oldPosition, timeStep);

	#endif // defined(PARTICLE_OLD_POSITION)

	#if defined(PARTICLE_COLOR)

		particleColor = color;

	#endif // defined(PARTICLE_COLOR)

	#if defined(SPRITE_SHEET)

		particleUV = vec2(0.5 + offset.x, 0.5 - offset.y);

		#if defined(PARTICLE_SPRITE_INDEX)

			float	particleIndex 	= mod(spriteIndex, spritesheetSize.x * spritesheetSize.y); // index = i + #cols * j	
			vec2 	ij				= vec2(
				floor(mod(spriteIndex, spritesheetSize.x)), 
				floor(spriteIndex / spritesheetSize.x)
			);

			particleUV = (particleUV + ij) / spritesheetSize;

		#endif // defined(PARTICLE_SPRITE_INDEX)

	#endif // defined(SPRITE_SHEET)


	vec4 pos = vec4(position, 1.0);

	#if !defined(WORLDSPACE_PARTICLES) && defined(MODEL_TO_WORLD)

		pos =  modelToWorldMatrix * pos;

	#endif
	
	pos = viewMatrix * pos; // pos is in eye space -> offset positions


	vec2 particleOffset = offset;

	#if defined(PARTICLE_ROTATION)

		vec4 offXY_cos_sin = vec4(particleOffset.x, particleOffset.y, cos(rotation), sin(rotation)); // less temp registers !

		particleOffset.xy = vec2(
			offXY_cos_sin.z * offXY_cos_sin.x - offXY_cos_sin.w * offXY_cos_sin.y, // cos * x - sin * y
			offXY_cos_sin.w * offXY_cos_sin.x + offXY_cos_sin.z * offXY_cos_sin.y  // sin * x + cos * y 
		);

	#endif // defined(PARTICLE_ROTATION)


	#if defined(PARTICLE_SIZE)

		particleOffset *= vec2(size);

	#endif // defined(PARTICLE_SIZE) 

	#if defined(SIZE_OVER_TIME)

		particleOffset *= particles_linearlyInterpolateFloat(
			particleTime,
			sizeOverTime.x,
			sizeOverTime.y,
			sizeOverTime.z,
			sizeOverTime.w
		);

	#endif // defined(SIZE_OVER_TIME)

	#if defined(SIZE_BY_SPEED)

		particleOffset *= particles_linearlyInterpolateFloat(
			particleVelocity,
			sizeBySpeed.x,
			sizeBySpeed.y,
			sizeBySpeed.z,
			sizeBySpeed.w
		);

	#endif // defined(SIZE_BY_SPEED)

	vUV 		= particleUV;
	vTime 		= particleTime;
	vVelocity	= particleVelocity;
	vColor 		= particleColor;

	gl_Position	= projectionMatrix * (pos + vec4(particleOffset, 0.0, 0.0));
}