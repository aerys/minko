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

Skinning::Skinning(const Skin& skin):
	AbstractComponent(),
	_skin(skin),
	_targetGeometry(),
	_targetStartTime(),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_frameBeginSlot(nullptr)
{

}

void
Skinning::update(Node::Ptr target, float time)
{
	// assume repeating behavior
	const float			currentTime = fmod(time - _targetStartTime[target], _skin.duration);
	const unsigned int	numFrames	= _skin.boneMatricesPerFrame.size();
	unsigned int		frameId		= (unsigned int)floorf(numFrames * currentTime / _skin.duration);
	frameId	= frameId % numFrames;

	updateFrame(target, frameId);
}

void
Skinning::updateFrame(Node::Ptr target, unsigned int frameId)
{
	if (_targetGeometry.count(target) == 0)
		return;

	const std::vector<Matrix4x4::Ptr>& skinningMatrices	= _skin.boneMatricesPerFrame[frameId];


	auto geometry		= _targetGeometry[target];
	auto vertexBuffer	= geometry->vertexBuffer("position");

	VertexBuffer::AttributePtr xyzAttr = nullptr;
	for (auto& attr : vertexBuffer->attributes())
		if (std::get<0>(*attr) == "position")
			xyzAttr = attr;
	
	//auto xyzAttr		= vertexBuffer->attribute("position"); // unresolved symbol ...

	const unsigned int numBones		= _skin.bones.size();
	const unsigned int vertexSize	= vertexBuffer->vertexSize();
	const unsigned int xyzSize		= std::get<1>(*xyzAttr);
	
	assert(xyzSize == 3); // Temporary

	const unsigned int	xyzOffset		= std::get<2>(*xyzAttr);

	std::vector<float>&	vertexData		= vertexBuffer->data();
	const unsigned int	numVertices		= vertexData.size() / xyzSize;

	assert(numVertices == _skin.numVertexBones.size());

	Vector4::Ptr vertex			= Vector4::create();
	Vector4::Ptr skinnedVertex	= Vector4::create();

	unsigned int index = xyzOffset;
	for (unsigned int vId = 0; vId < numVertices; ++vId)
	{
		const float x1 = vertexData[index];
		const float y1 = vertexData[index+1];
		const float z1 = vertexData[index+2];

		float x2 = 0.0f;
		float y2 = 0.0f;
		float z2 = 0.0f;

		const unsigned int numVertexBones = _skin.numVertexBones[vId];
		for (unsigned int j = 0; j < numVertexBones; ++j)
		{
			const unsigned int			index			= vId + numVertices * j; 
			const unsigned int			boneId			= _skin.vertexBones[index];
			const float					boneWeight		= _skin.vertexBoneWeights[index]; 
			const std::vector<float>&	skinningMatrix	= skinningMatrices[boneId]->data();

			x2 += skinningMatrix[0] * x1 + skinningMatrix[1] * y1 + skinningMatrix[2]  * z1;
			y2 += skinningMatrix[4] * x1 + skinningMatrix[5] * y1 + skinningMatrix[6]  * z1;
			z2 += skinningMatrix[8] * x1 + skinningMatrix[9] * y1 + skinningMatrix[10] * z1;
		}

		vertexData[index]	= x2;
		vertexData[index+1]	= y2;
		vertexData[index+2]	= z2;

		index += vertexSize;
	}

	vertexBuffer->upload();

	std::cout << "update for target: '" << target->name() << "' at frameid = " << frameId << std::endl;
}

void
Skinning::frameBeginHandler(SceneManager::Ptr)
{
	const float time = clock() / (float)CLOCKS_PER_SEC;

	for (auto& target : targets())
		update(target, time);
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

	if (_skin.duration < 1e-6f)
		return; // incorrect animation

	if (node->hasComponent<Surface>())
	{
		_targetGeometry[node]	= node->component<Surface>()->geometry();
		_targetStartTime[node]	= clock() / (float)CLOCKS_PER_SEC;
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
