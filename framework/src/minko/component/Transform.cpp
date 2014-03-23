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

#include "minko/component/Transform.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/data/Container.hpp"
#include "minko/data/StructureProvider.hpp"
#include "minko/component/SceneManager.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

Transform::Transform() :
	minko::component::AbstractComponent(),
	_matrix(1.),
	_modelToWorld(1.),
//	_worldToModel(1.),
	_data(data::StructureProvider::create("transform"))
{
}

void
Transform::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&Transform::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&Transform::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_data
		->set<mat4>("matrix", 				_matrix)
		->set<mat4>("modelToWorldMatrix", 	_modelToWorld);
}

void
Transform::targetAddedHandler(AbstractComponent::Ptr	ctrl,
							  scene::Node::Ptr			target)
{
	if (targets().size() > 1)
		throw std::logic_error("Transform cannot have more than one target.");
	if (target->component<Transform>(1) != nullptr)
		throw std::logic_error("A node cannot have more than one Transform.");

	target->data()->addProvider(_data);

	auto callback = std::bind(
		&Transform::addedOrRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	);

	_addedSlot = target->added()->connect(callback);
	//_removedSlot = target->removed()->connect(callback);

	addedOrRemovedHandler(nullptr, target, target->parent());
}

void
Transform::addedOrRemovedHandler(scene::Node::Ptr node,
								 scene::Node::Ptr target,
								 scene::Node::Ptr parent)
{
	if (!target->root()->component<RootTransform>())
		target->root()->addComponent(RootTransform::create());
}

void
Transform::targetRemovedHandler(AbstractComponent::Ptr ctrl,
								scene::Node::Ptr 		target)
{
	target->data()->removeProvider(_data);

	_addedSlot = nullptr;
	_removedSlot = nullptr;
}

void
Transform::RootTransform::initialize()
{
	_targetSlots.push_back(targetAdded()->connect(std::bind(
		&Transform::RootTransform::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	)));

	_targetSlots.push_back(targetRemoved()->connect(std::bind(
		&Transform::RootTransform::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	)));
}

