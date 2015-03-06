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

#include "minko/component/Skinning.hpp"
#include "minko/component/AbstractComponent.hpp"

#include <minko/scene/Node.hpp>
#include <minko/scene/NodeSet.hpp>
#include <minko/data/Provider.hpp>
#include <minko/geometry/Geometry.hpp>
#include <minko/geometry/Bone.hpp>
#include <minko/geometry/Skin.hpp>
#include <minko/render/AbstractContext.hpp>
#include <minko/component/Surface.hpp>
#include <minko/component/SceneManager.hpp>
#include <minko/component/MasterAnimation.hpp>
#include <minko/component/Animation.hpp>
#include <minko/component/Transform.hpp>

using namespace minko;
using namespace minko::data;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::geometry;
using namespace minko::render;

/*static*/ const unsigned int	Skinning::MAX_NUM_BONES_PER_VERTEX	= 8;
/*static*/ const std::string	Skinning::PNAME_NUM_BONES			= "numBones";
/*static*/ const std::string	Skinning::PNAME_BONE_MATRICES		= "boneMatrices";
/*static*/ const std::string	Skinning::ATTRNAME_POSITION			= "position";
/*static*/ const std::string	Skinning::ATTRNAME_NORMAL			= "normal";
/*static*/ const std::string	Skinning::ATTRNAME_BONE_IDS_A		= "boneIdsA";
/*static*/ const std::string	Skinning::ATTRNAME_BONE_IDS_B		= "boneIdsB";
/*static*/ const std::string	Skinning::ATTRNAME_BONE_WEIGHTS_A	= "boneWeightsA";
/*static*/ const std::string	Skinning::ATTRNAME_BONE_WEIGHTS_B	= "boneWeightsB";

Skinning::Skinning(const Skin::Ptr						skin, 
				   SkinningMethod						method,
				   AbstractContext::Ptr					context,
				   Node::Ptr							skeletonRoot,
				   bool									moveTargetBelowRoot,
				   bool									isLooping):
	AbstractAnimation(isLooping),
	_skin(skin),
	_context(context),
	_method(method),
	_skeletonRoot(skeletonRoot),
	_moveTargetBelowRoot(moveTargetBelowRoot),
	_boneVertexBuffer(nullptr),
	_targetGeometry(),
	_targetInputPositions(),
	_targetInputNormals(),
	_targetAddedSlot(nullptr)
{
}

Skinning::Skinning(const Skinning& skinning, const CloneOption& option) :
	AbstractAnimation(skinning, option),
	_skin(),
	_context(skinning._context),
	_method(skinning._method),
	_skeletonRoot(skinning._skeletonRoot),
	_moveTargetBelowRoot(skinning._moveTargetBelowRoot),
	_boneVertexBuffer(nullptr),
	_targetGeometry(),
	_targetInputPositions(),
	_targetInputNormals(),
	_targetAddedSlot(nullptr)
{	
	_skin = skinning._skin->clone();

	auto targetGeometry = skinning._targetGeometry;	

	for (auto it = targetGeometry.begin(); it != targetGeometry.end(); ++it)
	{
		_targetGeometry[it->first] = it->second->clone();
	}
}

AbstractComponent::Ptr
Skinning::clone(const CloneOption& option)
{
	auto skin = std::shared_ptr<Skinning>(new Skinning(*this, option));

	skin->initialize();	

	return skin;
}

void
Skinning::initialize()
{
	AbstractAnimation::initialize();

	if (_skin == nullptr)
		throw std::invalid_argument("skin");

	if (_context == nullptr)
		throw std::invalid_argument("context");

	if (_method != SkinningMethod::SOFTWARE && _skin->maxNumVertexBones() > MAX_NUM_BONES_PER_VERTEX)
	{
		std::cerr << "The maximum number of bones per vertex gets too high (" << _skin->maxNumVertexBones() 
			<< ") to propose hardware skinning (max allowed = " << MAX_NUM_BONES_PER_VERTEX << ")" 
			<< std::endl;

		_method	= SkinningMethod::SOFTWARE;
	}

	_boneVertexBuffer	= _method == SkinningMethod::SOFTWARE 
		? nullptr 
		: createVertexBufferForBones();

	_maxTime = _skin->duration();

	setPlaybackWindow(0, _maxTime)->seek(0);
}

