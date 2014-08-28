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

#include "minko/component/Transform.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/data/Container.hpp"
#include "minko/component/SceneManager.hpp"

using namespace minko;
using namespace minko::component;

Transform::Transform() :
	minko::component::AbstractComponent(),
	_matrix(nullptr),
    _modelToWorld(nullptr),
//	_worldToModel(1.),
	_data(data::Provider::create())
{
	_data
		->set<math::mat4>("matrix", 			math::mat4(1.f))
        ->set<math::mat4>("modelToWorldMatrix", math::mat4(1.f));

    _matrix = _data->getUnsafePointer<math::mat4>("matrix");
    _modelToWorld = _data->getUnsafePointer<math::mat4>("modelToWorldMatrix");
}

void
Transform::targetAdded(scene::Node::Ptr	target)
{
	if (target->component<Transform>(1) != nullptr)
		throw std::logic_error("A node cannot have more than one Transform.");

	target->data().addProvider(_data);

	auto callback = std::bind(
		&Transform::addedOrRemovedHandler,
		std::static_pointer_cast<Transform>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	);

	_addedSlot = target->added().connect(callback);
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
Transform::targetRemoved(scene::Node::Ptr target)
{
	target->data().removeProvider(_data);

	_addedSlot = nullptr;
	_removedSlot = nullptr;
}

void
Transform::RootTransform::targetAdded(scene::Node::Ptr target)
{
	_targetSlots.push_back(target->added().connect(std::bind(
		&Transform::RootTransform::addedHandler,
		std::static_pointer_cast<RootTransform>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->removed().connect(std::bind(
		&Transform::RootTransform::removedHandler,
		std::static_pointer_cast<RootTransform>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->componentAdded().connect(std::bind(
		&Transform::RootTransform::componentAddedHandler,
		std::static_pointer_cast<RootTransform>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->componentRemoved().connect(std::bind(
		&Transform::RootTransform::componentRemovedHandler,
		std::static_pointer_cast<RootTransform>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));

	auto sceneManager = target->root()->component<SceneManager>();

	if (sceneManager != nullptr)
		_renderingBeginSlot = sceneManager->renderingBegin()->connect(std::bind(
			&Transform::RootTransform::renderingBeginHandler, 
			std::static_pointer_cast<RootTransform>(shared_from_this()), 
			std::placeholders::_1, 
			std::placeholders::_2, 
			std::placeholders::_3
		), 1000.f);

	addedHandler(target, target->root(), target->parent());
}

void
Transform::RootTransform::targetRemoved(scene::Node::Ptr target)
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
			std::static_pointer_cast<RootTransform>(shared_from_this()), 
			std::placeholders::_1, 
			std::placeholders::_2, 
			std::placeholders::_3
		), 1000.f);
    else
    {
        if (std::dynamic_pointer_cast<Transform>(ctrl) != nullptr)
        {
            auto removeIt = std::find(_toRemove.begin(), _toRemove.end(), target);

            if (removeIt != _toRemove.end())
                _toRemove.erase(removeIt);
            else
            {
                _toAdd.push_back(target);
                _invalidLists = true;
            }
        }
    }
}

void
Transform::RootTransform::componentRemovedHandler(scene::Node::Ptr			node,
												  scene::Node::Ptr 			target,
												  AbstractComponent::Ptr	ctrl)
{
	auto sceneManager = std::dynamic_pointer_cast<SceneManager>(ctrl);

	if (sceneManager)
		_renderingBeginSlot = nullptr;
    else
    {
        if (std::dynamic_pointer_cast<Transform>(ctrl) != nullptr)
        {
            auto addIt = std::find(_toAdd.begin(), _toAdd.end(), target);

            if (addIt != _toAdd.end())
                _toAdd.erase(addIt);
            else
            {
                _toRemove.push_back(target);
                _invalidLists = true;
            }
        }
    }
}

void
Transform::RootTransform::addedHandler(scene::Node::Ptr node,
									   scene::Node::Ptr target,
									   scene::Node::Ptr ancestor)
{
    if (node->root() == this->target() && node != target)
    {
        auto otherRoot = target->component<RootTransform>();

        if (otherRoot != nullptr)
        {
            _toAdd.insert(_toAdd.begin(), otherRoot->_nodes.begin(), otherRoot->_nodes.end());
            _toAdd.insert(_toAdd.begin(), otherRoot->_toAdd.begin(), otherRoot->_toAdd.end());
            for (const auto& toRemove : _toRemove)
                _toAdd.remove(toRemove);
            _invalidLists = true;
        
            target->removeComponent(otherRoot);
        }
    }
}

void
Transform::RootTransform::removedHandler(scene::Node::Ptr node,
									     scene::Node::Ptr target,
										 scene::Node::Ptr ancestor)
{
	_invalidLists = true;

    auto withTransforms = scene::NodeSet::create(target)
        ->descendants(true, false)
        ->where([](scene::Node::Ptr n){ return n->hasComponent<Transform>(); });
    _toRemove.insert(_toRemove.begin(), withTransforms->nodes().begin(), withTransforms->nodes().end());
}

void
Transform::RootTransform::updateTransformsList()
{
    for (const auto& toRemove : _toRemove)
        _nodeToId.erase(toRemove);
    _nodes.clear();
    for (const auto& nodeAndId : _nodeToId)
        _nodes.push_back(nodeAndId.first);
    for (const auto& node : _toAdd)
        _nodes.push_back(node);
    _toAdd.clear();
    _toRemove.clear();

    _matrix.resize(_nodes.size());
    _modelToWorld.resize(_nodes.size());
    _numChildren.resize(_nodes.size(), -1);
    _dirty.resize(_nodes.size(), true);
    _parentId.resize(_nodes.size(), -1);
    _providers.resize(_nodes.size(), nullptr);

    _firstChildId.clear();
    _firstChildId.resize(_nodes.size(), -1);

    sortNodes();

    auto nodeId = 0;
    for (const auto& node : _nodes)
	{
		_nodeToId[node] = nodeId;
        _matrix[nodeId] = node->data().getPointer<math::mat4>("matrix");
		_modelToWorld[nodeId] = node->data().getUnsafePointer<math::mat4>("modelToWorldMatrix");
        _providers[nodeId] = node->component<Transform>()->_data;

        ++nodeId;
	}

	_invalidLists = false;
}

void
Transform::RootTransform::sortNodes()
{
    auto nodeIt = _nodes.begin();

	// assumes 'nodes' is the result of a breadth-first search from the nodes
    for (uint nodeId = 0; nodeId < _nodes.size(); ++nodeId)
	{
		auto node = *nodeIt;
        auto ancestor = node->parent();

        while (ancestor != nullptr && !ancestor->hasComponent<Transform>())
            ancestor = ancestor->parent();

        if (!ancestor)
        {
            _parentId[nodeId] = -1;
            continue;
        }

        // find ancestor's actual it and index
        auto ancestorId = 0u;
        for (const auto& node : _nodes)
        {
            if (node == ancestor)
                break;

            ++ancestorId;            
        }
        auto ancestorIt = std::next(_nodes.begin(), ancestorId);

        if (_firstChildId[ancestorId] == -1)
        {
            if (ancestorId > nodeId)
            {
                _nodes.erase(ancestorIt);
                _nodes.insert(nodeIt, ancestor);
                _parentId[nodeId + 1] = nodeId;
                _numChildren[nodeId] = 1;
                _firstChildId[nodeId] = nodeId + 1;
                _dirty[nodeId] = true;
                ++nodeId;
            }
            else
            {
                _parentId[nodeId] = ancestorId;
                _firstChildId[ancestorId] = nodeId;
                _numChildren[ancestorId] = 1;
            }
        }
        else
		{
            _parentId[nodeId] = ancestorId;
            _numChildren[nodeId] = 0;
            _firstChildId[nodeId] = -1;
            _nodes.erase(nodeIt);
            _nodes.insert(std::next(ancestorIt, _numChildren[ancestorId]), node);
            ++_numChildren[ancestorId];
		}

        ++nodeIt;
	}
}

void
Transform::RootTransform::updateTransforms()
{
    math::mat4 modelToWorldMatrix;
    uint nodeId = 0;

    for (const auto& node : _nodes)
	{
		if (_dirty[nodeId])
		{
			auto parentId = _parentId[nodeId];

			if (parentId < 0)
				modelToWorldMatrix = *_matrix[nodeId];
			else
				modelToWorldMatrix = *_modelToWorld[parentId] * *_matrix[nodeId];

            // Because we use an unsafe pointer that gives us a direct access to the
            // data provider internal value for "modelToWorldMatrix", we have to trigger
            // the "property changed" signal manually.
            // This technique completely bypasses the container property name resolving
            // mechanism and is a lot faster.
            if (*_modelToWorld[nodeId] != modelToWorldMatrix)
            {
                auto& nodeData = node->data();
                auto provider = _providers[nodeId];

                // manually update the data provider internal mat4 object
                *_modelToWorld[nodeId] = modelToWorldMatrix;

                // execute the "property changed" signal(s) manually
                nodeData.propertyChanged().execute(nodeData, provider, "modelToWorldMatrix");
                if (nodeData.hasPropertyChangedSignal("modelToWorldMatrix"))
                    nodeData.propertyChanged("modelToWorldMatrix").execute(
                        nodeData,
                        provider,
                        "modelToWorldMatrix"
                    );

			    auto numChildren = _numChildren[nodeId];
			    auto firstChildId = _firstChildId[nodeId];
			    auto lastChildId = firstChildId + numChildren;

			    for (auto childId = firstChildId; childId < lastChildId; ++childId)
				    _dirty[childId] = true;
            }

	       	_dirty[nodeId] = false;
		}

        ++nodeId;
	}
}

void
Transform::RootTransform::forceUpdate(scene::Node::Ptr node, bool updateTransformLists)
{
	if (_invalidLists || updateTransformLists)
		updateTransformsList();

	updateTransforms();
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
