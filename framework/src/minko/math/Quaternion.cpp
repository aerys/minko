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

#include "Quaternion.hpp"
#include <minko/math/Matrix4x4.hpp>

using namespace minko;
using namespace minko::math;

Quaternion::Ptr
	Quaternion::identity()
{
	_i = _j = _k = 0.0f;
	_r = 1.0f;

	return shared_from_this();
}


Quaternion::Ptr
	Quaternion::initialize(float radians, Vector3::Ptr axis)
{
	const float x		= axis->x();
	const float y		= axis->y();
	const float z		= axis->z();
	const float length	= sqrtf(x*x + y*y + z*z);
	if (length > 0.0f)
	{
		float sinHalf = sinf(radians*0.5f) / length;

		return setTo(x*sinHalf, y*sinHalf, z*sinHalf, cosf(radians*0.5f));
	}
	else
		return identity();
}

float
	Quaternion::lengthSquared() const
{
	return _i*_i + _j*_j + _k*_k + _r*_r;
}

float 
	Quaternion::length() const
{
	return sqrtf(lengthSquared());
}

Quaternion::Ptr
	Quaternion::normalize()
{
	const float l = length();
	if (l > 0.0f)
	{
		const float invLength = 1.0f/l;
		_i *= invLength;
		_j *= invLength;
		_k *= invLength;
		_r *= invLength;
	}

	return shared_from_this();
}

Quaternion::Ptr
	Quaternion::fromMatrix(Matrix4x4ConstPtr matrix)
{
	const float det3x3	= matrix->determinant3x3();
	if (fabsf(fabsf(det3x3) - 1.0f) > 1e-3f)
	{
		std::stringstream stream;
		stream << "Quaternion::fromMatrix:\tSpecified matrix does not represent a rotation matrix (3x3 determinant = " << det3x3 << ").";
		throw std::invalid_argument(stream.str());
	}
		
	const std::vector<float>& m(matrix->values());

	// "From Quaternion to Matrix and Back" by JMP van Warenen
	uint k0		= 2;
	uint k1		= 3;
	uint k2		= 0;
	uint k3		= 1;
	float s0	= -1.0f;
	float s1	= -1.0f;
	float s2	=  1.0f;

	if (m[0] + m[5] + m[10] > 0.0f)
	{ 
		k0	= 3; 
		k1	= 2; 
		k2	= 1; 
		k3	= 0; 
		s0	= 1.0f; 
		s1	= 1.0f; 
		s2	= 1.0f; 
	} 
	else if (m[0] > m[5] && m[0] > m[10]) 
	{ 
		k0	= 0; 
		k1	= 1; 
		k2	= 2; 
		k3	= 3; 
		s0	=  1.0f; 
		s1	= -1.0f; 
		s2	= -1.0f; 
	} 
	else if (m[5] > m[10]) 
	{ 
		k0	= 1; 
		k1	= 0; 
		k2	= 3; 
		k3	= 2; 
		s0	= -1.0f; 
		s1	=  1.0f; 
		s2	= -1.0f; 
	}

	float t	= s0*m[0] + s1*m[5] + s2*m[10] + 1.0f;
	if (fabsf(t) < std::numeric_limits<float>::epsilon())
		throw std::invalid_argument("impossible to convert rotation matrix to quaternion");

	float s	= 0.5f / sqrtf(t);

	float quaternion[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	quaternion[k0]	= s * t;
	quaternion[k1]	= s * (m[4] - s2*m[1]);
	quaternion[k2]	= s * (m[2] - s1*m[8]);
	quaternion[k3]	= s * (m[9] - s0*m[6]);

	return setTo(quaternion[0], quaternion[1], quaternion[2], quaternion[3]);
}

Matrix4x4::Ptr
	Quaternion::toMatrix(Matrix4x4::Ptr output)const
{
	if (fabsf(length() - 1.0f) > std::numeric_limits<float>::epsilon())
		throw std::logic_error("Quaternion must be normalized prior to its conversion to a rotation matrix.");

	float qx	= _i;
	float qy	= _j;
	float qz	= _k;
	float qw	= _r;
	float qxx2	= qx*qx*2.0f;
	float qxy2	= qx*qy*2.0f;
	float qxz2	= qx*qz*2.0f;
	float qxw2	= qx*qw*2.0f;
	float qyy2	= qy*qy*2.0f;
	float qyz2	= qy*qz*2.0f;
	float qyw2	= qy*qw*2.0f;
	float qzz2	= qz*qz*2.0f;
	float qzw2	= qz*qw*2.0f;
	float qww2	= qw*qw*2.0f;

	if (output == nullptr)
		output	= Matrix4x4::create();

	return output->initialize(
		1.0f - qyy2 - qzz2,	qxy2 - qzw2,		qxz2 + qyw2,		0.0f,
		qxy2 + qzw2,		1.0f - qxx2 - qzz2,	qyz2 - qxw2,		0.0f,
		qxz2 - qyw2,		qyz2 + qxw2,		1.0f - qxx2 - qyy2,	0.0f,
		0.0f,				0.0f,				0.0f,				1.0f
		);
}
