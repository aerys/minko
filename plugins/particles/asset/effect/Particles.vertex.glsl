#ifdef GL_ES
	precision mediump float;
#endif

attribute vec2 offset;
attribute vec3 position;
attribute float size;
attribute vec3 color;
attribute float time;
attribute vec3 oldPosition;
attribute float rotation;
attribute float spriteIndex;

uniform mat4 modelToWorldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float spriteSheetRows;
uniform float spriteSheetColumns;

varying vec2 vertexUV;
varying float particleTime;

void main(void)
{
	particleTime = time;

	vec4 pos;
	
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
	
	#ifdef WORLDSPACE_PARTICLES	
		pos = viewMatrix * vec4(position, 1.0);
	#else
		pos = vec4(position, 1.0);
		
		#ifdef HAS_MODEL_TO_WORLD
			pos =  modelToWorldMatrix * pos;
		#endif
		
		pos = viewMatrix * pos;
	#endif
	
	float cosine = cos(rotation);
	float sine = sin(rotation);
	
	vec2 sinOffset = vec2(cosine * offset.x - sine * offset.y,
				  sine * offset.x + cosine * offset.y);

	// fake SizeOverTime
	float sizeOverTimer = size * (1.6 - 1.3 * pow(time, 2.5));

	// Sponza scale
	//size *= 0.05;

	pos += vec4(sinOffset, 0, 0) * sizeOverTimer;

	gl_Position = projectionMatrix * pos;
}