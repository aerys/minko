/*
Copyright (c) 2014 Aerys

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

#pragma once

#include <iostream>
#include <minko/math/Matrix4x4.hpp>
#include <minko/math/Quaternion.hpp>
#include <minko/math/Vector3.hpp>
#include <LinearMath/btTransform.h>

namespace minko
{
	namespace math
	{
		inline
		btVector3
		convert(Vector3::Ptr);

		inline
		Matrix4x4::Ptr
		removeScalingShear(Matrix4x4::Ptr	input, 
						   Matrix4x4::Ptr	output		= nullptr, 
						   Matrix4x4::Ptr	correction	= nullptr);

		inline
		Matrix4x4::Ptr
		fromBulletTransform(const btTransform&, 
							Matrix4x4::Ptr = nullptr);
		
		inline
		btTransform&
		toBulletTransform(Matrix4x4::Ptr, 
						  btTransform&);
		
		inline
		btTransform&
		toBulletTransform(Quaternion::Ptr, 
						  Vector3::Ptr, 
						  btTransform&);

		inline
		std::ostream&
		print(std::ostream&, const btTransform&);
	}
}

inline
btVector3
minko::math::convert(minko::math::Vector3::Ptr value)
{
	return btVector3(value->x(), value->y(), value->z());
}

inline
minko::math::Matrix4x4::Ptr
minko::math::removeScalingShear(minko::math::Matrix4x4::Ptr	input, 
							    minko::math::Matrix4x4::Ptr	output, 
								minko::math::Matrix4x4::Ptr	correction)
{
	static auto	matrix = Matrix4x4::create();

	if (output == nullptr)
		output = Matrix4x4::create();

	if (correction == nullptr)
		correction = Matrix4x4::create();

	// remove translational component, then perform QR decomposition
	auto translation = input->translation();
	matrix
		->copyFrom(input)
		->appendTranslation(-(*translation))
		->decomposeQR(output, correction);

	return output->appendTranslation(translation);
}


inline
minko::math::Matrix4x4::Ptr
minko::math::fromBulletTransform(const btTransform&				transform,
								 minko::math::Matrix4x4::Ptr	output)
{
	auto basis			= transform.getBasis();
	auto translation	= transform.getOrigin();

	if (output == nullptr)
		output = Matrix4x4::create();

	return output->initialize(
		basis[0][0], basis[0][1], basis[0][2], translation[0],
		basis[1][0], basis[1][1], basis[1][2], translation[1],
		basis[2][0], basis[2][1], basis[2][2], translation[2],
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

inline
btTransform&
minko::math::toBulletTransform(minko::math::Matrix4x4::Ptr	transform,
							   btTransform&					output)
{
	return toBulletTransform(
		transform->rotationQuaternion(), 
		transform->translation(), 
		output
	);
}

inline
btTransform&
minko::math::toBulletTransform(minko::math::Quaternion::Ptr	rotation, 
							   minko::math::Vector3::Ptr	translation, 
							   btTransform&					output)
{
	btQuaternion	btRotation(rotation->i(), rotation->j(), rotation->k(), rotation->r());
	btVector3		btOrigin(translation->x(), translation->y(), translation->z());

	output.setOrigin(btOrigin);
	output.setRotation(btRotation);

	return output;
}

inline
std::ostream&
minko::math::print(std::ostream&		out, 
				   const btTransform&	bulletTransform)
{
	const btVector3&	origin	(bulletTransform.getOrigin());
	const btMatrix3x3&	basis	(bulletTransform.getBasis());

	out << "\t- origin\t= [" << origin[0] << "\t" << origin[1] << "\t" << origin[2] << "]\n\t- basis \t=\n" 
		<< "\t[" << basis[0][0] << "\t" << basis[0][1] << "\t" << basis[0][2] 
		<< "\n\t " << basis[1][0] << "\t" << basis[1][1] << "\t" << basis[1][2] 
		<< "\n\t " << basis[2][0] << "\t" << basis[2][1] << "\t" << basis[2][2] << "]";

	return out;
}