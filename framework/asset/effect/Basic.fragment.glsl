#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

#pragma include("Fog.function.glsl")

uniform vec4        diffuseColor;
uniform sampler2D   diffuseMap;
uniform samplerCube	diffuseCubeMap;

// alpha
uniform sampler2D 	alphaMap;
uniform float 		alphaThreshold;

varying vec2 vertexUV;
varying vec3 vertexUVW;

void main(void)
{
	vec4 	diffuse 		= diffuseColor;
	
	#if defined(DIFFUSE_CUBEMAP)
		diffuse		= textureCube(diffuseCubeMap, vertexUVW);
	#elif defined(DIFFUSE_MAP)
		diffuse 	= texture2D(diffuseMap, vertexUV);
	#endif
	
	#ifdef ALPHA_MAP
		diffuse.a = texture2D(alphaMap, vertexUV).r;
	#endif // ALPHA_MAP

	#ifdef ALPHA_THRESHOLD
		if (diffuse.a < alphaThreshold)
			discard;
	#endif // ALPHA_THRESHOLD
	
	gl_FragColor = fog_sampleFog(diffuse, gl_FragCoord);
}

#endif // FRAGMENT_SHADER