void
Skinning::addedHandler(Node::Ptr node, Node::Ptr target, Node::Ptr parent)
{
	AbstractAnimation::addedHandler(node, target, parent);

	if (_skin->duration() == 0)
		return; // incorrect animation

	// FIXME
	if (node->components<Surface>().size() > 1)
		std::cerr << "Warning: The skinning component is not intended to work on node with several surfaces. Attempts to apply skinning to first surface." << std::endl;

	if (node->hasComponent<Surface>())
	{
		auto geometry = node->component<Surface>()->geometry();

		if (geometry->hasVertexAttribute(ATTRNAME_POSITION) 
			&& geometry->vertexBuffer(ATTRNAME_POSITION)->numVertices() == _skin->numVertices()
			&& !geometry->hasVertexBuffer(_boneVertexBuffer))
		{
			_targetGeometry[node]			= geometry;
			_targetInputPositions[node]		= geometry->vertexBuffer(ATTRNAME_POSITION)->data();			

			if (geometry->hasVertexAttribute(ATTRNAME_NORMAL)
				&& geometry->vertexBuffer(ATTRNAME_NORMAL)->numVertices() == _skin->numVertices())
				_targetInputNormals[node]	= geometry->vertexBuffer(ATTRNAME_NORMAL)->data();

			if (_method != SkinningMethod::SOFTWARE)
			{
				geometry->addVertexBuffer(_boneVertexBuffer);

                geometry->data()->set(PNAME_BONE_MATRICES, std::vector<math::mat4>());
				geometry->data()->set<int>(PNAME_NUM_BONES, 0);
			}
		}
	}
}

void
Skinning::removedHandler(Node::Ptr node, Node::Ptr target, Node::Ptr parent)
{
	AbstractAnimation::removedHandler(node, target, parent);

	if (_targetGeometry.count(target) > 0)
	{
		auto geometry	= _targetGeometry[target];

		if (_method != SkinningMethod::SOFTWARE)
		{
			geometry->removeVertexBuffer(_boneVertexBuffer);
			geometry->data()->unset(PNAME_BONE_MATRICES);
			geometry->data()->unset(PNAME_NUM_BONES);
		}

		_targetGeometry.erase(target);
	}
	if (_targetInputPositions.count(target) > 0)
		_targetInputPositions.erase(target);
	if (_targetInputNormals.count(target) > 0)
		_targetInputNormals.erase(target);
}

VertexBuffer::Ptr
Skinning::createVertexBufferForBones() const
{
	static const unsigned int vertexSize = 16;  // [bId0 bId1 bId2 bId3] [bId4 bId5 bId6 bId7] [bWgt0 bWgt1 bWgt2 bWgt3] [bWgt4 bWgt5 bWgt6 bWgt7]

	assert(_skin->maxNumVertexBones() <= MAX_NUM_BONES_PER_VERTEX);

	const unsigned int	numVertices	= _skin->numVertices();
	std::vector<float>	vertexData	(numVertices * vertexSize, 0.0f);

	unsigned int index	= 0;
	for (unsigned int vId = 0; vId < numVertices; ++vId)
	{
		const unsigned int numVertexBones = _skin->numVertexBones(vId);
	
		unsigned int j = 0;
		while(j < numVertexBones && j < (vertexSize >> 2))
		{
			vertexData[index + j] = (float)_skin->vertexBoneId(vId, j);
			++j;
		}
		index += (vertexSize >> 1);

		j = 0;
		while(j < numVertexBones && j < (vertexSize >> 2))
		{
			vertexData[index + j] = (float)_skin->vertexBoneWeight(vId, j);
			++j;
		}
		index += (vertexSize >> 1);
	}

#ifdef DEBUG_SKINNING
	assert(index == vertexData.size());
#endif // DEBUG_SKINNING

	auto vertexBuffer	= VertexBuffer::create(_context, vertexData);

	vertexBuffer->addAttribute(ATTRNAME_BONE_IDS_A,		4, 0);
	vertexBuffer->addAttribute(ATTRNAME_BONE_IDS_B,		4, 4);
	vertexBuffer->addAttribute(ATTRNAME_BONE_WEIGHTS_A,	4, 8);
	vertexBuffer->addAttribute(ATTRNAME_BONE_WEIGHTS_B,	4, 12);

	return vertexBuffer;
}

void
Skinning::update()
{
	const uint frameId = _skin->getFrameId(_currentTime);

	updateFrame(frameId, target());
}

void
Skinning::updateFrame(unsigned int frameId, Node::Ptr target)
{
	if (_targetGeometry.count(target) == 0)
		return;

	assert(frameId < _skin->numFrames());

	auto& geometry = _targetGeometry[target];
	const std::vector<math::mat4>&	boneMatrices = _skin->matrices(frameId);

	if (_method == SkinningMethod::HARDWARE)
	{
		if (!geometry->data()->hasProperty(PNAME_NUM_BONES) ||
			geometry->data()->get<int>(PNAME_NUM_BONES) != _skin->numBones())
			geometry->data()->set<int>(PNAME_NUM_BONES, _skin->numBones());
	
        geometry->data()->set(PNAME_BONE_MATRICES, boneMatrices);
        geometry->data()->set<int>(PNAME_NUM_BONES, _skin->numBones());
	}
	else
		performSoftwareSkinning(target, boneMatrices);
}

