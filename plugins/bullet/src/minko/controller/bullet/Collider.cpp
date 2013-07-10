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
#include <minko/controller/bullet/AbstractPhysicsShape.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::controller;

bullet::Collider::Collider(float						mass,
						   AbstractPhysicsShape::Ptr	shape,
						   Vector3::Ptr					inertia,
						   Matrix4x4::Ptr				centerOfMassOffset):
_mass(mass),
	_worldTransform(Matrix4x4::create()),
	_centerOfMassOffset(centerOfMassOffset),
	_scaleCorrectionMatrix(Matrix4x4::create()),
	_shape(shape),
	_inertia(inertia),
	_linearVelocity(Vector3::create(0.0f, 0.0f, 0.0f)),
	_linearFactor(Vector3::create(1.0f, 1.0f, 1.0f)),
	_linearDamping(0.0f),
	_angularVelocity(Vector3::create(0.0f, 0.0f, 0.0f)),
	_angularFactor(Vector3::create(1.0f, 1.0f, 1.0f)),
	_angularDamping(0.0f),
	_restitution(0.0f),
	_transformChanged(Signal<Ptr>::create())
{
	_worldTransform->identity();
	_scaleCorrectionMatrix->identity();
}

void
	bullet::Collider::setWorldTransform(Matrix4x4::Ptr modelToWorldMatrix)
{
	if (fabsf(fabsf(modelToWorldMatrix->determinant3x3()) - 1.0f) > std::numeric_limits<float>::epsilon())
		throw std::invalid_argument("Colliders are currently incompatible with Transforms with scaling and shear.");

	// decompose the specified transform into its rotational and translational components
	// (Bullet requires this)
	auto rotation		= modelToWorldMatrix->rotation();
	auto translation	= modelToWorldMatrix->translation();
	_worldTransform->initialize(rotation, translation);

	// record the corrective term that keeps the
	// scale/shear lost by the collider's world transform
	// @todo currently, scale/shear are not properly handled, the scale correction matrix always end up being the identity matrix...
	_scaleCorrectionMatrix	= Matrix4x4::create()
		->copyFrom(_worldTransform)
		->invert()
		->append(modelToWorldMatrix);
}

void
	bullet::Collider::updateColliderWorldTransform(Matrix4x4::Ptr colliderWorldTransform)
{
	_worldTransform
		->copyFrom(colliderWorldTransform)
		->append(_scaleCorrectionMatrix);

	transformChanged()->execute(shared_from_this());
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