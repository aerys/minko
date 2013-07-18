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

#include "AbstractPhysicsShape.hpp"
#include <minko/math/Matrix4x4.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::component;

bullet::AbstractPhysicsShape::AbstractPhysicsShape(Type type):
	_type(type),
	_margin(0.0f),
	_localScaling(1.0f),
	_centerOfMassOffset(Matrix4x4::create()),
	_centerOfMassTranslation(Vector3::create(0.0f, 0.0f, 0.0f)),
	_centerOfMassRotation(Quaternion::create()->identity()),
	_shapeChanged(Signal<Ptr>::create())
{

}

void
bullet::AbstractPhysicsShape::setCenterOfMassOffset(Matrix4x4::Ptr centerOfMassOffset)
{
	_centerOfMassOffset->copyFrom(centerOfMassOffset); // TODO: should disappear soon

	const float scaling = powf(centerOfMassOffset->determinant3x3(), 1.0f/3.0f);

	Vector3Ptr translation = centerOfMassOffset->translationVector();	
	_centerOfMassTranslation->setTo(
		translation->x() * scaling,
		translation->y() * scaling,
		translation->z() * scaling
	);

	_centerOfMassRotation->identity();
	if (fabsf(scaling) < 1e-6f)
		return;
	const float invScaling = 1.0f/scaling;
	_centerOfMassRotation = Matrix4x4::create()
		->copyFrom(centerOfMassOffset)
		->prependScaling(invScaling, invScaling, invScaling) // remove scaling effect
		->rotation();
}