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
	_localScaling(Vector3::create(1.0f, 1.0f, 1.0f)),
_centerOfMassTranslation(Vector3::create(0.0f, 0.0f, 0.0f)),
_centerOfMassRotation(Quaternion::create()->identity()),
	_deltaTransform(Matrix4x4::create()->identity()),
	_deltaTransformInverse(Matrix4x4::create()->identity()),
	_centerOfMassOffset(Matrix4x4::create()->identity()),
	_physicsToGraphics(Matrix4x4::create()->identity()),
	_shapeChanged(Signal<Ptr>::create())
{

}
				
void
bullet::AbstractPhysicsShape::localScaling(float x, float y, float z)
{
	_localScaling->setTo(x, y, z);
}

void
bullet::AbstractPhysicsShape::initialize(Matrix4x4::Ptr deltaTransform, 
										 Matrix4x4::Ptr graphicsStartTransform)
{
	auto correction			= Matrix4x4::create();
	PhysicsWorld::removeScalingShear(
		deltaTransform, 
		_deltaTransform, 
		correction
	);

	_deltaTransformInverse
		->copyFrom(_deltaTransform)
		->invert();

	_deltaTransform->translationVector(_centerOfMassTranslation);
	_deltaTransform->rotationQuaternion(_centerOfMassRotation);
	localScaling(correction->values()[0], correction->values()[5], correction->values()[10]);

#ifdef DEBUG_PHYSICS
	Matrix4x4::Ptr centerOfMassRotation = _centerOfMassRotation->toMatrix();
	const std::vector<float>& dRot(centerOfMassRotation->values());

	std::cout << "- delta.translation\t= [ " << _centerOfMassTranslation->x() << " " << _centerOfMassTranslation->y() << " " << _centerOfMassTranslation->z() << " ]" << std::endl;
	std::cout << "- delta.rotation\t=\n\t" << dRot[0] << " " << dRot[1] << " " << dRot[2] << "\n\t" << dRot[4] << " " << dRot[5] << " " << dRot[6]
	<< "\n\t" << dRot[8] << " " << dRot[9] << " " << dRot[10] << std::endl;
	std::cout << "- delta.scaling\t= [ " << _localScaling->x() << " " << _localScaling->y() << " " << _localScaling->z() << " ]" << std::endl;
#endif // DEBUG_PHYSICS

	/*
	PhysicsWorld::removeScalingShear(
		graphicsStartTransform,
		noScaleTransform
	);

	
	Matrix4x4::Ptr graphicsNoScaleTransform	= Matrix4x4::create();
	PhysicsWorld::removeScalingShear(graphicsTransform, graphicsNoScaleTransform);

	localScaling(deltaScaling->x(), deltaScaling->y(), deltaScaling->z());

	// matrix used to initialize Bullet's motion state offset
	_centerOfMassOffset
		->copyFrom(graphicsNoScaleTransform)->invert()
		->append(_centerOfMassRotation)
		->append(graphicsNoScaleTransform)
		->appendTranslation(_centerOfMassTranslation)
		->invert();

	// matrix used to convert Bullet's physics matrix to Minko graphics matrix
	_physicsToGraphics
		->identity()
		->append(_centerOfMassRotation)
		->appendTranslation(_centerOfMassTranslation)
		->invert();
		*/
}