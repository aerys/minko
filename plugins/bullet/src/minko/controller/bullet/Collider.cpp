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
	_startScaleShearCorrection(Matrix4x4::create()),
	_shape(shape),
	_inertia(inertia),
	_transformChanged(Signal<Ptr>::create())
{
	_worldTransform->identity();
	_startScaleShearCorrection->identity();
}

void
	bullet::Collider::updateColliderWorldTransform(Matrix4x4::Ptr transform)
{
	_worldTransform
		->copyFrom(_startScaleShearCorrection)
		->append(transform);

	transformChanged()->execute(shared_from_this());
}

void
	bullet::Collider::initializeWorldTransform(Matrix4x4::Ptr transform)
{
	// decompose the specified transform into its rotational and translational components
	// (Bullet requires this)
	auto rotation		= transform->rotation();
	auto translation	= transform->translation();
	_worldTransform->initialize(rotation, translation);

	// record the starting scale/shear corrective term
	auto invColliderTransform	= Matrix4x4::create()
		->copyFrom(_worldTransform)
		->invert();

	_startScaleShearCorrection
		->copyFrom(transform)
		->append(invColliderTransform);
}