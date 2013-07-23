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
#include <minko/component/bullet/PhysicsWorld.hpp>

using namespace minko;
using namespace minko::math;
using namespace minko::component;

bullet::AbstractPhysicsShape::AbstractPhysicsShape(Type type):
	_type(type),
	_margin(0.0f),
	_localScaling(1.0f),
	_centerOfMassOffset(Matrix4x4::create()->identity()),
	_centerOfMassOffsetInverse(Matrix4x4::create()->identity()),
	_physicsToGraphics(Matrix4x4::create()->identity()),
	_centerOfMassTransform(Matrix4x4::create()->identity()),
	_centerOfMassInverseTransform(Matrix4x4::create()->identity()),
	_centerOfMassTranslation(Vector3::create(0.0f, 0.0f, 0.0f)),
	_centerOfMassRotation(Quaternion::create()->identity()),
	_shapeChanged(Signal<Ptr>::create())
{

}

void
bullet::AbstractPhysicsShape::setCenterOfMassOffset(Matrix4x4::Ptr centerOfMassOffset,
													Matrix4x4::Ptr modelToWorld)
{
	/*
	const float offsetScaling = powf(centerOfMassOffset->determinant3x3(), 1.0f/3.0f);

	Vector3Ptr translation = centerOfMassOffset->translationVector();




	_centerOfMassTranslation->setTo(
		-translation->x(),
		-translation->y(),
		-translation->z()
	);

	_centerOfMassRotation->identity();
	const float scaling = powf(centerOfMassOffset->determinant3x3(), 1.0f/3.0f);
	if (fabsf(scaling) < 1e-6f)
		return;
	const float invScaling = 1.0f/scaling;
	_centerOfMassRotation = Matrix4x4::create()
		->copyFrom(centerOfMassOffset)
		->prependScaling(invScaling, invScaling, invScaling) // remove scaling effect
		->rotationQuaternion();

#ifdef DEBUG
	std::cout << "physics shape offset\n\t- translation = " << _centerOfMassTranslation->x() 
		<< " " << _centerOfMassTranslation->y() << " " << _centerOfMassTranslation->z() 
		<< "\n\t- rotation = " << std::to_string(_centerOfMassRotation->toMatrix()) 
		<< "\n\tfrom delta matrix = " << std::to_string(centerOfMassOffset)
		<< "\n\twith model->world = " << std::to_string(modelToWorld) << std::endl;
#endif //DEBUG

	_centerOfMassTransform->initialize(_centerOfMassRotation, _centerOfMassTranslation);
	_centerOfMassInverseTransform->copyFrom(_centerOfMassTransform)->invert();
	*/
}

void
bullet::AbstractPhysicsShape::initializeCenterOfMassOffset(Matrix4x4::Ptr deltaMatrix, 
														   Matrix4x4::Ptr graphicsMatrix)
{
	// IMPORTANT: all scaling contained in the deltaMatrix is expected to be properly repercuted
	// on the dimensions of the collision shape itself. 

	Matrix4x4::Ptr deltaNoScaleMatrix	= Matrix4x4::create();
	PhysicsWorld::removeScalingShear(deltaMatrix, deltaNoScaleMatrix);
	auto deltaTranslation	= deltaNoScaleMatrix->translationVector();
	auto deltaRotation		= deltaNoScaleMatrix->rotationQuaternion()->toMatrix();

	Matrix4x4::Ptr graphicsNoScaleMatrix	= Matrix4x4::create();
	PhysicsWorld::removeScalingShear(graphicsMatrix, graphicsNoScaleMatrix);

	// matrix used to construct Bullet's motion state offset
	_centerOfMassOffset
		->copyFrom(graphicsNoScaleMatrix)->invert()
		->append(deltaRotation)
		->append(graphicsNoScaleMatrix)
		->appendTranslation(deltaTranslation)
		->invert();

	// matrix used to specify a transform to Bullet's motion state
	_centerOfMassOffsetInverse
		->copyFrom(_centerOfMassOffset)
		->invert();

	// matrix used to convert Bullet's physics matrix to Minko graphics matrix
	_physicsToGraphics
		->identity()
		->append(deltaRotation)
		->appendTranslation(deltaTranslation)
		->invert();
}