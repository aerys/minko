/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Skin.hpp"

#include <minko/scene/Node.hpp>
#include <minko/math/Matrix4x4.hpp>
#include
using namespace minko;
using namespace minko::scene;
using namespace minko::math;
using namespace minko::geometry;

void
Skin::clear()
{
	_bones.clear();
	_boneMatricesPerFrame.clear();
	_duration = 0.0f;
	_numVertexBones.clear();
	_vertexBones.clear();
	_vertexBoneWeights.clear();
}

void
Skin::reorganizeByVertices()
{
	numVertexBones.clear();
	vertexBones.clear();
	vertexBoneWeights.clear();

	const unsigned int lastId		= lastVertexId();
	const unsigned int numVertices	= lastId + 1;
	const unsigned int numBones		= bones.size();

	numVertexBones		.resize(numVertices,			0);
	vertexBones			.resize(numVertices * numBones, 0);
	vertexBoneWeights	.resize(numVertices * numBones, 0.0f);

	for (unsigned int boneId = 0; boneId < numBones; ++boneId)
	{
		auto				bone			= bones[boneId];
		const unsigned int	numBoneVertices = bone->vertexIds().size();

		assert(bone.vertexWeight.size() == numBoneVertices);

		for (unsigned int i = 0; i < numBoneVertices; ++i)
		{
			const unsigned short	vId		= bone.vertexId[i];
			const unsigned int		index	= vId + numVertices * numVertexBones[vId];

			vertexBones[index]			= boneId;
			vertexBoneWeights[index]	= bone.vertexWeight[i];

			++numVertexBones[vId];
		}
	}
}

unsigned short
Skin::lastVertexId() const
{
	unsigned short lastId = 0;

	for (unsigned int boneId = 0; boneId < bones.size(); ++boneId)
	{
		const std::vector<unsigned short>& vertexId = bones[boneId].vertexId;
		for (unsigned int i = 0; i < vertexId.size(); ++i)
			lastId = std::max(lastId, vertexId[i]);
	}

	return lastId;
}

Bone::Ptr
Skin::bone(unsigned int i)
{

}