void
Transform::RootTransform::targetAddedHandler(AbstractComponent::Ptr 	ctrl,
											 scene::Node::Ptr			target)
{
	_targetSlots.push_back(target->added()->connect(std::bind(
		&Transform::RootTransform::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->removed()->connect(std::bind(
		&Transform::RootTransform::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->componentAdded()->connect(std::bind(
		&Transform::RootTransform::componentAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->componentRemoved()->connect(std::bind(
		&Transform::RootTransform::componentRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));

	auto sceneManager = target->root()->component<SceneManager>();

	if (sceneManager != nullptr)
		_renderingBeginSlot = sceneManager->renderingBegin()->connect(std::bind(
			&Transform::RootTransform::renderingBeginHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
		), 1000.f);

	addedHandler(nullptr, target, target->parent());
}

void
Transform::RootTransform::targetRemovedHandler(AbstractComponent::Ptr 	ctrl,
											   scene::Node::Ptr			target)
{
	_targetSlots.clear();
	_renderingBeginSlot = nullptr;
}

void
Transform::RootTransform::componentAddedHandler(scene::Node::Ptr		node,
												scene::Node::Ptr 		target,
												AbstractComponent::Ptr	ctrl)
{
	auto sceneManager = std::dynamic_pointer_cast<SceneManager>(ctrl);

	if (sceneManager != nullptr)
		_renderingBeginSlot = sceneManager->renderingBegin()->connect(std::bind(
			&Transform::RootTransform::renderingBeginHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
		), 1000.f);
	else if (std::dynamic_pointer_cast<Transform>(ctrl) != nullptr)
		_invalidLists = true;
}

void
Transform::RootTransform::componentRemovedHandler(scene::Node::Ptr			node,
												  scene::Node::Ptr 			target,
												  AbstractComponent::Ptr	ctrl)
{
	auto sceneManager = std::dynamic_pointer_cast<SceneManager>(ctrl);

	if (sceneManager)
		_renderingBeginSlot = nullptr;
	else if (std::dynamic_pointer_cast<Transform>(ctrl) != nullptr)
		_invalidLists = true;
}

void
Transform::RootTransform::addedHandler(scene::Node::Ptr node,
									   scene::Node::Ptr target,
									   scene::Node::Ptr ancestor)
{
	auto descendants = scene::NodeSet::create(target->root())->descendants(false);
	for (auto descendant : descendants->nodes())
	{
		auto rootTransformCtrl = descendant->component<RootTransform>();

		if (rootTransformCtrl)
			descendant->removeComponent(rootTransformCtrl);
	}

	_invalidLists = true;
}

void
Transform::RootTransform::removedHandler(scene::Node::Ptr node,
									     scene::Node::Ptr target,
										 scene::Node::Ptr ancestor)
{
	_invalidLists = true;
}

void
Transform::RootTransform::updateTransformsList()
{
	unsigned int nodeId = 0;

	_idToNode.clear();
	_transforms.clear();
	_modelToWorld.clear();
	_numChildren.clear();
	_firstChildId.clear();
	_parentId.clear();
	_dirty.clear();

	auto descendants = scene::NodeSet::create(targets())
		->descendants(true, false)
		->where([](scene::Node::Ptr node)
		{
			return node->hasComponent<Transform>();
		});

	for (auto node : descendants->nodes())
	{
		auto transformCtrl  = node->component<Transform>();

		_nodeToId[node] = nodeId;

		_idToNode.push_back(node);
		_transforms.push_back(transformCtrl);
		_modelToWorld.push_back(&transformCtrl->_modelToWorld);
		_numChildren.push_back(0);
		_firstChildId.push_back(0);
		_dirty.push_back(true);

		auto ancestor = node->parent();
		while (ancestor != nullptr && _nodeToId.count(ancestor) == 0)
			ancestor = ancestor->parent();

		if (ancestor != nullptr)
		{
			auto ancestorId = _nodeToId[ancestor];

			_parentId.push_back(ancestorId);
			if (_numChildren[ancestorId] == 0)
				_firstChildId[ancestorId] = nodeId;
			_numChildren[ancestorId]++;
		}
		else
			_parentId.push_back(-1);

		++nodeId;
	}

	_invalidLists = false;
}

void
Transform::RootTransform::updateTransforms()
{
	unsigned int numNodes 	= _transforms.size();
	unsigned int nodeId 	= 0;

	while (nodeId < numNodes)
	{
		auto numChildren 				= _numChildren[nodeId];
		auto firstChildId 				= _firstChildId[nodeId];
		auto lastChildId 				= firstChildId + numChildren;
		auto parentId 					= _parentId[nodeId];
		auto parentDirty				= _dirty[nodeId];
		auto parentModelToWorldMatrix 	= _modelToWorld[nodeId];

		if (parentDirty && parentId == -1)
		{
			auto transform = _transforms[nodeId];

			*parentModelToWorldMatrix = transform->_matrix;
        	transform->_data->set("modelToWorldMatrix", *parentModelToWorldMatrix);
        }

		for (auto childId = firstChildId; childId < lastChildId; ++childId)
		{
			auto transform = _transforms[childId];
			auto modelToWorldMatrix = _modelToWorld[childId];

			if (parentDirty || _dirty[childId])
			{
				*modelToWorldMatrix = *parentModelToWorldMatrix * transform->_matrix;
				transform->_data->set("modelToWorldMatrix", *modelToWorldMatrix);
				_dirty[childId] = true;
			}
		}

       	_dirty[nodeId] = false;
		++nodeId;
	}
}

void
Transform::RootTransform::forceUpdate(scene::Node::Ptr node, bool updateTransformLists)
{
	if (_invalidLists || updateTransformLists)
		updateTransformsList();

	updateTransforms();

/*
	auto				targetNodeId	= _nodeToId[node];
	int					nodeId			= targetNodeId;
	auto				dirtyRoot		= -1;
	std::vector<uint>	path;

	// find the "dirty" root and build the path to get back to the target node
	while (nodeId >= 0)
	{
		if ((_transforms[nodeId]->_hasChanged)
			|| (nodeId != targetNodeId && _modelToWorld[nodeId]->_hasChanged))
			dirtyRoot = nodeId;

		path.push_back(nodeId);

		nodeId = _parentId[nodeId];
	}

	// update that path starting from the dirty root
	for (int i = path.size() - 1; i >= 0; --i)
	{
		auto dirtyNodeId	= path[i];
		auto parentId		= _parentId[dirtyNodeId];
		auto modelToWorld	= _modelToWorld[dirtyNodeId];

		modelToWorld->copyFrom(_transforms[dirtyNodeId]);
		if (parentId != -1)
			modelToWorld->append(_modelToWorld[parentId]);
		modelToWorld->_hasChanged = false;
	}
	*/
}

void
Transform::RootTransform::renderingBeginHandler(std::shared_ptr<SceneManager>				sceneManager, 
											    uint										frameId, 
												std::shared_ptr<render::AbstractTexture>	abstractTexture)
{
	if (_invalidLists)
		updateTransformsList();

	updateTransforms();
}
