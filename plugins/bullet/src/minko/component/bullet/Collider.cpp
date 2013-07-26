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

#include "Collider.hpp"

#include <minko/math/Matrix4x4.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>
#include <minko/component/bullet/PhysicsWorld.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::component;

bullet::Collider::Collider(float						mass,
						   AbstractPhysicsShape::Ptr	shape,
						   Vector3::Ptr					inertia):
	_name(""),
	_mass(mass),
	_correctionMatrix(Matrix4x4::create()->identity()),
	_shape(shape),
	_inertia(inertia),
	_linearVelocity(Vector3::create(0.0f, 0.0f, 0.0f)),
	_linearFactor(Vector3::create(1.0f, 1.0f, 1.0f)),
	_linearDamping(0.0f),
	_linearSleepingThreshold(0.8f),
	_angularVelocity(Vector3::create(0.0f, 0.0f, 0.0f)),
	_angularFactor(Vector3::create(1.0f, 1.0f, 1.0f)),
	_angularDamping(0.0f),
	_angularSleepingThreshold(1.0f),
	_restitution(0.0f),
	_friction(0.5f),
	_rollingFriction(0.0f),
	_deactivationDisabled(false),
	_transformChanged(Signal<Ptr>::create()),
	_graphicsWorldTransformChanged(Signal<Ptr, Matrix4x4Ptr>::create())
{

}

void
bullet::Collider::setCorrectionMatrix(Matrix4x4::Ptr value)
{
	_correctionMatrix->copyFrom(value);
}

void
bullet::Collider::setLinearVelocity(float x, float y, float z)
{
	_linearVelocity->setTo(x, y, z);
}

void
bullet::Collider::setAngularVelocity(float x, float y, float z)
{
	_angularVelocity->setTo(x, y, z);
}

void
bullet::Collider::setLinearFactor(float x, float y, float z)
{
	_linearFactor->setTo(x, y, z);
}

void
bullet::Collider::setAngularFactor(float x, float y, float z)
{
	_angularFactor->setTo(x, y, z);
}

void
bullet::Collider::updateGraphicsTransform(Matrix4x4::Ptr graphicsNoScaleTransform)
{
#ifdef DEBUG_PHYSICS
	const float det3x3 = fabsf(graphicsNoScaleTransform->determinant3x3());
	if (fabsf(det3x3 - 1.0f) > 1e-3f)
		throw std::logic_error("Updates of colliders' graphics transforms must involve pure rotation-translation matrices.");
#endif // DEBUG_PHYSICS

	auto graphicsTransform = Matrix4x4::create()
		->copyFrom(_correctionMatrix)
		->append(graphicsNoScaleTransform);

	graphicsWorldTransformChanged()->execute(
		shared_from_this(), 
		graphicsTransform
	);
}

void
bullet::Collider::updateGraphicsTransformFromPhysics(Matrix4x4::Ptr physicsNoScaleTransform)
{
//	auto invOffsetRot = Matrix4x4::create()->copyFrom(_shape->_centerOfMassRotation->toMatrix())->invert();

	auto graphicsTransform = Matrix4x4::create()
		->copyFrom(_correctionMatrix)
		//->appendTranslation(-(*_shape->_centerOfMassTranslation))
		//->append(Quaternion::create()->copyFrom(_shape->_centerOfMassRotation)->invert())
		->append(_shape->_deltaTransformInverse)
		->append(physicsNoScaleTransform)
		//->appendTranslation(-(*_shape->_centerOfMassTranslation))
		;
	/*
	auto trf = Matrix4x4::create()
		->append(_shape->_centerOfMassRotation)
		->appendTranslation(_shape->_centerOfMassTranslation)
		->invert();

	auto graphicsTransform = Matrix4x4::create()
		->copyFrom(_correctionMatrix)
		->append(trf)
		->append(physicsNoScaleTransform);
		*/

	// bullet returns the world transform of the center-of-mass, need to account for possible offset
	/*
	auto graphicsNoScaleTransform = Matrix4x4::create()
		->copyFrom(physicsTransform);
		*/
	/*
	auto graphicsNoScaleTransform = Matrix4x4::create()
		->copyFrom(_correctionMatrix) // account for lost scaling/shear
		->append(_shape->physicsToGraphics()) // account for collision shape offset
		->append(physicsTransform);
	*/
	
	/*
	auto graphicsNoScaleTransform = Matrix4x4::create()
		->copyFrom(_correctionMatrix) // account for lost scaling/shear
		->append(physicsTransform)
		->append(_shape->centerOfMassOffset()); // account for collision shape offset
	*/
	PhysicsWorld::print(std::cout << "updateGraphicsTransformFromPhysics\tgraphics = \n", graphicsTransform) << std::endl;

	graphicsWorldTransformChanged()->execute(
		shared_from_this(), 
		graphicsTransform
	);
}

