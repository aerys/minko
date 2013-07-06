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

#include "PhysicsWorld.hpp"
#include <btBulletDynamicsCommon.h>
#include <minko/scene/Node.hpp>
#include <minko/scene/NodeSet.hpp>
#include <minko/controller/RenderingController.hpp>
#include <minko/controller/bullet/Collider.hpp>

using namespace minko;
using namespace minko::scene;
using namespace minko::controller;

bullet::PhysicsWorld::PhysicsWorld():
	AbstractController(),
	_colliderMap(),
	_btBroadphase(nullptr),
	_btCollisionConfiguration(nullptr),
	_btConstraintSolver(nullptr),
	_btDispatcher(nullptr),
	_btDynamicsWorld(nullptr),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_exitFrameSlot(nullptr)
{

}

void 
	bullet::PhysicsWorld::initialize()
{
	// straightforward physics world initialization for the time being
	_btBroadphase				= std::shared_ptr<btDbvtBroadphase>(new btDbvtBroadphase());
	_btCollisionConfiguration	= std::shared_ptr<btDefaultCollisionConfiguration>(new btDefaultCollisionConfiguration());
	_btConstraintSolver			= std::shared_ptr<btSequentialImpulseConstraintSolver>(new btSequentialImpulseConstraintSolver());
	_btDispatcher				= std::shared_ptr<btCollisionDispatcher>(new btCollisionDispatcher(_btCollisionConfiguration.get()));

	_btDynamicsWorld			= std::shared_ptr<btDiscreteDynamicsWorld>(new btDiscreteDynamicsWorld(
		_btDispatcher.get(), 
		_btBroadphase.get(),
		_btConstraintSolver.get(),
		_btCollisionConfiguration.get()
		));

	_targetAddedSlot	= targetAdded()->connect(std::bind(
		&bullet::PhysicsWorld::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));

	_targetRemovedSlot	= targetRemoved()->connect(std::bind(
		&bullet::PhysicsWorld::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
		));
}

void 
	bullet::PhysicsWorld::targetAddedHandler(std::shared_ptr<AbstractController> controller, std::shared_ptr<scene::Node> target)
{
	if (target != target->root())
		throw std::logic_error("A PhysicsWorld instance can only affect a root scene node.");

	auto renderingController	= getRootRenderingController(target->root());
	_exitFrameSlot				= renderingController->exitFrame()->connect(std::bind(
		&bullet::PhysicsWorld::exitFrameHandler,
		shared_from_this(),
		std::placeholders::_1
		));
}

void 
	bullet::PhysicsWorld::targetRemovedHandler(std::shared_ptr<AbstractController> controller, std::shared_ptr<scene::Node> target)
{
	std::cout << "bullet::PhysicsWorld::targetRemovedHandler" << std::endl;
}

void
	bullet::PhysicsWorld::addChild(ColliderPtr collider)
{
	if (hasCollider(collider))
		throw std::logic_error("The same collider cannot be added twice.");

	BulletColliderPtr btCollider = BulletCollider::create(collider);
	_colliderMap.insert(std::pair<ColliderPtr, BulletColliderPtr>(collider, btCollider));

	_btDynamicsWorld->addCollisionObject(btCollider->collisionObject.get());
}

void
	bullet::PhysicsWorld::removeChild(ColliderPtr collider)
{
	ColliderMap::const_iterator	it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		throw std::invalid_argument("collider");

	_btDynamicsWorld->removeCollisionObject(it->second->collisionObject.get());

	_colliderMap.erase(it);
}

bool
	bullet::PhysicsWorld::hasCollider(ColliderPtr collider) const
{
	return _colliderMap.find(collider) != _colliderMap.end();
}

void
	bullet::PhysicsWorld::exitFrameHandler(std::shared_ptr<RenderingController> controller)
{
	update();
}

void
	bullet::PhysicsWorld::update(float timeStep)
{
	//std::cout << "update physics" << std::endl;
	_btDynamicsWorld->stepSimulation(timeStep);
}

/*static*/
std::shared_ptr<RenderingController>
	bullet::PhysicsWorld::getRootRenderingController(NodePtr target)
{
	if (target == nullptr)
		throw std::invalid_argument("target");

	// get and count all RenderingController controllers in the descendants of the target's root
	auto nodeSet	= NodeSet::create(target->root())
		->descendants(true)
		->where([](NodePtr node)
	{
		return node->hasController<RenderingController>();
	});

	uint numControllers	= 0;
	for (auto node: nodeSet->nodes())
		numControllers	+= node->controllers<RenderingController>().size();

#ifdef DEBUG
	std::cout << numControllers << " rendering controllers" << std::endl;
#endif // DEBUG

	if (numControllers != 1)
		throw std::logic_error("PhysicsWorld requires exactly one RenderingController among the descendants of its target node.");

	return nodeSet->nodes()[0]->controllers<RenderingController>()[0];
}

/*static*/
bullet::PhysicsWorld::BulletColliderPtr
	bullet::PhysicsWorld::BulletCollider::create(bullet::PhysicsWorld::ColliderPtr collider)
{
	BulletColliderPtr	btCollider(new BulletCollider());

	// bad initialization right now
	auto shape	= collider->shape();

	btCollider->collisionShape	= std::shared_ptr<btCollisionShape>(new btSphereShape(1.0f));
	btCollider->motionState		= std::shared_ptr<btMotionState>(new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0))));

	btScalar	mass	= 1.0f;
	btVector3	inertia	 (0.0, 0.0, 0.0);
	btCollider->collisionShape->calculateLocalInertia(mass, inertia);

	btCollider->collisionObject	= std::shared_ptr<btCollisionObject>(new btRigidBody(
		mass,
		btCollider->motionState.get(),
		btCollider->collisionShape.get(),
		inertia
		));

	return btCollider;
}