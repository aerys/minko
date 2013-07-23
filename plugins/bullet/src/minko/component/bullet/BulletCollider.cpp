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
#include <minko/component/bullet/Collider.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>
#include <minko/component/bullet/SphereShape.hpp>
#include <minko/component/bullet/BoxShape.hpp>
#include <minko/component/bullet/ConeShape.hpp>
#include <minko/component/bullet/CylinderShape.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::component;

bullet::PhysicsWorld::BulletCollider::BulletCollider():
	_bulletCollisionShape(nullptr),
	_bulletMotionState(nullptr),
	_bulletCollisionObject(nullptr)
{
}

void
bullet::PhysicsWorld::BulletCollider::initialize(Collider::Ptr collider)
{
	if (collider == nullptr)
		throw std::invalid_argument("collider");

	initializeCollisionShape(collider->shape());

#ifdef DEBUG_PHYSICS
	std::cout << "[" << collider->name() << "]\tinit collision shape\n\t- local scaling = " << _bulletCollisionShape->getLocalScaling()[0] 
	<< "\n\t- margin = " << _bulletCollisionShape->getMargin() << std::endl;
#endif // DEBUG_PHYSICS

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

	case AbstractPhysicsShape::CYLINDER:
		initializeCylinderShape(std::dynamic_pointer_cast<CylinderShape>(shape));
		break;

	default:
		throw std::logic_error("Unsupported physics shape");
	}

	_bulletCollisionShape->setLocalScaling(btVector3(
		shape->localScaling(), 
		shape->localScaling(), 
		shape->localScaling()
	));
	_bulletCollisionShape->setMargin(shape->margin());
}

void 
bullet::PhysicsWorld::BulletCollider::initializeSphereShape(SphereShape::Ptr sphere)
{
	_bulletCollisionShape	= std::shared_ptr<btSphereShape>(new btSphereShape(sphere->radius()));
}

void 
bullet::PhysicsWorld::BulletCollider::initializeBoxShape(BoxShape::Ptr box)
{
	btVector3 halfExtents (box->halfExtentX(), box->halfExtentY(), box->halfExtentZ());
	_bulletCollisionShape	= std::shared_ptr<btBoxShape>(new btBoxShape(halfExtents));
}

void 
bullet::PhysicsWorld::BulletCollider::initializeConeShape(ConeShape::Ptr cone)
{
	_bulletCollisionShape	= std::shared_ptr<btConeShape>(new btConeShape(cone->radius(), cone->height()));
}

void
bullet::PhysicsWorld::BulletCollider::initializeCylinderShape(CylinderShape::Ptr cylinder)
{
	btVector3 halfExtents (cylinder->halfExtentX(), cylinder->halfExtentY(), cylinder->halfExtentZ());
	_bulletCollisionShape	= std::shared_ptr<btCylinderShape>(new btCylinderShape(halfExtents));
}

void
bullet::PhysicsWorld::BulletCollider::initializeMotionState(Collider::Ptr collider)
{
	// collider's starting world transform
	Matrix4x4Ptr startWorld = Matrix4x4::create()
		->copyFrom(collider->worldTransform());
		//->appendTranslation(collider->shape()->centerOfMassTranslation()->x(),
		//					collider->shape()->centerOfMassTranslation()->y(),
		//					collider->shape()->centerOfMassTranslation()->z());

	btTransform bulletStartTransform;
	toBulletTransform(startWorld, bulletStartTransform);

	// collider's center-of-mass offset transform
	btTransform bulletOffsetTransform;

	Quaternion::Ptr offRotation = collider->shape()->centerOfMassInverseTransform()->rotationQuaternion();
	Vector3::Ptr	offTransl = collider->shape()->centerOfMassInverseTransform()->translationVector();

	/*
	Matrix4x4::Ptr	offTransf = Matrix4x4::create()
		->copyFrom(startWorld)->invert()
		->append(offRotation)
		->append(startWorld)
		->appendTranslation(offTransl)
		->invert();
		*/

	Matrix4x4::Ptr offTransf = Matrix4x4::create()
		->copyFrom(collider->shape()->centerOfMassOffset());

	toBulletTransform(
		offTransf->rotationQuaternion(),
		offTransf->translationVector(),
		bulletOffsetTransform
	);

	/*
	auto	offsetTranslation	= Vector3::create(0.f, 0.f, 0.f);
	auto	offsetRotation		= collider->shape()->centerOfMassRotation();

	btTransform bulletOffsetTransform;
	toBulletTransform(
		collider->shape()->centerOfMassRotation(),
		collider->shape()->centerOfMassTranslation(),
		bulletOffsetTransform
	);
	*/

#ifdef DEBUG_PHYSICS
	std::cout << "[" << collider->name() << "]\tinit motion state" << std::endl;
	print(std::cout << "- startTrans =\n", bulletStartTransform) << std::endl;
	print(std::cout << "- centerOfMassOffset =\n", bulletOffsetTransform) << std::endl;
#endif // DEBUG_PHYSICS


	_bulletMotionState	= std::shared_ptr<btMotionState>(new btDefaultMotionState(
		bulletStartTransform,
		bulletOffsetTransform
		));
}

