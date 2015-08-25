#if defined(VERTEX_SHADER) && defined(SKINNING_NUM_BONES) && SKINNING_NUM_BONES != 0

	uniform mat4 uBoneMatrices[SKINNING_NUM_BONES];
	attribute vec4 aBoneIdsA;
	attribute vec4 aBoneIdsB;

	vec4 skinning_moveVertex(vec4 inputVec,
							 vec4 boneWeightsA,
							 vec4 boneWeightsB)
	{
		return (
			boneWeightsA.x * uBoneMatrices[int(aBoneIdsA.x)] +
			boneWeightsA.y * uBoneMatrices[int(aBoneIdsA.y)] +
			boneWeightsA.z * uBoneMatrices[int(aBoneIdsA.z)] +
			boneWeightsA.w * uBoneMatrices[int(aBoneIdsA.w)] +
			boneWeightsB.x * uBoneMatrices[int(aBoneIdsB.x)] +
			boneWeightsB.y * uBoneMatrices[int(aBoneIdsB.y)] +
			boneWeightsB.z * uBoneMatrices[int(aBoneIdsB.z)] +
			boneWeightsB.w * uBoneMatrices[int(aBoneIdsB.w)]
		) * inputVec;
	}

#endif // defined(VERTEX_SHADER) && defined(SKINNING_NUM_BONES)
