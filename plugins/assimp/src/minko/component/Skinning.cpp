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

#include "Skinning.hpp"

#include <minko/scene/Node.hpp>
#include <minko/scene/NodeSet.hpp>
#include <minko/geometry/Geometry.hpp>
#include <minko/geometry/Bone.hpp>
#include <minko/geometry/Skin.hpp>
#include <minko/render/VertexBuffer.hpp>
#include <minko/math/Matrix4x4.hpp>
#include <minko/component/Surface.hpp>
#include <minko/component/SceneManager.hpp>

using namespace minko;
using namespace minko::scene;
using namespace minko::math;
using namespace minko::component;
using namespace minko::geometry;
using namespace minko::render;

Skinning::Skinning(const Skin::Ptr skin):
	AbstractComponent(),
	_skin(skin),
	_targetGeometry(),
	_targetStartTime(),
	_targetInputPositions(),
	_targetInputNormals(),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_frameBeginSlot(nullptr)
{

}

void
Skinning::updateFrame(Node::Ptr		target, 
					  unsigned int	frameId)
{
	if (_targetGeometry.count(target) == 0 || frameId >= _skin->numFrames())
		return;

	performSoftwareSkinning(target, _skin->matrices(frameId));
}

void
Skinning::performSoftwareSkinning(Node::Ptr							target, 
								 const std::vector<Matrix4x4::Ptr>&	skinningMatrices)
{
	static const std::string ATTRNAME_XYZ		= "position";
	static const std::string ATTRNAME_NORMAL	= "normal";

#ifdef DEBUG_SKINNING
	assert(target && _targetGeometry.count(target) > 0 && _targetInputPositions.count(target) > 0);
#endif //DEBUG_SKINNING
	
	auto geometry	= _targetGeometry[target];

	// transform positions
	auto						xyzBuffer	= geometry->vertexBuffer(ATTRNAME_XYZ);
	VertexBuffer::AttributePtr	xyzAttr		= nullptr;
	for (auto& attr : xyzBuffer->attributes())
		if (std::get<0>(*attr) == ATTRNAME_XYZ)
			xyzAttr = attr;

	performSoftwareSkinning(xyzAttr, xyzBuffer, _targetInputPositions[target], skinningMatrices, false);

	// transform normals
	if (geometry->hasVertexAttribute(ATTRNAME_NORMAL) && _targetInputNormals.count(target) > 0)
	{
		auto						normalBuffer	= geometry->vertexBuffer(ATTRNAME_NORMAL);
		VertexBuffer::AttributePtr	normalAttr		= nullptr;
		for (auto& attr : normalBuffer->attributes())
			if (std::get<0>(*attr) == ATTRNAME_NORMAL)
				normalAttr = attr;

		performSoftwareSkinning(normalAttr, normalBuffer, _targetInputNormals[target], skinningMatrices, true);
	}
}

void
Skinning::performSoftwareSkinning(VertexBuffer::AttributePtr			attr,
								 VertexBuffer::Ptr					vertexBuffer, 
								 const std::vector<float>&			inputData,
								 const std::vector<Matrix4x4::Ptr>&	skinningMatrices,
								 bool								doDeltaTransform)
{
#ifdef DEBUG_SKINNING
	assert(vertexBuffer && vertexBuffer->data().size() == inputData.size());
	assert(attr && std::get<1>(*attr) == 3);
	assert(skinningMatrices.size() == _skin->bones().size());
#endif // DEBUG_SKINNING

	const unsigned int	numBones		= _skin->bones().size();
	const unsigned int	vertexSize		= vertexBuffer->vertexSize();
	std::vector<float>&	outputData		= vertexBuffer->data();
	const unsigned int	numVertices		= outputData.size() / vertexSize;
	
#ifdef DEBUG_SKINNING
	assert(numVertices == _skin->numVertices());
#endif // DEBUG_SKINNING

	unsigned int index = std::get<2>(*attr);
	for (unsigned int vId = 0; vId < numVertices; ++vId)
	{
		const float x1 = inputData[index];
		const float y1 = inputData[index+1];
		const float z1 = inputData[index+2];

		float x2 = 0.0f;
		float y2 = 0.0f;
		float z2 = 0.0f;

		const unsigned int numVertexBones = _skin->numVertexBones(vId);
		for (unsigned int j = 0; j < numVertexBones; ++j)
		{
			unsigned int	boneId		= 0;
			float			boneWeight	= 0.0f;

			_skin->vertexBoneData(vId, j, boneId, boneWeight);

			const std::vector<float>&	skinningMatrix	= skinningMatrices[boneId]->data();

			if (!doDeltaTransform)
			{
				x2 += boneWeight * (skinningMatrix[0] * x1 + skinningMatrix[1] * y1 + skinningMatrix[2]  * z1 + skinningMatrix[3]);
				y2 += boneWeight * (skinningMatrix[4] * x1 + skinningMatrix[5] * y1 + skinningMatrix[6]  * z1 + skinningMatrix[7]);
				z2 += boneWeight * (skinningMatrix[8] * x1 + skinningMatrix[9] * y1 + skinningMatrix[10] * z1 + skinningMatrix[11]);
			}
			else
			{
				x2 += boneWeight * (skinningMatrix[0] * x1 + skinningMatrix[1] * y1 + skinningMatrix[2]  * z1);
				y2 += boneWeight * (skinningMatrix[4] * x1 + skinningMatrix[5] * y1 + skinningMatrix[6]  * z1);
				z2 += boneWeight * (skinningMatrix[8] * x1 + skinningMatrix[9] * y1 + skinningMatrix[10] * z1);
			}
		}

		//std::cout << "frameId = " << frameId << "\t(" << x1 << ", " << y1 << ", " << z1 << ")\t=> (" << x2 << ", " << y2 << ", " << z2 << ")" << std::endl;

		outputData[index]	= x2;
		outputData[index+1]	= y2;
		outputData[index+2]	= z2;

		index += vertexSize;
	}

	vertexBuffer->upload();
}


