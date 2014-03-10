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

#include "minko/component/bullet/Collider.hpp"

#include <minko/math/Matrix4x4.hpp>
#include <minko/scene/Node.hpp>
#include <minko/scene/NodeSet.hpp>
#include <minko/component/Transform.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>
#include <minko/component/bullet/ColliderData.hpp>
#include <minko/component/bullet/PhysicsWorld.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::scene;
using namespace minko::component;

/*static*/
Matrix4x4::Ptr bullet::Collider::_TMP_MATRIX = Matrix4x4::create();

bullet::Collider::Collider(ColliderData::Ptr data):
	AbstractComponent(),
	_colliderData(data),
	_physicsWorld(nullptr),
	_targetTransform(nullptr),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_graphicsTransformChangedSlot(nullptr),
	_collisionStartedHandlerSlot(nullptr),
	_collisionEndedHandlerSlot(nullptr)
{
	if (data == nullptr)
		throw std::invalid_argument("data");
}

void
bullet::Collider::initialize()
{
	_targetAddedSlot	= targetAdded()->connect(std::bind(
		&bullet::Collider::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_targetRemovedSlot	= targetRemoved()->connect(std::bind(
		&bullet::Collider::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_graphicsTransformChangedSlot	= _colliderData->graphicsWorldTransformChanged()->connect(std::bind(
		&bullet::Collider::graphicsWorldTransformChangedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));
}

void
bullet::Collider::targetAddedHandler(
	AbstractComponent::Ptr controller, 
	Node::Ptr target)
{
	if (targets().size() > 1)
		throw std::logic_error("Collider cannot have more than one target.");

	_addedSlot		= targets().front()->added()->connect(std::bind(
		&bullet::Collider::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));

	_removedSlot	= targets().front()->removed()->connect(std::bind(
		&bullet::Collider::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
		));

	// initialize from node if possible (mostly for adding a controller to the camera)
	//initializeFromNode(target);
}

void
bullet::Collider::targetRemovedHandler(
	AbstractComponent::Ptr controller, 
	Node::Ptr target)
{
	_addedSlot		= nullptr;
	_removedSlot	= nullptr;
}

void 
bullet::Collider::addedHandler(
	Node::Ptr node, 
	Node::Ptr target, 
	Node::Ptr parent)
{
	initializeFromNode(node);

	_collisionStartedHandlerSlot = _colliderData->collisionStarted()->connect(std::bind(
		&bullet::Collider::collisionStartedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_collisionEndedHandlerSlot	= _colliderData->collisionEnded()->connect(std::bind(
		&bullet::Collider::collisionEndedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));
}

void
bullet::Collider::removedHandler(
	Node::Ptr node, 
	Node::Ptr target, 
	Node::Ptr parent)
{
	if (_physicsWorld != nullptr)
		_physicsWorld->removeChild(_colliderData);

	_physicsWorld					= nullptr;
	_targetTransform				= nullptr;
	_collisionStartedHandlerSlot	= nullptr;
	_collisionEndedHandlerSlot		= nullptr;
}

void
bullet::Collider::collisionStartedHandler(ColliderData::Ptr obj0, ColliderData::Ptr obj1)
{
#ifdef DEBUG_PHYSICS_COLLISIONS
	std::cout << "[" << obj0->name() << "]\t>-< [" << obj1->name() << "]" << std::endl;
#endif // DEBUG_PHYSICS_COLLISIONS
}

void
bullet::Collider::collisionEndedHandler(ColliderData::Ptr obj0, ColliderData::Ptr obj1)
{
#ifdef DEBUG_PHYSICS_COLLISIONS
	std::cout << "[" << obj0->name() << "]\t< > [" << obj1->name() << "]" << std::endl;
#endif // DEBUG_PHYSICS_COLLISIONS
}

void
bullet::Collider::initializeFromNode(Node::Ptr node)
{
	if (_targetTransform != nullptr && _physicsWorld != nullptr)
		return;

	// identify physics world
	auto withPhysicsWorld = NodeSet::create(node)
		->ancestors(true)
		->where([](Node::Ptr n){ return n->hasComponent<bullet::PhysicsWorld>(); });

	if (withPhysicsWorld->nodes().size() != 1)
	{
#ifdef DEBUG_PHYSICS
		std::cout << "[" << node->name() << "]\tcollider CANNOT be added (# PhysicsWorld = " << nodeSet->nodes().size() << ")." << std::endl;
#endif // DEBUG_PHYSICS

		return;
	}

	_physicsWorld = withPhysicsWorld->nodes().front()->component<bullet::PhysicsWorld>();
	assert(_physicsWorld);

	if (!node->hasComponent<Transform>())
		node->addComponent(Transform::create());
	
	_targetTransform = node->component<Transform>();
	assert(_targetTransform);

	// check that target's model-to-world matrix is good to begin with.
	auto modelToWorld = _targetTransform->modelToWorldMatrix(true);
	if (fabsf(modelToWorld->determinant()) < 1e-3f)
		throw new std::logic_error("The node's model-to-world matrix cannot be inverted.");

    _colliderData->_node = node; 
	_physicsWorld->addChild(_colliderData);

	synchronizePhysicsWithGraphics();
}

void
bullet::Collider::synchronizePhysicsWithGraphics()
{
	if (_physicsWorld == nullptr || _targetTransform == nullptr)
		return;

	auto graphicsTransform = _targetTransform->modelToWorldMatrix(true);

	// remove the scaling/shear from the graphics transform, but record it to restitute it during rendering
	PhysicsWorld::removeScalingShear(
		graphicsTransform, 
		_TMP_MATRIX, 
		_colliderData->correction()
	);
	// _TMP_MATRIX = graphicsNoScaleTransform

#ifdef DEBUG_PHYSICS
	std::cout << "[" << _colliderData->name() << "]\tsynchro graphics->physics" << std::endl;
	PhysicsWorld::print(std::cout << "- correction =\n", _colliderData->correction()) << std::endl;
	PhysicsWorld::print(std::cout << "- scalefree(graphics) =\n", _TMP_MATRIX) << std::endl;
#endif // DEBUG_PHYSICS

	_physicsWorld->synchronizePhysicsWithGraphics(_colliderData, _TMP_MATRIX);
}

void
bullet::Collider::graphicsWorldTransformChangedHandler(ColliderData::Ptr collider, 
													   Matrix4x4::Ptr graphicsTransform)
{
	if (_targetTransform == nullptr)
		return;

	// get the world-to-parent matrix in order to update the target's Transform
	_TMP_MATRIX
		->copyFrom(_targetTransform->modelToWorldMatrix(true))
		->invert()
		->append(_targetTransform->matrix());
	// _TMP_MATRIX = worldToParent

	_targetTransform->matrix()
		->copyFrom(graphicsTransform)
		->append(_TMP_MATRIX);
}