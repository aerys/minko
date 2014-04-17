#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4        diffuseColor;
uniform sampler2D   diffuseMap;
uniform samplerCube	diffuseCubeMap;

varying vec2 vertexUV;
varying vec3 vertexUVW;

varying vec4 reflectionPosition;

void main(void)
{
	// Transform the projective reflection texcoords to NDC space
	// and scale and offset xy to correctly sample a DX texture
	vec4 reflectionTexCoord = reflectionPosition;
	reflectionTexCoord.xyz /= reflectionTexCoord.w;
	reflectionTexCoord.x = 0.5f * reflectionTexCoord.x + 0.5f;
	reflectionTexCoord.y = -0.5f * reflectionTexCoord.y + 0.5f;
	// reflect more based on distance from the camera
	reflectionTexCoord.z = .1f / reflectionTexCoord.z;
	
	vec2 screen = (reflectionPosition.xy/reflectionPosition.z + 1.0)*0.5;
	
	#if defined(DIFFUSE_CUBEMAP)
		gl_FragColor	= textureCube(diffuseCubeMap, vertexUVW);
	#elif defined(DIFFUSE_MAP)
		//gl_FragColor 	= texture2D(diffuseMap, vertexUV);
		gl_FragColor 	= texture2D(diffuseMap, reflectionTexCoord.xy);		
		//gl_FragColor 	= texture2D(diffuseMap, screen);		
	#else
		gl_FragColor 	= diffuseColor;
	#endif
}

#endif // FRAGMENT_SHADER
