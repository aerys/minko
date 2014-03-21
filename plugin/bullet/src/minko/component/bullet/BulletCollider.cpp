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

#include "minko/component/bullet/PhysicsWorld.hpp"

#include <btBulletDynamicsCommon.h>
#include "minko/math/tools.hpp"
#include <minko/component/bullet/ColliderData.hpp>
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

std::shared_ptr<btRigidBody>
bullet::PhysicsWorld::BulletCollider::rigidBody() const
{
	return std::dynamic_pointer_cast<btRigidBody>(_bulletCollisionObject);
}

void
bullet::PhysicsWorld::BulletCollider::initialize(ColliderData::Ptr data)
{
	if (data == nullptr)
		throw std::invalid_argument("data");

	std::shared_ptr<btCollisionShape>	bulletCollisionShape	= initializeCollisionShape(data->shape());	
	std::shared_ptr<btMotionState>		bulletMotionState		= initializeMotionState(data);

#ifdef DEBUG_PHYSICS
	std::cout << "[" << data->name() << "]\tinit collision shape\n\t- local scaling = " << bulletCollisionShape->getLocalScaling()[0] 
	<< "\n\t- margin = " << bulletCollisionShape->getMargin() << std::endl;
#endif // DEBUG_PHYSICS

	initializeCollisionObject(
		data, 
		bulletCollisionShape, 
		bulletMotionState
	);
}

std::shared_ptr<btCollisionShape>
bullet::PhysicsWorld::BulletCollider::initializeCollisionShape(AbstractPhysicsShape::Ptr shape) const
{
	if (shape == nullptr)
		throw std::invalid_argument("shape");

	std::shared_ptr<btCollisionShape> bulletShape = nullptr;
	switch(shape->type())
	{
	case AbstractPhysicsShape::SPHERE:
		bulletShape = initializeSphereShape(std::dynamic_pointer_cast<SphereShape>(shape));
		break;

	case AbstractPhysicsShape::BOX:
		bulletShape = initializeBoxShape(std::dynamic_pointer_cast<BoxShape>(shape));
		break;

	case AbstractPhysicsShape::CONE:
		bulletShape = initializeConeShape(std::dynamic_pointer_cast<ConeShape>(shape));
		break;

	case AbstractPhysicsShape::CYLINDER:
		bulletShape = initializeCylinderShape(std::dynamic_pointer_cast<CylinderShape>(shape));
		break;

	default:
		throw std::logic_error("Unsupported physics shape");
	}

	bulletShape->setLocalScaling(btVector3(
		shape->localScaling()->x(), 
		shape->localScaling()->y(), 
		shape->localScaling()->z()
	));

	bulletShape->setMargin(shape->margin());

	return bulletShape;
}

std::shared_ptr<btCollisionShape>
bullet::PhysicsWorld::BulletCollider::initializeSphereShape(SphereShape::Ptr sphere) const
{
	return std::shared_ptr<btSphereShape>(new btSphereShape(sphere->radius()));
}

std::shared_ptr<btCollisionShape>
bullet::PhysicsWorld::BulletCollider::initializeBoxShape(BoxShape::Ptr box) const
{
	btVector3 halfExtents (box->halfExtentX(), box->halfExtentY(), box->halfExtentZ());

	return std::shared_ptr<btBoxShape>(new btBoxShape(halfExtents));
}

std::shared_ptr<btCollisionShape>
bullet::PhysicsWorld::BulletCollider::initializeConeShape(ConeShape::Ptr cone) const
{
	return std::shared_ptr<btConeShape>(new btConeShape(cone->radius(), cone->height()));
}

std::shared_ptr<btCollisionShape>
bullet::PhysicsWorld::BulletCollider::initializeCylinderShape(CylinderShape::Ptr cylinder) const
{
	btVector3 halfExtents (cylinder->halfExtentX(), cylinder->halfExtentY(), cylinder->halfExtentZ());

	return std::shared_ptr<btCylinderShape>(new btCylinderShape(halfExtents));
}

std::shared_ptr<btMotionState>
bullet::PhysicsWorld::BulletCollider::initializeMotionState(ColliderData::Ptr collider) const
{
	return std::shared_ptr<btMotionState>(new btDefaultMotionState(
		btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, 0.0f, 0.0f)),
		btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, 0.0f, 0.0f))
	));
}

void
bullet::PhysicsWorld::BulletCollider::initializeCollisionObject(ColliderData::Ptr					data,
																std::shared_ptr<btCollisionShape>	bulletCollisionShape, 
																std::shared_ptr<btMotionState>		bulletMotionState) 
{
	// only rigid objects are considerered for the moment

	btVector3 inertia (0.0, 0.0, 0.0);
	if (data->inertia() == nullptr)
	{
		if (data->mass() > 0.0f)
			bulletCollisionShape->calculateLocalInertia(data->mass(), inertia);
	}
	else
	{
		inertia.setX(data->inertia()->x());
		inertia.setY(data->inertia()->y());
		inertia.setZ(data->inertia()->z());
	}

	// construction of a new rigid collision object
	auto info = btRigidBody::btRigidBodyConstructionInfo(
		data->mass(),
		bulletMotionState.get(),
		bulletCollisionShape.get(),
		inertia
	);

	info.m_friction					= data->friction();
	info.m_rollingFriction			= data->rollingFriction();
	info.m_restitution				= data->restitution();

	auto bulletRigidBody	= std::shared_ptr<btRigidBody>(new btRigidBody(info));

	_bulletCollisionShape	= bulletCollisionShape;
	_bulletMotionState		= bulletMotionState;
	_bulletCollisionObject	= bulletRigidBody;
}

