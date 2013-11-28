#ifdef VERTEX_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

attribute	vec3	position;
attribute	vec2 	uv;
attribute	vec3	normal;

uniform		mat4	modelToWorldMatrix;
uniform		mat4	worldToScreenMatrix;

varying 	vec2 	vertexUV;
varying		vec3	vertexNormal;

void main(void)
{
	#ifdef DIFFUSE_MAP
		vertexUV = uv;
	#endif

	vec4 posVector = vec4(position, 1.0);
	#ifdef HAS_NORMAL
		vec4 normalVec = vec4(normalize(normal), 0.0);
	#endif // HAS_NORMAL

	#ifdef NUM_BONES
	
		posVector = performSkinning(posVector);
		#ifdef HAS_NORMAL
			normalVec = normalize(performSkinning(normalVec));
		#endif // HAS_NORMAL
		
	#endif // NUM_BONES
	
	#ifdef MODEL_TO_WORLD
	
		posVector = modelToWorldMatrix * posVector;
		#ifdef HAS_NORMAL
			normalVec = modelToWorldMatrix * vec4(normalVec.xyz, 0.0);
		#endif // HAS_NORMAL
		
	#endif // MODEL_TO_WORLD

	gl_Position 	=  worldToScreenMatrix * posVector;
	
	#ifdef HAS_NORMAL
		vertexNormal = normalVec.xyz;
	#endif // HAS_NORMAL
}

#endif // VERTEX_SHADER