void
Skinning::frameBeginHandler(SceneManager::Ptr)
{
	const float time = clock() / (float)CLOCKS_PER_SEC;

	for (auto& target : targets())
		updateFrame(target, _skin->getFrameId(time - _targetStartTime[target]));
}

void
Skinning::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&Skinning::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&Skinning::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
Skinning::targetAddedHandler(AbstractComponent::Ptr, Node::Ptr target)
{
	_addedSlot	= target->added()->connect(std::bind(
		&Skinning::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot	= target->removed()->connect(std::bind(
		&Skinning::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));
}

void
Skinning::targetRemovedHandler(AbstractComponent::Ptr, Node::Ptr)
{
	_addedSlot		= nullptr;
	_removedSlot	= nullptr;
}

void
Skinning::addedHandler(Node::Ptr node, Node::Ptr target, Node::Ptr parent)
{
	findSceneManager();

	if (_skin->duration() < 1e-6f)
		return; // incorrect animation

	if (node->hasComponent<Surface>())
	{
		auto geometry	= node->component<Surface>()->geometry();

		if (geometry->hasVertexAttribute("position"))
		{
			_targetGeometry[node]			= geometry;
			_targetStartTime[node]			= (float)clock() / (float)CLOCKS_PER_SEC;

			_targetInputPositions[node]		= geometry->vertexBuffer("position")->data();			
			if (geometry->hasVertexAttribute("normal"))
				_targetInputNormals[node]	= geometry->vertexBuffer("normal")->data();
		}
	}
}

void
Skinning::removedHandler(Node::Ptr, Node::Ptr target, Node::Ptr)
{
	findSceneManager();

	if (_targetGeometry.count(target) > 0)
		_targetGeometry.erase(target);
	if (_targetStartTime.count(target) > 0)
		_targetStartTime.erase(target);
	if (_targetInputPositions.count(target) > 0)
		_targetInputPositions.erase(target);
	if (_targetInputNormals.count(target) > 0)
		_targetInputNormals.erase(target);
}

void
Skinning::findSceneManager()
{
	NodeSet::Ptr roots = NodeSet::create(targets())
		->roots()
		->where([](NodePtr node)
		{
			return node->hasComponent<SceneManager>();
		});

	if (roots->nodes().size() > 1)
		throw std::logic_error("Renderer cannot be in two separate scenes.");
	else if (roots->nodes().size() == 1)
		setSceneManager(roots->nodes()[0]->component<SceneManager>());		
	else
		setSceneManager(nullptr);
}

void
Skinning::setSceneManager(SceneManager::Ptr sceneManager)
{
	if (sceneManager)
	{
		_frameBeginSlot = sceneManager->frameBegin()->connect(std::bind(
			&Skinning::frameBeginHandler, 
			shared_from_this(), 
			std::placeholders::_1
		));
	}
	else if (_frameBeginSlot)
	{
		for (auto& target : targets())
		{
			//_started[target] = false;
			//stop(target);
		}

		_frameBeginSlot = nullptr;
	}
}
