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
#include "minko/data/Store.hpp"
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

AbstractComponent::Ptr
Transform::clone(const CloneOption& option)
{
	return Transform::create(this->matrix());
}

void
Transform::targetAdded(scene::Node::Ptr	target)
{
	if (target->component<Transform>(1) != nullptr)
		throw std::logic_error("A node cannot have more than one Transform.");

	target->data().addProvider(_data);

	_addedSlot = target->added().connect([=](scene::Node::Ptr n, scene::Node::Ptr t, scene::Node::Ptr p)
    {
        addedOrRemovedHandler(n, t, p);
    });
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

Transform::RootTransform::NodeTransformCacheEntry::NodeTransformCacheEntry() :
    _node(nullptr),
    _matrix(nullptr),
    _modelToWorldMatrix(nullptr),
    _parentId(-1),
    _firstChildId(-1),
    _numChildren(0),
    _dirty(true),
    _provider(nullptr)
{
}

AbstractComponent::Ptr
Transform::RootTransform::clone(const CloneOption& option)
{
	Transform::RootTransform::Ptr origin = std::static_pointer_cast<Transform::RootTransform>(shared_from_this());
	return Transform::RootTransform::create();
}

void
Transform::RootTransform::targetAdded(scene::Node::Ptr target)
{
    _targetSlots.push_back(target->added().connect(
        [this](scene::Node::Ptr n, scene::Node::Ptr t, scene::Node::Ptr p)
        {
            addedHandler(n, t, p);
        }
	));
    _targetSlots.push_back(target->removed().connect(
        [this](scene::Node::Ptr n, scene::Node::Ptr t, scene::Node::Ptr p)
        {
            removedHandler(n, t, p);
        }
    ));
    _targetSlots.push_back(target->componentAdded().connect(
        [this](scene::Node::Ptr n, scene::Node::Ptr t, AbsCtrlPtr c)
        {
            componentAddedHandler(n, t, c);
        }
    ));
    _targetSlots.push_back(target->componentRemoved().connect(
        [this](scene::Node::Ptr n, scene::Node::Ptr t, AbsCtrlPtr c)
        {
            componentRemovedHandler(n, t, c);
        }
	));

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
    if (_toAdd.empty() && _toRemove.empty())
        return;

    for (const auto& toRemove : _toRemove)
        _nodeToId.erase(toRemove);

    _nodes.clear();

    for (const auto& nodeAndId : _nodeToId)
        _nodes.push_back(nodeAndId.first);

    for (const auto& node : _toAdd)
        _nodes.push_back(node);

    _toAdd.clear();
    _toRemove.clear();

    _nodeTransformCache.clear();
    _nodeTransformCache.resize(_nodes.size(), NodeTransformCacheEntry());

    sortNodes();

    auto nodeId = 0;
    auto ancestor = scene::Node::Ptr();
    auto ancestorId = -1;
    auto firstSiblingId = -1;
    auto numSiblings = 0;

    for (const auto& node : _nodes)
	{
        auto previousAncestor = ancestor;

        ancestor = node->parent();

        while (ancestor != nullptr && !ancestor->hasComponent<Transform>())
            ancestor = ancestor->parent();

        if (ancestor == nullptr)
        {
            numSiblings = 0;
        }
        else if (ancestor != previousAncestor)
        {
            if (previousAncestor == nullptr)
            {
                ancestorId = _nodeToId.at(ancestor);

                firstSiblingId = nodeId;
                ++numSiblings;
            }
            else
            {
                const auto previousAncestorId = _nodeToId.at(previousAncestor);

                auto& previousAncestorCacheEntry = _nodeTransformCache.at(previousAncestorId);

                previousAncestorCacheEntry._firstChildId = firstSiblingId;
                previousAncestorCacheEntry._numChildren = numSiblings;

                firstSiblingId = nodeId;
                numSiblings = 1;

                previousAncestor = ancestor;

                ancestorId = _nodeToId.at(ancestor);
            }
        }
        else
        {
            ++numSiblings;
        }

		_nodeToId[node] = nodeId;

        auto& nodeCacheEntry = _nodeTransformCache.at(nodeId);

        nodeCacheEntry._node = node;

        nodeCacheEntry._parentId = ancestorId;

        nodeCacheEntry._matrix = node->data().getPointer<math::mat4>("matrix");
		nodeCacheEntry._modelToWorldMatrix = node->data().getUnsafePointer<math::mat4>("modelToWorldMatrix");
        nodeCacheEntry._provider = node->component<Transform>()->_data;

        ++nodeId;
	}

    if (ancestor != nullptr)
    {
        ancestorId = _nodeToId.at(ancestor);

        auto& ancestorCacheEntry = _nodeTransformCache.at(ancestorId);

        ancestorCacheEntry._firstChildId = firstSiblingId;
        ancestorCacheEntry._numChildren = numSiblings;
    }

	_invalidLists = false;
}

void
Transform::RootTransform::sortNodes()
{
    auto nodeDepths = std::map<int, std::list<scene::Node::Ptr>>();

    for (auto node : _nodes)
    {
        auto depth = 0;

        auto parent = node;

        while (parent->parent() != nullptr)
        {
            ++depth;

            parent = parent->parent();
        }

        nodeDepths[depth].push_back(node);
    }

    _nodes.clear();

    auto parentIndices = std::unordered_map<scene::Node::Ptr, int>();

    for (const auto& nodeDepth : nodeDepths)
    {
        const auto depth = nodeDepth.first;
        const auto& nodes = nodeDepth.second;

        auto localNodes = parentIndices.empty()
            ? std::vector<std::list<scene::Node::Ptr>> { nodes }
            : std::vector<std::list<scene::Node::Ptr>>(parentIndices.size());

        if (!parentIndices.empty())
        {
            for (auto node : nodes)
            {
                auto parentIndex = 0;
                auto parentIndexIt = parentIndices.find(node->parent());

                if (parentIndexIt != parentIndices.end())
                {
                    parentIndex = parentIndexIt->second;
                }
                else
                {
                    for (auto parentSibling : node->parent()->parent()->children())
                    {
                        if (parentSibling == node->parent())
                            break;

                        ++parentIndex;
                    }

                    if (localNodes.size() <= parentIndex)
                        localNodes.resize(parentIndex + 1);
                }

                localNodes[parentIndex].push_back(node);
            }

            parentIndices.clear();
        }

        auto parentIndexOffset = 0;

        for (auto localNodesEntry : localNodes)
        {
            if (localNodesEntry.empty())
                continue;

            auto parentNode = localNodesEntry.front()->parent();
            const auto& parentNodeChildren = parentNode->children();

            auto localSortedNodes = std::vector<scene::Node::Ptr>(parentNodeChildren.size());

            for (auto node : localNodesEntry)
            {
                auto nodeLocalIndex = 0;

                for (auto child : parentNodeChildren)
                {
                    if (child == node)
                        break;

                    ++nodeLocalIndex;
                }

                localSortedNodes[nodeLocalIndex] = node;
                parentIndices[node] = nodeLocalIndex + parentIndexOffset;
            }

            parentIndexOffset += localNodesEntry.size();

            for (auto node : localSortedNodes)
            {
                if (node == nullptr)
                    continue;

                _nodes.push_back(node);
            }
        }
    }
}

void
Transform::RootTransform::updateTransforms()
{
    math::mat4 modelToWorldMatrix;
    uint nodeId = 0;
	auto propertyName = data::Store::PropertyName(std::string("modelToWorldMatrix"));

    for (const auto& node : _nodes)
	{
        auto& nodeCacheEntry = _nodeTransformCache.at(nodeId);

		if (nodeCacheEntry._dirty)
		{
			auto parentId = nodeCacheEntry._parentId;

			if (parentId < 0)
				modelToWorldMatrix = *nodeCacheEntry._matrix;
            else
            {
                const auto& parentCacheEntry = _nodeTransformCache.at(parentId);

                modelToWorldMatrix = *parentCacheEntry._modelToWorldMatrix * *nodeCacheEntry._matrix;
            }

            // Because we use an unsafe pointer that gives us a direct access to the
            // data provider internal value for "modelToWorldMatrix", we have to trigger
            // the "property changed" signal manually.
            // This technique completely bypasses the storeproperty name resolving
            // mechanism and is a lot faster.
            if (*nodeCacheEntry._modelToWorldMatrix != modelToWorldMatrix)
            {
                auto& nodeData = node->data();
                auto provider = nodeCacheEntry._provider;

                // manually update the data provider internal mat4 object
                *nodeCacheEntry._modelToWorldMatrix = modelToWorldMatrix;

                // execute the "property changed" signal(s) manually
                nodeData.propertyChanged().execute(nodeData, provider, propertyName);
                if (nodeData.hasPropertyChangedSignal("modelToWorldMatrix"))
                    nodeData.propertyChanged("modelToWorldMatrix").execute(nodeData, provider, propertyName);

			    auto numChildren = nodeCacheEntry._numChildren;

                if (numChildren > 0)
                {
                    auto firstChildId = nodeCacheEntry._firstChildId;
                    auto lastChildId = firstChildId + numChildren;

                    for (auto childId = firstChildId; childId < lastChildId; ++childId)
                    {
                        auto& childCacheEntry = _nodeTransformCache.at(childId);

                        childCacheEntry._dirty = true;
                    }
                }
            }

	       	nodeCacheEntry._dirty = false;
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
