#if defined(VERTEX_SHADER) && defined(NUM_BONES)

	uniform 	mat4	boneMatrices[NUM_BONES];
	attribute	vec4	boneIdsA;
	attribute	vec4	boneIdsB;
	attribute	vec4	boneWeightsA;
	attribute	vec4	boneWeightsB;

	vec4 skinning_moveVertex(vec4 inputVec)
	{
		vec4 outputVec = vec4(0.0);
	
		outputVec += boneWeightsA.x * boneMatrices[int(boneIdsA.x)] * inputVec;
		outputVec += boneWeightsA.y * boneMatrices[int(boneIdsA.y)] * inputVec;
		outputVec += boneWeightsA.z * boneMatrices[int(boneIdsA.z)] * inputVec;
		outputVec += boneWeightsA.w * boneMatrices[int(boneIdsA.w)] * inputVec;
	
		outputVec += boneWeightsB.x * boneMatrices[int(boneIdsB.x)] * inputVec;
		outputVec += boneWeightsB.y * boneMatrices[int(boneIdsB.y)] * inputVec;
		outputVec += boneWeightsB.z * boneMatrices[int(boneIdsB.z)] * inputVec;
		outputVec += boneWeightsB.w * boneMatrices[int(boneIdsB.w)] * inputVec;
	
		return outputVec;
	}

#endif // defined(VERTEX_SHADER) && defined(NUM_BONES)