void
Skinning::performSoftwareSkinning(Node::Ptr					        target, 
								  const std::vector<math::mat4>&	boneMatrices)
{
#ifdef DEBUG_SKINNING
	assert(target && _targetGeometry.count(target) > 0 && _targetInputPositions.count(target) > 0);
#endif //DEBUG_SKINNING
	
	auto geometry = _targetGeometry[target];

	// transform positions
	auto xyzBuffer = geometry->vertexBuffer(ATTRNAME_POSITION);
    const auto& xyzAttr = xyzBuffer->attribute(ATTRNAME_POSITION);

	performSoftwareSkinning(xyzAttr, xyzBuffer, _targetInputPositions[target], boneMatrices, false);

	// transform normals
	if (geometry->hasVertexAttribute(ATTRNAME_NORMAL) && _targetInputNormals.count(target) > 0)
	{
		auto normalBuffer = geometry->vertexBuffer(ATTRNAME_NORMAL);
        const auto&	normalAttr = normalBuffer->attribute(ATTRNAME_NORMAL);

		performSoftwareSkinning(normalAttr, normalBuffer, _targetInputNormals[target], boneMatrices, true);
	}
}

void
Skinning::performSoftwareSkinning(const VertexAttribute&		 attr,
								  VertexBuffer::Ptr				 vertexBuffer, 
								  const std::vector<float>&		 inputData,
								  const std::vector<math::mat4>& boneMatrices,
								  bool							 doDeltaTransform)
{
#ifdef DEBUG_SKINNING
	assert(vertexBuffer && vertexBuffer->data().size() == inputData.size());
	assert(attr && std::get<1>(*attr) == 3);
	assert(boneMatrices.size() == (_skin->numBones() << 4));
#endif // DEBUG_SKINNING

	const unsigned int vertexSize = vertexBuffer->vertexSize();
	std::vector<float>&	outputData = vertexBuffer->data();
	const unsigned int numVertices = outputData.size() / vertexSize;
	
#ifdef DEBUG_SKINNING
	assert(numVertices == _skin->numVertices());
#endif // DEBUG_SKINNING

	unsigned int index = attr.offset;
	for (unsigned int vId = 0; vId < numVertices; ++vId)
	{
        math::vec4 v1 = math::vec4(inputData[index], inputData[index + 1], inputData[index + 2], 1.f);
        math::vec4 v2 = math::vec4(0.f);

		const unsigned int numVertexBones = _skin->numVertexBones(vId);
		for (unsigned int j = 0; j < numVertexBones; ++j)
		{
			unsigned int boneId = 0;
			float boneWeight = 0.0f;

			_skin->vertexBoneData(vId, j, boneId, boneWeight);

            const math::mat4& boneMatrix = (boneMatrices[boneId]);

			if (!doDeltaTransform)
			{
                v2 += boneWeight * (boneMatrix * v1);
			}
			else
			{
                v2 += math::vec4(boneWeight * (math::mat3(boneMatrix)) * math::vec3(v1), 0.f);
			}
		}

		outputData[index] = v2.x;
		outputData[index+1]	= v2.y;
		outputData[index+2]	= v2.z;

		index += vertexSize;
	}

	vertexBuffer->upload();
}

void
Skinning::targetAdded(Node::Ptr target)
{
    AbstractAnimation::targetAdded(target);

	// FIXME: in certain circumstances (deserialization from minko studio)
	// it may be necessary to move the target directly below the skeleton root
	// for which the skinning matrices have been computed.

	if (_skeletonRoot == nullptr || !_moveTargetBelowRoot)
		return;

	if (target->parent())
		target->parent()->removeChild(target);

	_skeletonRoot->addChild(target);

	if (target->hasComponent<Transform>())
		target->component<Transform>()->matrix(math::mat4(1.f));

	if (target->hasComponent<MasterAnimation>())
	{
		auto masterAnimation = target->component<MasterAnimation>();
		masterAnimation->initAnimations();
	}
}

void
Skinning::rebindDependencies(std::map<AbstractComponent::Ptr, AbstractComponent::Ptr>& componentsMap, std::map<NodePtr, NodePtr>& nodeMap, CloneOption option)
{
	_skeletonRoot = nodeMap[_skeletonRoot];

	auto oldSurface = _targetGeometry.begin()->first->component<Surface>();
	auto oldGeometry = oldSurface->geometry();

	std::dynamic_pointer_cast<Surface>(componentsMap[oldSurface])->geometry(oldGeometry->clone());
}
