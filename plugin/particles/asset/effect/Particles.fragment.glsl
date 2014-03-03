#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4		diffuseColor;
uniform sampler2D	diffuseMap;
	
varying vec2	vUV;
varying vec3	vColor;
varying float	vTime;

void main(void)
{			
	vec4 color = vec4(vColor, 1.0);
	gl_FragColor = color;
	return;

	#ifdef DIFFUSE_MAP
		color *= texture2D(diffuseMap, vertexUV);
	#endif
	
	#ifdef TECHNIQUE_DIFFUSE_COLOR
		color *= vec4(diffuseColor);
	#endif
	
	// fake ColorOverTime
	color *= 1.0 - pow(vTime, 4.);
	
	gl_FragColor = color;
}