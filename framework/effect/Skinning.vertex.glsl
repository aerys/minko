#ifdef VERTEX_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

attribute	vec3	position;
attribute	vec2 	uv;

attribute	vec4	boneIdsA;
attribute	vec4	boneIdsB;
attribute	vec4	boneWeightsA;
attribute	vec4	boneWeightsB;

uniform		mat4	modelToWorldMatrix;
uniform		mat4	worldToScreenMatrix;

varying 	vec2 	vertexUV;

varying 	vec4 	interpBoneIdsA;
//varying vec4 interpBoneIdsB;
//varying vec4 interpBoneWeightsA;
//varying vec4 interpBoneWeightsB;

void main(void)
{
	#ifdef DIFFUSE_MAP
		vertexUV = uv;
	#endif

	vec4 pos = vec4(position, 1.0);

	#ifdef MODEL_TO_WORLD
		pos = modelToWorldMatrix * pos;
	#endif

	gl_Position =  worldToScreenMatrix * pos;
	
	#ifdef IS_SKINNED
		// for debugging purposes only
		interpBoneIdsA = boneIdsA;
		//interpBoneIdsB = boneIdsB;
		//interpBoneWeightsA = boneWeightsA;
		//interpBoneWeightsB = boneWeightsB;
	#endif // IS_SKINNED
}

#endif // VERTEX_SHADER