void
bullet::PhysicsWorld::BulletCollider::initializeCollisionObject(Collider::Ptr collider)
{
	// only rigid objects are considerered for the moment

	btVector3 inertia (0.0, 0.0, 0.0);
	if (collider->inertia() == nullptr)
	{
		if (collider->mass() > 0.0f)
			_bulletCollisionShape->calculateLocalInertia(collider->mass(), inertia);
	}
	else
	{
		inertia.setX(collider->inertia()->x());
		inertia.setY(collider->inertia()->y());
		inertia.setZ(collider->inertia()->z());
	}

	// construction of a new rigid collision object
	auto info = btRigidBody::btRigidBodyConstructionInfo(
		collider->mass(),
		_bulletMotionState.get(),
		_bulletCollisionShape.get(),
		inertia
	);
	info.m_linearDamping			= collider->linearDamping();
	info.m_angularDamping			= collider->angularDamping();
	info.m_friction					= collider->friction();
	info.m_rollingFriction			= collider->rollingFriction();
	info.m_restitution				= collider->restitution();
	info.m_linearSleepingThreshold	= collider->linearSleepingThreshold();
	info.m_angularSleepingThreshold	= collider->angularSleepingThreshold();

	auto bulletRigidBody = std::shared_ptr<btRigidBody>(new btRigidBody(info));

	// communicate several properties of the rigid object
	bulletRigidBody->setLinearVelocity(btVector3(
		collider->linearVelocity()->x(), 
		collider->linearVelocity()->y(), 
		collider->linearVelocity()->z()
		));
	bulletRigidBody->setLinearFactor(btVector3(
		collider->linearFactor()->x(), 
		collider->linearFactor()->y(), 
		collider->linearFactor()->z()
		));
	bulletRigidBody->setAngularVelocity(btVector3(
		collider->angularVelocity()->x(), 
		collider->angularVelocity()->y(), 
		collider->angularVelocity()->z()
		));
	bulletRigidBody->setAngularFactor(btVector3(
		collider->angularFactor()->x(), 
		collider->angularFactor()->y(), 
		collider->angularFactor()->z()
		));

	bulletRigidBody->setActivationState(collider->deactivationDisabled() 
		? DISABLE_DEACTIVATION 
		: ACTIVE_TAG
	);

	_bulletCollisionObject	= bulletRigidBody;
}

void
bullet::PhysicsWorld::BulletCollider::setLinearVelocity(Vector3::Ptr velocity)
{
	std::shared_ptr<btRigidBody> bulletRigidBody = std::dynamic_pointer_cast<btRigidBody>(_bulletCollisionObject);
	bulletRigidBody->setLinearVelocity(btVector3(velocity->x(), velocity->y(), velocity->z()));
}

void 
bullet::PhysicsWorld::BulletCollider::setWorldTransform(Matrix4x4::Ptr worldTransform)
{
	btTransform bulletWorldTransform;
	toBulletTransform(worldTransform, bulletWorldTransform);
	_bulletMotionState->setWorldTransform(bulletWorldTransform);
}

void
bullet::PhysicsWorld::BulletCollider::applyRelativeImpulse(Vector3::Ptr relativeImpulse)
{
	std::shared_ptr<btRigidBody> bulletRigidBody = std::dynamic_pointer_cast<btRigidBody>(_bulletCollisionObject);

	btVector3 btRelImpulse(relativeImpulse->x(), relativeImpulse->y(), relativeImpulse->z());

	bulletRigidBody->applyImpulse(
		bulletRigidBody->getWorldTransform().getBasis() * btRelImpulse, 
		btVector3(0.0f, 0.0f, 0.0f)
	);
}

void
bullet::PhysicsWorld::BulletCollider::prependLocalTranslation(Vector3::Ptr relTranslation)
{
	std::shared_ptr<btRigidBody> bulletRigidBody = std::dynamic_pointer_cast<btRigidBody>(_bulletCollisionObject);

	btVector3 btRelTranslation(relTranslation->x(), relTranslation->y(), relTranslation->z());
	btVector3 btTranslation = bulletRigidBody->getWorldTransform().getBasis() * btRelTranslation;

	btTransform btNewTransform;
	btNewTransform.setBasis(bulletRigidBody->getWorldTransform().getBasis());
	btNewTransform.setOrigin(bulletRigidBody->getWorldTransform().getOrigin() + btTranslation);

	bulletRigidBody->setWorldTransform(btNewTransform);
}

void
bullet::PhysicsWorld::BulletCollider::prependRotationY(float radians)
{
	btMatrix3x3	btRotation (btQuaternion(btVector3(0.0f, 1.0f, 0.0f), radians));

	std::shared_ptr<btRigidBody> bulletRigidBody = std::dynamic_pointer_cast<btRigidBody>(_bulletCollisionObject);

	btTransform btNewTransform;
	btNewTransform.setBasis(bulletRigidBody->getWorldTransform().getBasis() * btRotation);
	btNewTransform.setOrigin(bulletRigidBody->getWorldTransform().getOrigin());

	bulletRigidBody->setWorldTransform(btNewTransform);
}

bullet::PhysicsWorld::BulletCollider::Ptr
bullet::PhysicsWorld::BulletCollider::create(Collider::Ptr collider)
{
	BulletColliderPtr bulletCollider(new BulletCollider());

	bulletCollider->initialize(collider);

	return bulletCollider;
}
