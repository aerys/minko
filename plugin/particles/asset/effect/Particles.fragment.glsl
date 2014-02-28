#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4 diffuseColor;
uniform sampler2D diffuseMap;
	
varying vec2 vertexUV;
varying float particleTime;

void main(void)
{			
	vec4 c;

	c = vec4(1., 1., 1., 1.);

	#ifdef TECHNIQUE_DIFFUSE_MAP
		c *= texture2D(diffuseMap, vertexUV);
	#endif
	
	#ifdef TECHNIQUE_DIFFUSE_COLOR
		c *= vec4(diffuseColor);
	#endif
	
	// fake ColorOverTime
	c *= 1.0 - pow(particleTime, 4.);
	
	gl_FragColor = c;
}