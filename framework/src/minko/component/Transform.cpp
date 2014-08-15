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
#include "minko/component/SceneManager.hpp"

using namespace minko;
using namespace minko::component;

Transform::Transform() :
	minko::component::AbstractComponent(),
	_matrix(1.),
	_modelToWorld(1.),
//	_worldToModel(1.),
	_data(data::Provider::create())
{
	_data
		->set<math::mat4>("matrix", 				_matrix)
		->set<math::mat4>("modelToWorldMatrix", 	_modelToWorld);
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

    /*auto withTransforms = scene::NodeSet::create(this->target())
        ->descendants(true, false)
        ->where([](scene::Node::Ptr n){ return n->hasComponent<Transform>(); });
    _toAdd.insert(_toAdd.begin(), withTransforms->nodes().begin(), withTransforms->nodes().end());*/
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
    _nodes.reserve(_nodeToId.size() + _toAdd.size());
    for (const auto& nodeAndId : _nodeToId)
        _nodes.push_back(nodeAndId.first);
    for (const auto& node : _toAdd)
        _nodes.push_back(node);
    _toAdd.clear();
    _toRemove.clear();

    juxtaposeSiblings(_nodes); // make sure siblings are at contiguous positions in the vector

    //_nodeToId.clear();
    _matrix.clear();
    _modelToWorld.clear();
    _parentId.clear();
    _firstChildId.clear();
    _numChildren.clear();
    _dirty.clear();
    _matrix.resize(_nodes.size());
    _modelToWorld.resize(_nodes.size());
    _parentId.resize(_nodes.size(), -1);
    _firstChildId.resize(_nodes.size(), -1);
    _numChildren.resize(_nodes.size(), -1);
    _dirty.resize(_nodes.size(), true);

    auto nodeId = 0;
    for (const auto& node : _nodes)
	{
		auto transform	= node->component<Transform>();
		auto ancestor	= node->parent();

		while (ancestor != nullptr && _nodeToId.count(ancestor) == 0)
			ancestor = ancestor->parent();

		_nodeToId[node] = nodeId;
		_matrix[nodeId] = &transform->_matrix;
		_modelToWorld[nodeId] = node->data().getUnsafePointer<math::mat4>("modelToWorldMatrix");
        _dirty[nodeId] = true;
		
		if (ancestor)
		{
			assert(_nodeToId.count(ancestor) > 0);
			auto ancestorId = _nodeToId[ancestor];

			_parentId[nodeId]	= ancestorId;

			if (_numChildren[ancestorId] == 0)
				_firstChildId[ancestorId] = nodeId;
			++_numChildren[ancestorId];
		}

        ++nodeId;
	}

	_invalidLists = false;
}

/*static*/
void
Transform::RootTransform::juxtaposeSiblings(std::vector<NodePtr>& nodes)
{
	// assumes 'nodes' is the result of a breadth-first search from the nodes
	std::unordered_map<scene::Node::Ptr, unsigned int>	firstChild;

	for (unsigned int nodeId = 0; nodeId < nodes.size(); ++nodeId)
	{
		auto it = std::next(nodes.begin(), nodeId);
		auto node = *it;
        auto ancestor = node->parent();

        while (ancestor != nullptr && !ancestor->hasComponent<Transform>())
            ancestor = ancestor->parent();

        if (!ancestor)
            continue;

        if (firstChild.count(ancestor) == 0)
            firstChild[ancestor] = nodeId;
		else
		{
            assert(firstChild[ancestor] <= nodeId);

			nodes.erase(it);
            nodes.insert(std::next(nodes.begin(), firstChild[ancestor]), node);
		}
	}
}

void
Transform::RootTransform::updateTransforms()
{
    math::mat4 modelToWorldMatrix;

    for (unsigned int nodeId = 0; nodeId < _matrix.size(); ++nodeId)
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
            // This technique completely bypasses the container property name solving
            // mechanism and is a lot faster.
            if (*_modelToWorld[nodeId] != modelToWorldMatrix)
            {
                auto nodeData = _nodes[nodeId]->data();

                *_modelToWorld[nodeId] = modelToWorldMatrix;
                if (nodeData.hasPropertyChangedSignal("modelToWorldMatrix"))
                    nodeData.propertyChanged("modelToWorldMatrix")->execute(
                        nodeData,
                        "modelToWorldMatrix",
                        "modelToWorldMatrix"
                    );
            }

			auto numChildren = _numChildren[nodeId];
			auto firstChildId = _firstChildId[nodeId];
			auto lastChildId = firstChildId + numChildren;

			for (auto childId = firstChildId; childId < lastChildId; ++childId)
				_dirty[childId] = true;

	       	_dirty[nodeId] = false;
		}
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
