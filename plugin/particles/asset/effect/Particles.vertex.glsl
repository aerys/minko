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

uniform float	spriteSheetRows;
uniform float	spriteSheetColumns;

varying vec2	vUV;
varying vec3	vColor;
varying float	vTime;

void main(void)
{
	float	particleTime 	= 0.0;
	vec3	particleColor	= vec3(1.0);

	#ifdef PARTICLE_TIME

		particleTime = time;

	#endif // PARTICLE_TIME

	#ifdef PARTICLE_COLOR

		particleColor = color;

	#endif // PARTICLE_COLOR


	/*****
	#ifdef TECHNIQUE_DIFFUSE_MAP
		vertexUV = vec2(offset.x, -offset.y) + 0.5;
	
		#ifdef SPRITE_SHEET
			float index;
			float r;
			float c;
		
			index = mod(floor(spriteIndex), spriteSheetRows * spriteSheetColumns);
		
		//x - y * floor(x/y).
		// r = modf(index/spriteSheetRows, c)

			//r = floor(index / spriteSheetRows);
			r = fract(index / spriteSheetRows);
			c = index - spriteSheetRows * floor(index / spriteSheetRows);
			c /= spriteSheetColumns;
		
			vertexUV /= vec2(spriteSheetRows, spriteSheetColumns);
			vertexUV += vec2(r, c);
		#endif
	#endif
	****/


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


	/****
	float cosine = cos(rotation);
	float sine = sin(rotation);
	
	vec2 sinOffset = vec2(cosine * offset.x - sine * offset.y,
				  sine * offset.x + cosine * offset.y);

	// fake SizeOverTime
	float sizeOverTimer = size * (1.6 - 1.3 * pow(time, 2.5));

	// Sponza scale
	//size *= 0.05;

	pos += vec4(sinOffset, 0, 0) * sizeOverTimer;
	****/

	vTime 		= particleTime;
	vColor 		= particleColor;
	gl_Position	= projectionMatrix * (pos + vec4(particleOffset, 0.0, 0.0));
}