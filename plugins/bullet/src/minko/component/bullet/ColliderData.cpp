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

#include "minko/component/bullet/ColliderData.hpp"

#include <minko/math/Matrix4x4.hpp>
#include <minko/component/bullet/AbstractPhysicsShape.hpp>
#include <minko/component/bullet/PhysicsWorld.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::component;

bullet::ColliderData::ColliderData(float						mass,
								   AbstractPhysicsShape::Ptr	shape,
								   Vector3::Ptr					inertia):
	_uid(0),
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
	_graphicsWorldTransformChanged(Signal<Ptr, Matrix4x4Ptr>::create()),
	_collisionStarted(Signal<Ptr, Ptr>::create()),
	_collisionEnded(Signal<Ptr, Ptr>::create())
{

}

void
bullet::ColliderData::correction(Matrix4x4::Ptr value)
{
	_correctionMatrix->copyFrom(value);
}

void
bullet::ColliderData::linearVelocity(float x, float y, float z)
{
	_linearVelocity->setTo(x, y, z);
}

void
bullet::ColliderData::angularVelocity(float x, float y, float z)
{
	_angularVelocity->setTo(x, y, z);
}

void
bullet::ColliderData::linearFactor(float x, float y, float z)
{
	_linearFactor->setTo(x, y, z);
}

void
bullet::ColliderData::angularFactor(float x, float y, float z)
{
	_angularFactor->setTo(x, y, z);
}
