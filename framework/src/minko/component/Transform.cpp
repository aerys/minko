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

#include "minko/math/Matrix4x4.hpp"
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
	_matrix(Matrix4x4::create()),
	_modelToWorld(Matrix4x4::create()),
	_worldToModel(Matrix4x4::create()),
	_data(data::StructureProvider::create("transform"))
{
}

void
Transform::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&Transform::targetAddedHandler,
		std::static_pointer_cast<Transform>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&Transform::targetRemovedHandler,
		std::static_pointer_cast<Transform>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_data->set<Matrix4x4::Ptr>("matrix", _matrix);
	_data->set<Matrix4x4::Ptr>("modelToWorldMatrix", _modelToWorld);
	//_data->set("transform/worldToModelMatrix", _worldToModel);
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
		std::static_pointer_cast<Transform>(shared_from_this()),
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
		std::static_pointer_cast<RootTransform>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2
	)));

	_targetSlots.push_back(targetRemoved()->connect(std::bind(
		&Transform::RootTransform::targetRemovedHandler,
		std::static_pointer_cast<RootTransform>(shared_from_this()),
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
		std::static_pointer_cast<RootTransform>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->removed()->connect(std::bind(
		&Transform::RootTransform::removedHandler,
		std::static_pointer_cast<RootTransform>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->componentAdded()->connect(std::bind(
		&Transform::RootTransform::componentAddedHandler,
		std::static_pointer_cast<RootTransform>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	)));
	_targetSlots.push_back(target->componentRemoved()->connect(std::bind(
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
			std::static_pointer_cast<RootTransform>(shared_from_this()), 
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
	_transforms		.clear();
	_modelToWorld	.clear();
	_nodeToId		.clear();
	_idToNode		.clear();
	_parentId		.clear();
	_firstChildId	.clear();
	_numChildren	.clear();

	auto withTransforms			= scene::NodeSet::create(targets())
		->descendants(true, false)
		->where([](scene::Node::Ptr n){ return n->hasComponent<Transform>(); });

	auto nodesWithTransform	= withTransforms->nodes();

	juxtaposeSiblings(nodesWithTransform); // make sure siblings are at contiguous positions in the vector

	_transforms		.resize(nodesWithTransform.size());
	_modelToWorld	.resize(nodesWithTransform.size());
	_idToNode		.resize(nodesWithTransform.size());
	_parentId		.resize(nodesWithTransform.size(), -1);
	_firstChildId	.resize(nodesWithTransform.size(), 0);
	_numChildren	.resize(nodesWithTransform.size(), 0);

	for (uint nodeId = 0; nodeId < nodesWithTransform.size(); ++nodeId)
	{
		auto	node		= nodesWithTransform[nodeId];
		auto	transform	= node->component<Transform>();
		auto	ancestor	= node->parent();

		while (ancestor != nullptr && _nodeToId.count(ancestor) == 0)
			ancestor = ancestor->parent();

		_nodeToId[node]			= nodeId;

		_transforms[nodeId]		= transform->_matrix;
		_modelToWorld[nodeId]	= transform->_modelToWorld;
		_idToNode[nodeId]		= node;
		
		if (ancestor)
		{
			assert(_nodeToId.count(ancestor) > 0);
			auto ancestorId = _nodeToId[ancestor];

			_parentId[nodeId]	= ancestorId;

			if (_numChildren[ancestorId] == 0)
				_firstChildId[ancestorId] = nodeId;
			++_numChildren[ancestorId];
		}
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
		auto it			= nodes.begin() + nodeId;
		auto node		= *it;
		auto ancestor	= node->parent();
		while (ancestor != nullptr 
			&& std::find(nodes.begin(), it, ancestor) == nodes.end())
			ancestor = ancestor->parent();

		if (firstChild.count(ancestor) == 0)
			firstChild[ancestor] = nodeId;
		else
		{
			assert(firstChild[ancestor] <= nodeId);

			nodes.erase(it);
			nodes.insert(nodes.begin() + firstChild[ancestor], node);
		}
	}
}

void
Transform::RootTransform::updateTransforms()
{
	unsigned int numNodes 	= _transforms.size();
	unsigned int nodeId 	= 0;

	while (nodeId < numNodes)
	{
		auto parentModelToWorldMatrix 	= _modelToWorld[nodeId];
		auto numChildren 				= _numChildren[nodeId];
		auto firstChildId 				= _firstChildId[nodeId];
		auto lastChildId 				= firstChildId + numChildren;
		auto parentId 					= _parentId[nodeId];
        auto parentTransformChanged     = parentModelToWorldMatrix->_hasChanged;

        parentModelToWorldMatrix->_hasChanged = false;

		if (parentId == -1)
		{
            auto parentTransform = _transforms[nodeId];
	        
            if (_transforms[nodeId]->_hasChanged)
            {
			    parentModelToWorldMatrix->copyFrom(parentTransform);
				parentModelToWorldMatrix->_hasChanged = false;
                parentTransform->_hasChanged = false;
				parentTransformChanged = true;
            }
		}

		for (auto childId = firstChildId; childId < lastChildId; ++childId)
		{
			auto modelToWorld   = _modelToWorld[childId];
            auto transform      = _transforms[childId];

            if (transform->_hasChanged || parentTransformChanged)
            {
			    modelToWorld->lock()->copyFrom(transform)->append(parentModelToWorldMatrix)->unlock();
			    transform->_hasChanged = false;
            }
		}

		++nodeId;
	}
}

void
Transform::RootTransform::forceUpdate(scene::Node::Ptr node, bool updateTransformLists)
{
	if (_invalidLists || updateTransformLists)
		updateTransformsList();

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
