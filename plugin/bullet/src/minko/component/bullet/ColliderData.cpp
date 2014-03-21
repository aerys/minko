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

#include <minko/component/bullet/AbstractPhysicsShape.hpp>
#include <minko/component/bullet/PhysicsWorld.hpp>
#include "minko/math/tools.hpp"

#include <btBulletDynamicsCommon.h>

using namespace minko;
using namespace minko::math;
using namespace minko::component;

/*static*/ const short  bullet::ColliderData::DEFAULT_DYNAMIC_FILTER    = btBroadphaseProxy::DefaultFilter;
/*static*/ const short  bullet::ColliderData::DEFAULT_STATIC_FILTER     = btBroadphaseProxy::StaticFilter;
/*static*/ const short  bullet::ColliderData::DEFAULT_DYNAMIC_MASK      = btBroadphaseProxy::AllFilter;
/*static*/ const short  bullet::ColliderData::DEFAULT_STATIC_MASK       = btBroadphaseProxy::AllFilter ^ btBroadphaseProxy::StaticFilter;

bullet::ColliderData::ColliderData(float						mass,
								   AbstractPhysicsShape::Ptr	shape,
								   Vector3::Ptr					inertia):
	_uid(0),
	_mass(mass),
	_correctionMatrix(Matrix4x4::create()->identity()),
	_shape(shape),
	_inertia(inertia),
	_restitution(0.0f),
	_friction(0.5f),
	_rollingFriction(0.0f),
	_triggerCollisions(false),
	_physicsWorldTransformChanged(Signal<Ptr, Matrix4x4Ptr>::create()),
	_graphicsWorldTransformChanged(Signal<Ptr, Matrix4x4Ptr>::create()),
	_collisionStarted(Signal<Ptr, Ptr>::create()),
	_collisionEnded(Signal<Ptr, Ptr>::create()),
    _collisionFilterChanged(Signal<Ptr>::create())
{

}

void
bullet::ColliderData::synchronizePhysicsWithGraphics(Matrix4x4::Ptr graphicsTransform,
													 Matrix4x4::Ptr graphicsNoScaleTransform,
													 Matrix4x4::Ptr	centerOfMassOffset)
{
	if (graphicsNoScaleTransform == nullptr)
		graphicsNoScaleTransform = Matrix4x4::create();
	if (centerOfMassOffset == nullptr)
		centerOfMassOffset = Matrix4x4::create();

	// remove the scaling/shear from the graphics transform, but record it to restitute it during rendering
	removeScalingShear(
		graphicsTransform, 
		graphicsNoScaleTransform,
		_correctionMatrix
	);

	centerOfMassOffset
		->copyFrom(graphicsNoScaleTransform)->invert()
		->append(_shape->deltaTransform())
		->append(graphicsNoScaleTransform)
		->invert();

	static auto physicsModelToWorld = Matrix4x4::create();

	physicsModelToWorld
		->copyFrom(centerOfMassOffset)->invert()
		->prepend(graphicsNoScaleTransform);

	updatePhysicsTransform(physicsModelToWorld);
}

void
bullet::ColliderData::updatePhysicsTransform(Matrix4x4::Ptr physicsModelToWorld) // triggers transform updating signals
{
	auto graphicsModelToWorld = math::Matrix4x4::create()
		->copyFrom(physicsModelToWorld)
		->prepend(_shape->deltaTransformInverse())
		->prepend(_correctionMatrix);

	_physicsWorldTransformChanged->execute(shared_from_this(), physicsModelToWorld);
	_graphicsWorldTransformChanged->execute(shared_from_this(), graphicsModelToWorld);
}

bullet::ColliderData::Ptr
bullet::ColliderData::correction(Matrix4x4::Ptr value)
{
	_correctionMatrix->copyFrom(value);

	return shared_from_this();
}
