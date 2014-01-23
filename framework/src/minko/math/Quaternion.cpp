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

#include "minko/math/Quaternion.hpp"

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
	if (l > 1e-6f)
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
Quaternion::invert()
{
	normalize();
	_i = -_i;
	_j = -_j;
	_k = -_k;

	return shared_from_this();
}

Quaternion::Ptr
Quaternion::fromMatrix(Matrix4x4ConstPtr matrix)
{
#ifdef DEBUG
	if (fabsf(matrix->determinant3x3() - 1.0f) > 1e-3f)
		std::cerr << "Warning: matrix that is to be converted to quaternion does not represent a proper rotation (det3x3 = " << matrix->determinant3x3() << ")." << std::endl;
#endif // DEBUG

	const auto& m	= matrix->data();

	const float a1	= m[0];
	const float a2	= m[1];
	const float a3	= m[2];

	const float b1	= m[4];
	const float b2	= m[5];
	const float b3	= m[6];

	const float c1	= m[8];
	const float c2	= m[9];
	const float c3	= m[10];

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;

	const float t	= a1 + b2 + c3;
	if (t > 0.0f)
	{
		const float s = sqrtf(1.0f + t) * 2.0f;
		x = (c2 - b3) / s;
		y = (a3 - c1) / s;
		z = (b1 - a2) / s;
		w = 0.25f * s;
	}
	else if (a1 > b2 && a1 > c3 )  
	{	 
		const float s = sqrtf(1.0f + a1 - b2 - c3) * 2.0f;
		x = 0.25f * s;
		y = (b1 + a2) / s;
		z = (a3 + c1) / s;
		w = (c2 - b3) / s;
	} 
	else if (b2 > c3) 
	{  
		const float s = sqrtf(1.0f + b2 - a1 - c3) * 2.0f;
		x = (b1 + a2) / s;
		y = 0.25f * s;
		z = (c2 + b3) / s;
		w = (a3 - c1) / s;
	} 
	else 
	{ 
		const float s = sqrtf(1.0f + c3 - a1 - b2) * 2.0f;
		x = (a3 + c1) / s;
		y = (c2 + b3) / s;
		z = 0.25f * s;
		w = (b1 - a2) / s;
	}

	_i = x;
	_j = y;
	_k = z;
	_r = w;

	return normalize();
}

Matrix4x4::Ptr
Quaternion::toMatrix(Matrix4x4::Ptr output)const
{
#ifdef DEBUG
	if (fabsf(length() - 1.0f) > 1e-3f)
		std::cerr << "Warning: quaternion not normalized prior conversion to rotation matrix." << std::endl;
#endif // DEBUG

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

Quaternion::Ptr
Quaternion::slerp(Quaternion::Ptr target, float ratio)
{
	const float q1x = _i;
	const float q1y = _j;
	const float q1z = _k;
	const float q1w = _r;

	float q2x = target->_i;
	float q2y = target->_j;
	float q2z = target->_k;
	float q2w = target->_r;
		
	float cosOmega = q1x * q2x + q1y * q2y + q1z * q2z + q1w * q2w;

	// adjust signs (if necessary)
	if (cosOmega < 0.0f)
	{
		cosOmega	= -cosOmega;
		q2x			= -q2x;   
		q2y			= -q2y;
		q2z			= -q2z;
		q2w			= -q2w;
	} 

	float weight1 = 0.0f;
	float weight2 = 1.0f;

	if ((1.0f - cosOmega) > 1e-4f)
	{
		// slerp
		const float omega		= acosf(cosOmega);
		const float sinOmega	= sinf(omega);

		weight1  = sinf((1.0f - ratio) * omega)	/ sinOmega;
		weight2  = sinf(ratio * omega)			/ sinOmega;
	} else
	{
		// lerp for small angles
		weight1 = 1.0f - ratio;
		weight2 = ratio;
	}

	_i = weight1 * q1x + weight2 * q2x;
	_j = weight1 * q1y + weight2 * q2y;
	_k = weight1 * q1z + weight2 * q2z;
	_r = weight1 * q1w + weight2 * q2w;
	
	return shared_from_this();
}