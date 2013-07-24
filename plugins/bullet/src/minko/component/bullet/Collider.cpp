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
	//_worldTransform(Matrix4x4::create()->identity()),
	//_scaleCorrection(1.0f),
	_correctionMatrix(Matrix4x4::create()->identity()),
	//_physicsTransform(Matrix4x4::create()->identity()),
	//_physicsStartOrientation(Quaternion::create()),
	//_physicsStartPosition(Vector3::create()),
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

/*
void
bullet::Collider::setWorldTransform(Matrix4x4::Ptr modelToWorldMatrix)
{
	
	// uniform scaling assumed
	_scaleCorrection	= powf(modelToWorldMatrix->determinant3x3(), 1.0f/3.0f);

	if (fabsf(_scaleCorrection) < 1e-6f)
		throw std::logic_error("Physics simulation requires matrices with non-null uniform scaling (world transform matrix).");

	// initialize collider's world transform from a scaling-free matrix
	const float invScaling = 1.0f / _scaleCorrection;
	auto scalingFreeMatrix = Matrix4x4::create()
		->copyFrom(modelToWorldMatrix)
		->prependScaling(invScaling, invScaling, invScaling);

	const float newScaling = powf(scalingFreeMatrix->determinant3x3(), 1.0f/3.0f);
	if (fabsf(newScaling - 1.0f) > 1e-3f)
	{
		std::stringstream stream;
		stream << "Model to world matrix does not have a uniform scaling.\n\tmatrix = "
			<< std::to_string(modelToWorldMatrix) << std::endl;
		throw std::logic_error(stream.str());
	}

	// decompose the specified transform into its rotational and translational components
	// (Bullet requires this)
	auto rotation		= scalingFreeMatrix->rotationQuaternion();
	auto translation	= scalingFreeMatrix->translationVector();
	_worldTransform->initialize(rotation, translation);
	
}

void
bullet::Collider::updateColliderWorldTransform(Matrix4x4::Ptr colliderWorldTransform)
{
#ifdef DEBUG
	if (fabsf(fabsf(colliderWorldTransform->determinant3x3()) - 1.0f) > 1e-3f)
		throw std::logic_error("Update of collider's world transform can only involve scaling-free matrices.");
#endif // DEBUG

	// correct scaling lost at initialization of the collider's world transform
	const std::vector<float>& m(colliderWorldTransform->values());

	Vector3Ptr offset = _shape->centerOfMassTranslation();

	_worldTransform
		->initialize(
		m[0]*_scaleCorrection, m[1]*_scaleCorrection, m[2]*_scaleCorrection, m[3] + offset->x(),
		m[4]*_scaleCorrection, m[5]*_scaleCorrection, m[6]*_scaleCorrection, m[7] + offset->y() ,
		m[8]*_scaleCorrection, m[9]*_scaleCorrection, m[10]*_scaleCorrection, m[11] + offset->z(),
		0.0f, 0.0f, 0.0f, 1.0f
		);

	transformChanged()->execute(shared_from_this());
}
*/

/*
void
bullet::Collider::initializePhysicsFromGraphicsWorldTransform(Matrix4x4Ptr graphicsMatrix)
{
	
	// remove the influence of scaling and shear, but record it in order to correct the graphics matrix
	auto graphicsNoScaleMatrix = Matrix4x4::create();
	PhysicsWorld::removeScalingShear(graphicsMatrix, _worldTransform, _correctionMatrix);

	

	// get the body's position from the translation
	_physicsStartPosition		= graphicsTransform->translationVector(_physicsStartPosition);

	// extract the rotational part of the matrix (once its translation has been removed) by orthogonalization
	auto decompQR				= Matrix4x4::create()
		->copyFrom(graphicsTransform)
		->appendTranslation(-(*_physicsStartPosition))
		->decomposeQR();
	_physicsStartOrientation	= decompQR.first->rotationQuaternion();

	_worldTransform->initialize(_physicsStartOrientation, _physicsStartPosition);

	// record the lost scale and shear in the correction matrix
	_correctionMatrix->copyFrom(decompQR.second);
	

#ifdef DEBUG_PHYSICS
	std::cout << "[" << _name << "]\tinitialize from graphics matrix\n- correction =\n" << std::to_string(_correctionMatrix) << std::endl;
#endif // DEBUG_PHYSICS
	
}
*/

void
bullet::Collider::updateGraphicsTransformFromPhysics(Matrix4x4::Ptr physicsTransform)
{
	// bullet returns the world transform of the center-of-mass, need to account for possible offset
	auto graphicsTransform = Matrix4x4::create()
		->copyFrom(_correctionMatrix)
		->append(_shape->physicsToGraphics())
		->append(physicsTransform);

	graphicsWorldTransformChanged()->execute(shared_from_this(), graphicsTransform);
}

