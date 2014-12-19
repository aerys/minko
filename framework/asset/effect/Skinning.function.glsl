#if defined(VERTEX_SHADER) && defined(NUM_BONES)

	uniform 	mat4	boneMatrices[NUM_BONES];
	attribute	vec4	boneIdsA;
	attribute	vec4	boneIdsB;
	attribute	vec4	boneWeightsA;
	attribute	vec4	boneWeightsB;

	vec4 skinning_moveVertex(vec4 inputVec)
	{
		return ( 
			boneWeightsA.x * boneMatrices[int(boneIdsA.x)] + 
			boneWeightsA.y * boneMatrices[int(boneIdsA.y)] + 
			boneWeightsA.z * boneMatrices[int(boneIdsA.z)] + 
			boneWeightsA.w * boneMatrices[int(boneIdsA.w)] + 
			boneWeightsB.x * boneMatrices[int(boneIdsB.x)] + 
			boneWeightsB.y * boneMatrices[int(boneIdsB.y)] + 
			boneWeightsB.z * boneMatrices[int(boneIdsB.z)] + 
			boneWeightsB.w * boneMatrices[int(boneIdsB.w)] 
			) * inputVec;	
	}

#endif // defined(VERTEX_SHADER) && defined(NUM_BONES)
