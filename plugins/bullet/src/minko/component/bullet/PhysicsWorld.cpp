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
#include <minko/math/Matrix4x4.hpp>
#include <minko/scene/Node.hpp>
#include <minko/scene/NodeSet.hpp>
#include <minko/component/Rendering.hpp>
#include <minko/component/bullet/Collider.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::scene;
using namespace minko::component;

bullet::PhysicsWorld::PhysicsWorld():
	AbstractComponent(),
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
	/*
	for (float ang = -90.0f; ang < 90.0f; ang += 10.0f)
	{
		for (uint iter = 0; iter < 20; ++iter)
		{
			Vector3::Ptr axis = Vector3::create((float)rand(), (float)rand(), (float)rand())->normalize();
			float angRad = ang * (float)PI / 180.0f;
			Quaternion::Ptr quat = Quaternion::create()->initialize(axis, angRad);
			Matrix4x4::Ptr	mat	= quat->toMatrix();
			btTransform btTransf;
			toBulletTransform(mat, btTransf);
			Matrix4x4::Ptr fromBt = fromBulletTransform(btTransf);

			Vector3::Ptr vec	= Vector3::create((float)rand(), (float)rand(), (float)rand())->normalize();
			btVector3 btVec(vec->x(), vec->y(), vec->z());

			vec = mat->project(vec);
			btVec = btTransf(btVec);
			if (fabsf(vec->x() - btVec.x()) > 1e-6f
				|| fabsf(vec->y() - btVec.y()) > 1e-6f
				|| fabsf(vec->z() - btVec.z()) > 1e-6f)
				throw std::logic_error("aie X2");

			for (uint i = 0; i < 16; ++i)
			{
				if (fabsf(mat->values()[i] - fromBt->values()[i]) > 1e-6f)
					throw std::logic_error("aie");
			}
			
		}
	}
	*/

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
	bullet::PhysicsWorld::targetAddedHandler(AbstractComponent::Ptr controller, 
	Node::Ptr target)
{
	if (target->components<PhysicsWorld>().size() > 1)
		throw std::logic_error("There cannot be two PhysicsWorld on the same node.");

	auto nodeSet		= NodeSet::create(target->root())
		->descendants(true)
		->where([](Node::Ptr node)
		{
			return node->hasComponent<Rendering>();
	});
	if (nodeSet->nodes().size() != 1)
	{
		std::stringstream stream;
		stream << "PhysicsWorld requires exactly one Rendering among the descendants of its target node. Found " << nodeSet->nodes().size();
		throw std::logic_error(stream.str());
	}

	auto renderingCtrl	= nodeSet->nodes().front()->component<Rendering>();

	_exitFrameSlot		= renderingCtrl->exitFrame()->connect(std::bind(
		&bullet::PhysicsWorld::exitFrameHandler,
		shared_from_this(),
		std::placeholders::_1
		));
}

void 
	bullet::PhysicsWorld::targetRemovedHandler(AbstractComponent::Ptr controller, 
	Node::Ptr target)
{
	std::cout << "bullet::PhysicsWorld::targetRemovedHandler" << std::endl;
}

void
	bullet::PhysicsWorld::addChild(Collider::Ptr collider)
{
	if (hasCollider(collider))
		throw std::logic_error("The same collider cannot be added twice.");

	BulletCollider::Ptr btCollider = BulletCollider::create(collider);
	_colliderMap.insert(std::pair<Collider::Ptr, BulletCollider::Ptr>(collider, btCollider));

	std::dynamic_pointer_cast<btDiscreteDynamicsWorld>(_btDynamicsWorld)
		->addRigidBody(std::dynamic_pointer_cast<btRigidBody>(btCollider->collisionObject()).get());
	//_btDynamicsWorld->addCollisionObject(btCollider->collisionObject().get());
}

void
	bullet::PhysicsWorld::removeChild(Collider::Ptr collider)
{
	ColliderMap::const_iterator	it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		throw std::invalid_argument("collider");

	_btDynamicsWorld->removeCollisionObject(it->second->collisionObject().get());

	_colliderMap.erase(it);
}

bool
	bullet::PhysicsWorld::hasCollider(Collider::Ptr collider) const
{
	return _colliderMap.find(collider) != _colliderMap.end();
}


void
	bullet::PhysicsWorld::setGravity(Vector3::Ptr gravity)
{
	_btDynamicsWorld->setGravity(btVector3(gravity->x(), gravity->y(), gravity->z()));
}

void
	bullet::PhysicsWorld::exitFrameHandler(Rendering::Ptr controller)
{
	update();
}


void
	bullet::PhysicsWorld::update(float timeStep)
{
	_btDynamicsWorld->stepSimulation(timeStep);
	updateColliders();
}

void
	bullet::PhysicsWorld::updateColliders()
{
	for (ColliderMap::iterator it = _colliderMap.begin(); it != _colliderMap.end(); ++it)
	{
		Collider::Ptr		collider(it->first);
		BulletCollider::Ptr	btCollider(it->second);

		if (collider->isStatic())
			continue;

		const btTransform& colliderWorldTrf(btCollider->collisionObject()->getWorldTransform());	

		//collider->updateColliderWorldTransform(it->first->worldTransform());
		collider->updateColliderWorldTransform(fromBulletTransform(colliderWorldTrf));
	}
}

void
	bullet::PhysicsWorld::setWorldTransformFromCollider(Collider::Ptr collider)
{
	auto it	= _colliderMap.find(collider);
	if (it == _colliderMap.end())
		return;

	it->second->setWorldTransform(collider->worldTransform());
}

/*static*/
Matrix4x4::Ptr
	bullet::PhysicsWorld::fromBulletTransform(const btTransform& transform)
{
	auto basis			= transform.getBasis();
	auto translation	= transform.getOrigin();

	Matrix4x4::Ptr	output = Matrix4x4::create();
	output->initialize(
		basis[0][0], basis[0][1], basis[0][2], translation[0],
		basis[1][0], basis[1][1], basis[1][2], translation[1],
		basis[2][0], basis[2][1], basis[2][2], translation[2],
		0.0f, 0.0f, 0.0f, 1.0f
		);
	return output;
}

/*static*/
void
	bullet::PhysicsWorld::toBulletTransform(Matrix4x4::Ptr transform,
	btTransform& output)
{
	auto translation	= transform->translationVector();
	auto rotation		= transform->rotation();

	btVector3		btOrigin(translation->x(), translation->y(), translation->z());
	btQuaternion	btRotation(rotation->i(), rotation->j(), rotation->k(), rotation->r());

	output.setOrigin(btOrigin);
	output.setRotation(btRotation);
}