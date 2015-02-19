/*
Copyright (c) 2014 Aerys

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

#include "minko/geometry/Skin.hpp"

#include <minko/scene/Node.hpp>
#include <minko/geometry/Bone.hpp>

using namespace minko;
using namespace minko::scene;
using namespace minko::geometry;

Skin::Skin(unsigned int numBones, unsigned int duration, unsigned int numFrames):
	_bones(numBones, nullptr),
	_numBones(numBones),
	_duration(duration),
	_timeFactor(duration > 0 ? numFrames / float(duration) : 0.0f),
	_boneMatricesPerFrame(numFrames, std::vector<math::mat4>(numBones)),
	_maxNumVertexBones(0),
	_numVertexBones(),
	_vertexBones(),
	_vertexBoneWeights()
{
}

Skin::Skin(const Skin& skin) :
	_bones(),
	_numBones(skin._numBones),
	_duration(skin._duration),
	_timeFactor(skin._timeFactor),
	_boneMatricesPerFrame(skin._boneMatricesPerFrame),
	_maxNumVertexBones(skin._maxNumVertexBones),
	_numVertexBones(skin._numVertexBones),
	_vertexBones(skin._vertexBones),
	_vertexBoneWeights(skin._vertexBoneWeights)
{

}

std::shared_ptr<Skin>
Skin::clone()
{
	auto skin = std::shared_ptr<Skin>(new Skin(*this));
	
	return skin;
}

void
Skin::matrix(unsigned int		frameId, 
			 unsigned int		boneId, 
			 const math::mat4&	value)
{
#ifdef DEBUG_SKINNING
	assert(frameId < numFrames() && boneId < numBones());
#endif // DEBUG_SKINNING

    _boneMatricesPerFrame[frameId][boneId] = value;
}

Skin::Ptr
Skin::reorganizeByVertices()
{
	_numVertexBones.clear();
	_vertexBones.clear();
	_vertexBoneWeights.clear();

	const unsigned int lastId		= lastVertexId();
	const unsigned int numVertices	= lastId + 1;
	const unsigned int numBones		= _bones.size();

	_numVertexBones		.resize(numVertices,			0);
	_vertexBones		.resize(numVertices * numBones, 0);
	_vertexBoneWeights	.resize(numVertices * numBones, 0.0f);

	for (unsigned int boneId = 0; boneId < numBones; ++boneId)
	{
		auto bone = _bones[boneId];

		const std::vector<unsigned short>&	vertexIds		= bone->vertexIds();
		const std::vector<float>&			vertexWeights	= bone->vertexWeights();

		for (unsigned int i = 0; i < vertexIds.size(); ++i)
			if (vertexWeights[i] > 0.0f)
			{
				const unsigned short	vId		= vertexIds[i];
#ifdef DEBUG_SKINNING
				assert(vId < numVertices);
#endif // DEBUG_SKINNING

				const unsigned int		j		= _numVertexBones[vId];
	
				++_numVertexBones[vId];
	
				const unsigned int		index	= vertexArraysIndex(vId, j);
				
				_vertexBones[index]				= boneId;
				_vertexBoneWeights[index]		= vertexWeights[i];
			}
	}

	_maxNumVertexBones = 0;
	for (unsigned int vId = 0; vId < numVertices; ++vId)
		_maxNumVertexBones = std::max(_maxNumVertexBones, _numVertexBones[vId]);

	return shared_from_this();
}

unsigned short
Skin::lastVertexId() const
{
	unsigned short lastId = 0;

	for (unsigned int boneId = 0; boneId < _bones.size(); ++boneId)
	{
		const std::vector<unsigned short>& vertexId = _bones[boneId]->vertexIds();

		for (unsigned int i = 0; i < vertexId.size(); ++i)
			lastId = std::max(lastId, vertexId[i]);
	}

	return lastId;
}

uint
Skin::getFrameId(uint time) const
{
	const int frameId	= (int)floorf(time * _timeFactor);

	return (uint)std::min(frameId, (int)numFrames() - 1);
}

void
Skin::vertexBoneData(unsigned int	vertexId, 
					 unsigned int	j, 
					 unsigned int&	boneId, 
					 float&			boneWeight) const
{
	const unsigned int index = vertexArraysIndex(vertexId, j);

	boneId		= _vertexBones[index];
	boneWeight	= _vertexBoneWeights[index];
}

unsigned int
Skin::vertexBoneId(unsigned int vertexId, unsigned int j) const
{
	return _vertexBones[vertexArraysIndex(vertexId, j)];
}

float 
Skin::vertexBoneWeight(unsigned int vertexId, unsigned int j) const
{
	return _vertexBoneWeights[vertexArraysIndex(vertexId, j)];
}

Skin::Ptr
Skin::disposeBones()
{
	_bones.clear();
	_bones.shrink_to_fit();

	return shared_from_this();
}