#ifdef FRAGMENT_SHADER

#ifdef GL_ES
	precision mediump float;
#endif

uniform vec4        diffuseColor;
uniform sampler2D   diffuseMap;

varying vec2 vertexUV;

#ifdef IS_MODEL_SKINNED
varying vec4 interpBoneIdsA;
varying vec4 interpBoneIdsB;
varying vec4 interpBoneWeightsA;
varying vec4 interpBoneWeightsB;
#endif // IS_MODEL_SKINNED

void main(void)
{
#ifdef IS_MODEL_SKINNED
	gl_FragColor = vec4(interpBoneWeightsA.x * interpBoneIdsA.x / 8.0, interpBoneWeightsA.y * interpBoneIdsA.y / 8.0, interpBoneWeightsA.z * interpBoneIdsA.z / 8.0, 1.0);
#else
	gl_FragColor = vec4(0.8, 0.2, 0.2, 1.0);
#endif // IS_MODEL_SKINNED
	return;

	#ifdef DIFFUSE_MAP
		gl_FragColor = texture2D(diffuseMap, vertexUV);
	#else
		gl_FragColor = diffuseColor;
	#endif
}

#endif // FRAGMENT_SHADER
