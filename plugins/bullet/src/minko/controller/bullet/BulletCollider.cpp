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
#include <minko/math/Vector3.hpp>
#include <minko/math/Quaternion.hpp>
#include <minko/math/Matrix4x4.hpp>
#include <minko/controller/bullet/Collider.hpp>
#include <minko/controller/bullet/AbstractPhysicsShape.hpp>
#include <minko/controller/bullet/SphereShape.hpp>
#include <minko/controller/bullet/BoxShape.hpp>
#include <minko/controller/bullet/ConeShape.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::controller;

bullet::PhysicsWorld::BulletCollider::BulletCollider():
	_btCollisionShape(nullptr),
	_btMotionState(nullptr),
	_btCollisionObject(nullptr)
{
}

void
	bullet::PhysicsWorld::BulletCollider::initialize(Collider::Ptr collider)
{
	if (collider == nullptr)
		throw std::invalid_argument("collider");

	initializeCollisionShape(collider->shape());
	initializeMotionState(collider);
	initializeCollisionObject(collider);
}

void
	bullet::PhysicsWorld::BulletCollider::initializeCollisionShape(AbstractPhysicsShape::Ptr shape)
{
	if (shape == nullptr)
		throw std::invalid_argument("shape");

	switch(shape->type())
	{
	case AbstractPhysicsShape::SPHERE:
		initializeSphereShape(std::dynamic_pointer_cast<SphereShape>(shape));
		break;

	case AbstractPhysicsShape::BOX:
		initializeBoxShape(std::dynamic_pointer_cast<BoxShape>(shape));
		break;

	case AbstractPhysicsShape::CONE:
		initializeConeShape(std::dynamic_pointer_cast<ConeShape>(shape));
		break;

	default:
		throw std::logic_error("Unsupported physics shape");
	}

	_btCollisionShape->setMargin(shape->margin());
}

void 
	bullet::PhysicsWorld::BulletCollider::initializeSphereShape(SphereShape::Ptr sphere)
{
	_btCollisionShape	= std::shared_ptr<btCollisionShape>(new btSphereShape(sphere->radius()));
}

void 
	bullet::PhysicsWorld::BulletCollider::initializeBoxShape(BoxShape::Ptr box)
{
	btVector3 halfExtents (box->halfExtentX(), box->halfExtentY(), box->halfExtentZ());
	_btCollisionShape	= std::shared_ptr<btCollisionShape>(new btBoxShape(halfExtents));
}

void 
	bullet::PhysicsWorld::BulletCollider::initializeConeShape(ConeShape::Ptr cone)
{
	_btCollisionShape	= std::shared_ptr<btCollisionShape>(new btConeShape(cone->radius(), cone->height()));
}

void
	bullet::PhysicsWorld::BulletCollider::initializeMotionState(Collider::Ptr collider)
{
	btTransform btStartTransform;
	toBulletTransform(collider->worldTransform(), btStartTransform);

	btTransform btOffsetTransform (btQuaternion(0.0f, 0.0f, 0.0, 1.0f), btVector3(0.0f, 0.0f, 0.0f));
	auto centerOfMassOffset	= collider->centerOfMassOffset();
	if (centerOfMassOffset == nullptr)
	{
		//auto offsetRotation		= centerOfMassOffset->rotation();
		//auto offsetTranslation	= centerOfMassOffset->translation();
		//btOffsetTransform.setBasis(btQuaternion(offsetRotation->x(), offsetRotation->y(), offsetRotation->z(), offsetRotation->w()).);

		//btTransform btOffsetTransform(
		//	btQuaternion(offsetRotation->x(), offsetRotation->y(), offsetRotation->z(), offsetRotation->w()),
		//	btVector3(offsetTranslation->x(), offsetTranslation->y(), offsetTranslation->z())
		//	);
	}

	_btMotionState	= std::shared_ptr<btMotionState>(new btDefaultMotionState(
		btStartTransform,
		btOffsetTransform
		));
}

void
	bullet::PhysicsWorld::BulletCollider::initializeCollisionObject(Collider::Ptr collider)
{
	// only rigid objects are considerered for the moment

	btVector3	inertia	 (0.0, 0.0, 0.0);
	if (collider->inertia() == nullptr)
	{
		if (collider->mass() > 0.0f)
			_btCollisionShape->calculateLocalInertia(collider->mass(), inertia);
	}
	else
	{
		inertia.setX(collider->inertia()->x());
		inertia.setY(collider->inertia()->y());
		inertia.setZ(collider->inertia()->z());
	}

	// only rigid objects are considerered for the moment
	auto btRigidCollisionObject	= std::shared_ptr<btRigidBody>(new btRigidBody(
		collider->mass(),
		_btMotionState.get(),
		_btCollisionShape.get(),
		inertia
		));

	// communicate several properties of the rigid object
	btRigidCollisionObject->setLinearVelocity(btVector3(
		collider->linearVelocity()->x(), 
		collider->linearVelocity()->y(), 
		collider->linearVelocity()->z()
		));
	btRigidCollisionObject->setLinearFactor(btVector3(
		collider->linearFactor()->x(), 
		collider->linearFactor()->y(), 
		collider->linearFactor()->z()
		));
	btRigidCollisionObject->setAngularVelocity(btVector3(
		collider->angularVelocity()->x(), 
		collider->angularVelocity()->y(), 
		collider->angularVelocity()->z()
		));
	btRigidCollisionObject->setAngularFactor(btVector3(
		collider->angularFactor()->x(), 
		collider->angularFactor()->y(), 
		collider->angularFactor()->z()
		));
	btRigidCollisionObject->setDamping(collider->linearDamping(), collider->angularDamping());
	btRigidCollisionObject->setRestitution(collider->restitution());

	_btCollisionObject	= btRigidCollisionObject;
}

void bullet::PhysicsWorld::BulletCollider::setWorldTransform(Matrix4x4::Ptr worldTransform)
{
	btTransform btWorldTransform;
	toBulletTransform(worldTransform, btWorldTransform);
	_btMotionState->setWorldTransform(btWorldTransform);
}

bullet::PhysicsWorld::BulletCollider::Ptr
	bullet::PhysicsWorld::BulletCollider::create(Collider::Ptr collider)
{
	BulletColliderPtr	btCollider(new BulletCollider());

	btCollider->initialize(collider);

	return btCollider;
}
