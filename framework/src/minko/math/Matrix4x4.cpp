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

#include "minko/math/Matrix4x4.hpp"

#include "minko/Signal.hpp"

using namespace minko;
using namespace minko::math;

Matrix4x4::Matrix4x4() :
	_m(16),
	_lock(false)
{
}

Matrix4x4::Matrix4x4(Matrix4x4::Ptr value) :
	_m(value->_m)
{
}

Matrix4x4::Ptr
Matrix4x4::prepend(float m00, float m01, float m02, float m03,
			 	   float m10, float m11, float m12, float m13,
			 	   float m20, float m21, float m22, float m23,
			 	   float m30, float m31, float m32, float m33)
{
	return initialize(
		_m[0] * m00 + _m[1] * m10 + _m[2] * m20 + _m[3] * m30,
		_m[0] * m01 + _m[1] * m11 + _m[2] * m21 + _m[3] * m31,
		_m[0] * m02 + _m[1] * m12 + _m[2] * m22 + _m[3] * m32,
		_m[0] * m03 + _m[1] * m13 + _m[2] * m23 + _m[3] * m33,
		_m[4] * m00 + _m[5] * m10 + _m[6] * m20 + _m[7] * m30,
		_m[4] * m01 + _m[5] * m11 + _m[6] * m21 + _m[7] * m31,
		_m[4] * m02 + _m[5] * m12 + _m[6] * m22 + _m[7] * m32,
		_m[4] * m03 + _m[5] * m13 + _m[6] * m23 + _m[7] * m33,
		_m[8] * m00 + _m[9] * m10 + _m[10] * m20 + _m[11] * m30,
		_m[8] * m01 + _m[9] * m11 + _m[10] * m21 + _m[11] * m31,
		_m[8] * m02 + _m[9] * m12 + _m[10] * m22 + _m[11] * m32,
		_m[8] * m03 + _m[9] * m13 + _m[10] * m23 + _m[11] * m33,
		_m[12] * m00 + _m[13] * m10 + _m[14] * m20 + _m[15] * m30,
		_m[12] * m01 + _m[13] * m11 + _m[14] * m21 + _m[15] * m31,
		_m[12] * m02 + _m[13] * m12 + _m[14] * m22 + _m[15] * m32,
		_m[12] * m03 + _m[13] * m13 + _m[14] * m23 + _m[15] * m33
	);
}

Matrix4x4::Ptr
Matrix4x4::append(float m00, float m01, float m02, float m03,
			 	  float m10, float m11, float m12, float m13,
			 	  float m20, float m21, float m22, float m23,
			 	  float m30, float m31, float m32, float m33)
{
	return initialize(
		m00 * _m[0] + m01 * _m[4] + m02 * _m[8] + m03 * _m[12],
		m00 * _m[1] + m01 * _m[5] + m02 * _m[9] + m03 * _m[13],
		m00 * _m[2] + m01 * _m[6] + m02 * _m[10] + m03 * _m[14],
		m00 * _m[3] + m01 * _m[7] + m02 * _m[11] + m03 * _m[15],
		m10 * _m[0] + m11 * _m[4] + m12 * _m[8] + m13 * _m[12],
		m10 * _m[1] + m11 * _m[5] + m12 * _m[9] + m13 * _m[13],
		m10 * _m[2] + m11 * _m[6] + m12 * _m[10] + m13 * _m[14],
		m10 * _m[3] + m11 * _m[7] + m12 * _m[11] + m13 * _m[15],
		m20 * _m[0] + m21 * _m[4] + m22 * _m[8] + m23 * _m[12],
		m20 * _m[1] + m21 * _m[5] + m22 * _m[9] + m23 * _m[13],
		m20 * _m[2] + m21 * _m[6] + m22 * _m[10] + m23 * _m[14],
		m20 * _m[3] + m21 * _m[7] + m22 * _m[11] + m23 * _m[15],
		m30 * _m[0] + m31 * _m[4] + m32 * _m[8] + m33 * _m[12],
		m30 * _m[1] + m31 * _m[5] + m32 * _m[9] + m33 * _m[13],
		m30 * _m[2] + m31 * _m[6] + m32 * _m[10] + m33 * _m[14],
		m30 * _m[3] + m31 * _m[7] + m32 * _m[11] + m33 * _m[15]
	);
}

Matrix4x4::Ptr
Matrix4x4::initialize(float m00, float m01, float m02, float m03,
			  		  float m10, float m11, float m12, float m13,
			  		  float m20, float m21, float m22, float m23,
			  		  float m30, float m31, float m32, float m33)
{
	_m[0] = m00;	_m[1] = m01; 	_m[2] = m02; 	_m[3] = m03;
	_m[4] = m10;	_m[5] = m11; 	_m[6] = m12; 	_m[7] = m13;
	_m[8] = m20;	_m[9] = m21; 	_m[10] = m22; 	_m[11] = m23;
	_m[12] = m30; 	_m[13] = m31; 	_m[14] = m32; 	_m[15] = m33;

	if (!_lock)
		changed()->execute(shared_from_this());
	_hasChanged = true;

	return shared_from_this();
}

Matrix4x4::Ptr
Matrix4x4::initialize(std::vector<float> m)
{
	return initialize(m[0], m[1], m[2], m[3],
					  m[4], m[5], m[6], m[7],
					  m[8], m[9], m[10], m[11],
					  m[12], m[13], m[14], m[15]);
}
					  
Matrix4x4::Ptr
Matrix4x4::initialize(Quaternion::Ptr rotation, Vector3::Ptr translation)
{
	return copyFrom(rotation->toMatrix())->appendTranslation(translation);
}

Matrix4x4::Ptr
Matrix4x4::identity()
{
	return initialize(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	);
}

float
Matrix4x4::determinant() const
{
    return _m[0] * _m[5] - _m[4] * _m[1] * _m[10] * _m[15] - _m[14] * _m[11]
    	- _m[0] * _m[6] - _m[4] * _m[2] * _m[9] * _m[15] - _m[13] * _m[11]
    	+ _m[0] * _m[7] - _m[4] * _m[3] * _m[9] * _m[14] - _m[13] * _m[10]
    	+ _m[1] * _m[6] - _m[5] * _m[2] * _m[8] * _m[15] - _m[12] * _m[11]
    	- _m[1] * _m[7] - _m[5] * _m[3] * _m[8] * _m[14] - _m[12] * _m[10]
    	+ _m[2] * _m[7] - _m[6] * _m[3] * _m[8] * _m[13] - _m[12] * _m[9];
}

float
Matrix4x4::determinant3x3() const
{
	return _m[0] * (_m[5]*_m[10] - _m[9]*_m[6])
		- _m[1] * (_m[4]*_m[10] - _m[8]*_m[6])
		+ _m[2] * (_m[4]*_m[9] - _m[8]*_m[5]);
}

Matrix4x4::Ptr
Matrix4x4::invert()
{
	float s0 =_m[0] * _m[5] - _m[4] * _m[1];
    float s1 =_m[0] * _m[6] - _m[4] * _m[2];
    float s2 =_m[0] * _m[7] - _m[4] * _m[3];
    float s3 =_m[1] * _m[6] - _m[5] * _m[2];
    float s4 =_m[1] * _m[7] - _m[5] * _m[3];
    float s5 =_m[2] * _m[7] - _m[6] * _m[3];

    float c5 =_m[10] * _m[15] - _m[14] * _m[11];
    float c4 =_m[9] * _m[15] - _m[13] * _m[11];
    float c3 =_m[9] * _m[14] - _m[13] * _m[10];
    float c2 =_m[8] * _m[15] - _m[12] * _m[11];
    float c1 =_m[8] * _m[14] - _m[12] * _m[10];
    float c0 =_m[8] * _m[13] - _m[12] * _m[9];

    float det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;

	if (det == 0.)
		throw std::logic_error("matrix is not invertible (determinant = 0).");

    float invdet = 1.f / det;

	return initialize(
		(_m[5] * c5 - _m[6] * c4 + _m[7] * c3) * invdet,
		(-_m[1] * c5 + _m[2] * c4 - _m[3] * c3) * invdet,
		(_m[13] * s5 - _m[14] * s4 + _m[15] * s3) * invdet,
		(-_m[9] * s5 + _m[10] * s4 - _m[11] * s3) * invdet,
		(-_m[4] * c5 + _m[6] * c2 - _m[7] * c1) * invdet,
		(_m[0] * c5 - _m[2] * c2 + _m[3] * c1) * invdet,
		(-_m[12] * s5 + _m[14] * s2 - _m[15] * s1) * invdet,
		(_m[8] * s5 - _m[10] * s2 + _m[11] * s1) * invdet,
		(_m[4] * c4 - _m[5] * c2 + _m[7] * c0) * invdet,
		(-_m[0] * c4 + _m[1] * c2 - _m[3] * c0) * invdet,
		(_m[12] * s4 - _m[13] * s2 + _m[15] * s0) * invdet,
		(-_m[8] * s4 + _m[9] * s2 - _m[11] * s0) * invdet,
		(-_m[4] * c3 + _m[5] * c1 - _m[6] * c0) * invdet,
		(_m[0] * c3 - _m[1] * c1 + _m[2] * c0) * invdet,
		(-_m[12] * s3 + _m[13] * s1 - _m[14] * s0) * invdet,
		(_m[8] * s3 - _m[9] * s1 + _m[10] * s0) * invdet
	);
}

Matrix4x4::Ptr
Matrix4x4::transpose()
{
	return initialize(
		_m[0],	_m[4], 	_m[8], 	_m[12],
		_m[1], 	_m[5], 	_m[9], 	_m[13],
		_m[2], 	_m[6], 	_m[10], _m[14],
		_m[3],	_m[7],	_m[11],	_m[15]
	);
}

std::shared_ptr<Vector3>
Matrix4x4::transform(std::shared_ptr<Vector3> v, std::shared_ptr<Vector3> output) const
{
    if (!output)
        output = Vector3::create();

	output->setTo(
        v->x() * _m[0] + v->y() * _m[1] + v->z() * _m[2] + _m[3],
        v->x() * _m[4] + v->y() * _m[5] + v->z() * _m[6] + _m[7],
        v->x() * _m[8] + v->y() * _m[9] + v->z() * _m[10] + _m[11]
    );

    return output;
}

std::shared_ptr<Vector3>
Matrix4x4::deltaTransform(std::shared_ptr<Vector3> v, std::shared_ptr<Vector3> output) const
{
    if (!output)
        output = Vector3::create();

	output->setTo(
        v->x() * _m[0] + v->y() * _m[1] + v->z() * _m[2],
        v->x() * _m[4] + v->y() * _m[5] + v->z() * _m[6],
        v->x() * _m[8] + v->y() * _m[9] + v->z() * _m[10]
    );

    return output;
}

Matrix4x4::Ptr
Matrix4x4::append(Matrix4x4::Ptr matrix)
{
	std::vector<float>& mv = matrix->_m;

	return append(
		mv[0],	mv[1], 	mv[2], 	mv[3],
		mv[4], 	mv[5], 	mv[6], 	mv[7],
		mv[8], 	mv[9], 	mv[10], mv[11],
		mv[12], mv[13], mv[14], mv[15]
	);
}

Matrix4x4::Ptr
Matrix4x4::prepend(Matrix4x4::Ptr matrix)
{
	std::vector<float>& mv = matrix->_m;

	return prepend(
		mv[0],	mv[1], 	mv[2], 	mv[3],
		mv[4], 	mv[5], 	mv[6], 	mv[7],
		mv[8], 	mv[9], 	mv[10], mv[11],
		mv[12], mv[13], mv[14], mv[15]
	);
}

Matrix4x4::Ptr
Matrix4x4::appendTranslation(float x, float y, float z)
{
	return append(
		1.f,	0.f,	0.f,    x,
		0.f,	1.f,	0.f,    y,
		0.f,	0.f,	1.f,    z,
		0.f,	0.f,	0.f,    1.f
	);
}

Matrix4x4::Ptr
Matrix4x4::prependTranslation(float x, float y, float z)
{
	return prepend(
		1.f,	0.f,	0.f,    x,
		0.f,	1.f,	0.f,    y,
		0.f,	0.f,	1.f,    z,
		0.f,	0.f,	0.f,    1.f
	);
}

Matrix4x4::Ptr
Matrix4x4::appendRotationX(float radians)
{
	return append(
		1.f,    0.f, 			0.f,			0.f,
		0.f,    cosf(radians),	-sinf(radians), 0.f,
		0.f,	sinf(radians),	cosf(radians),	0.f,
		0.f,	0.f,			0.f,			1.f
	);
}

Matrix4x4::Ptr
Matrix4x4::prependRotationX(float radians)
{
	return prepend(
		1.f,    0.f, 			0.f,			0.f,
		0.f,    cosf(radians),	-sinf(radians), 0.f,
		0.f,    sinf(radians),	cosf(radians),	0.f,
		0.f,	0.f,			0.f,			1.f
	);
}

Matrix4x4::Ptr
Matrix4x4::appendRotationY(float radians)
{
	return append(
		cosf(radians),	0.f,	sinf(radians),	0.f,
		0.f,			1.f,	0.f,			0.f,
		-sinf(radians),	0.f,	cosf(radians),	0.f,
		0.f,			0.f,	0.f,			1.f
	);
}

Matrix4x4::Ptr
Matrix4x4::prependRotationY(float radians)
{
	return prepend(
		cosf(radians),	0.f,	sinf(radians),	0.f,
		0.f,			1.f,	0.f,			0.f,
		-sinf(radians),	0.f,	cosf(radians),	0.f,
		0.f,			0.f,	0.f,			1.f
	);
}

Matrix4x4::Ptr
Matrix4x4::appendRotationZ(float radians)
{
	return append(
		cosf(radians),	-sinf(radians),	0.f,	0.f,
		sinf(radians),	cosf(radians),	0.f,	0.f,
		0.f,			0.f,			1.f,	0.f,
		0.f,			0.f,			0.f,	1.f
	);
}

Matrix4x4::Ptr
Matrix4x4::prependRotationZ(float radians)
{
	return prepend(
		cosf(radians),	-sinf(radians), 0.f,	0.f,
		sinf(radians),	cosf(radians),	0.f,	0.f,
		0.f,			0.f,			1.f,	0.f,
		0.f,			0.f,			0.f,	1.f
	);
}

Matrix4x4::Ptr
Matrix4x4::appendRotation(float radians, Vector3::Ptr axis)
{
	return append(Quaternion::create()->initialize(radians, axis));
}

Matrix4x4::Ptr
Matrix4x4::prependRotation(float radians, Vector3::Ptr axis)
{
	return prepend(Quaternion::create()->initialize(radians, axis));
}

Matrix4x4::Ptr
Matrix4x4::prepend(Quaternion::Ptr rotation)
{
	return prepend(rotation->toMatrix());
}

Matrix4x4::Ptr
Matrix4x4::append(Quaternion::Ptr rotation)
{
	return append(rotation->toMatrix());
}


Matrix4x4::Ptr
Matrix4x4::appendScale(float x, float y, float z)
{
	return append(
		x,		0.0f,	0.f,	0.f,
		0.f,	y,		0.f,	0.f,
		0.f,	0.f,	z,		0.f,
		0.f,	0.f,	0.f,	1.f
	);
}

Matrix4x4::Ptr
Matrix4x4::prependScale(float x, float y, float z)
{
	return prepend(
		x,		0.0f,	0.f,	0.f,
		0.f,	y,		0.f,	0.f,
		0.f,	0.f,	z,		0.f,
		0.f,	0.f,	0.f,	1.f
	);
}

Matrix4x4::Ptr
Matrix4x4::perspective(float fov, // vertical FOV
                       float ratio,
                       float zNear,
                       float zFar)
{
	const float invHalfFOV	= 1.0f / tanf(fov * .5f);
	const float	invZRange	= 1.0f / (zNear - zFar);

	/*
	// oculus rift's expected perspective transform
	return initialize(
		invHalfFOV / ratio,	0.f,		0.f,				0.f,
		0.f,				invHalfFOV,	0.f,				0.f,
		0.f,				0.f,		zFar * invZRange,	zNear * zFar * invZRange,
		0.f,				0.f,		-1.f,				0.f	
	);
	*/
	
	return initialize(
		invHalfFOV / ratio,	0.f,		0.f,						0.f,
		0.f,				invHalfFOV,	0.f,						0.f,
		0.f,				0.f,		(zFar + zNear) * invZRange,	2.f * zNear * zFar * invZRange,
		0.f,				0.f,		-1.f,						0.f
	);
	
}

Matrix4x4::Ptr
Matrix4x4::orthographic(float left, // vertical FOV
						float right,
						float top,
						float bottom,
						float zNear,
						float zFar)
{

	float	w = right - left;
	float	h = top - bottom;
	float	p = zFar - zNear;

	float	x = (right + left) / w;
	float	y = (top + bottom) / h;
	float	z = (zFar + zNear) / p;

	return initialize(
		2 / w, 0, 0, -x,
		0, 2 / h, 0, -y, 
		0, 0, -2 / p , -z, 
		0, 0, 0, 1
		);
}

Matrix4x4::Ptr
Matrix4x4::view(Vector3::Ptr eye, Vector3::Ptr lookAt, Vector3::Ptr upAxis)
{
    Vector3::Ptr	zAxis = eye - lookAt;

	zAxis->normalize();

	if (upAxis == 0)
	{
		if (zAxis->x() == 0. && zAxis->y() != 0. && zAxis->z() == 0.)
			upAxis = Vector3::xAxis();
		else
			upAxis = Vector3::yAxis();
	}

	Vector3::Ptr xAxis = Vector3::create()->copyFrom(upAxis)->cross(zAxis)->normalize();
	Vector3::Ptr yAxis = Vector3::create()->copyFrom(zAxis)->cross(xAxis)->normalize();

	if ((xAxis->x() == 0.f && xAxis->y() == 0.f && xAxis->z() == 0.f)
		|| (yAxis->x() == 0.f && yAxis->y() == 0.f && yAxis->z() == 0.f))
	{
		throw std::invalid_argument(
			"the eye direction (look at - eye position) and the up vector appear to be the same"
		);
	}

	float m41 = -(xAxis->dot(eye));
	float m42 = -(yAxis->dot(eye));
	float m43 = -(zAxis->dot(eye));
	
	return initialize(
		xAxis->x(),	xAxis->y(),	xAxis->z(),	m41,
		yAxis->x(),	yAxis->y(),	yAxis->z(), m42,
		zAxis->x(),	zAxis->y(),	zAxis->z(),	m43,
		0.f,		0.f,		0.f,		1.f
	);
}

Matrix4x4::Ptr
Matrix4x4::lookAt(Vector3::Ptr lookAt, Vector3::Ptr	position, Vector3::Ptr up)
{
	if (position == 0)
		position = Vector3::create(_m[3], _m[7], _m[11]);

	if (up == nullptr)
		up = Vector3::yAxis();

	return view(position, lookAt, up)->invert();
}

Matrix4x4::Ptr
Matrix4x4::lerp(Matrix4x4::Ptr target, float ratio)
{
	for (auto i = 0; i < 16; ++i)
		_m[i] = _m[i] + (target->_m[i] - _m[i]) * ratio;
	
	if (!_lock)
		changed()->execute(shared_from_this());
	_hasChanged = true;

	return shared_from_this();
}

Matrix4x4::Ptr
Matrix4x4::fromQuaternion(Quaternion::Ptr quaternion)
{
	return quaternion->toMatrix(shared_from_this());
}

Quaternion::Ptr
Matrix4x4::rotationQuaternion(Quaternion::Ptr output) const
{
	Quaternion::Ptr ret = output == nullptr
		? Quaternion::create()
		: output;

	return ret->fromMatrix(shared_from_this());
}

Matrix4x4::Ptr
Matrix4x4::copyFrom(Matrix4x4::Ptr source)
{
	std::copy(source->_m.begin(), source->_m.end(), _m.begin());

	if (!_lock)
		changed()->execute(shared_from_this());
	_hasChanged = true;

	return shared_from_this();
}

std::pair<Matrix4x4::Ptr, Matrix4x4::Ptr>
Matrix4x4::decomposeQR(Matrix4x4::Ptr matQ, Matrix4x4::Ptr matR) const
{	
	double vj[4]		= { 0.0, 0.0, 0.0, 0.0 };
	double accproj[4]	= { 0.0, 0.0, 0.0, 0.0 };
	
	auto projvec		= std::vector<double>(16, 0.0);
	auto valuesR		= std::vector<double>(16, 0.0);

	for (unsigned int j = 0; j < 4; ++j)
	{
		// m's jth column -> vj
		vj[0] = _m[j];
		vj[1] = _m[j + 4];
		vj[2] = _m[j + 8];
		vj[3] = _m[j + 12];

		accproj[0] = 0.0;
		accproj[1] = 0.0;
		accproj[2] = 0.0;
		accproj[3] = 0.0;

		for (unsigned int i = 0; i < j; ++i)
		{
			const double dot = 
				projvec[4*i]		* vj[0] + 
				projvec[4*i + 1]	* vj[1] + 
				projvec[4*i + 2]	* vj[2] + 
				projvec[4*i + 3]	* vj[3];

			for (unsigned int k = 0; k < 4; ++k)
				accproj[k] += dot * projvec[4*i + k];
		}

		double squaredLength = 0.0;
		for (unsigned int k = 0; k < 4; ++k)
		{
			const double diff	= vj[k] - accproj[k];

			projvec[4*j + k]	= diff;
			squaredLength		+= diff * diff;
		}

		const double invLength	= squaredLength > 1e-6 ? 1.0 / sqrt(squaredLength) : 0.0;
		for (unsigned int k = 0; k < 4; ++k)
			projvec[4*j + k] *= invLength;

		for (unsigned int i = 0; i <= j; ++i)
		{
			const double dot = 
				projvec[4*i]		* vj[0] + 
				projvec[4*i + 1]	* vj[1] + 
				projvec[4*i + 2]	* vj[2] + 
				projvec[4*i + 3]	* vj[3];

			valuesR[j + 4*i] = dot;
		}
	}

	if (matQ == nullptr)
		matQ = Matrix4x4::create();
	if (matR == nullptr)
		matR = Matrix4x4::create();

	matQ->initialize(
		(float)projvec[0],	(float)projvec[4],	(float)projvec[8],	(float)projvec[12],
		(float)projvec[1],	(float)projvec[5],	(float)projvec[9],	(float)projvec[13],
		(float)projvec[2],	(float)projvec[6],	(float)projvec[10], (float)projvec[14],
		(float)projvec[3],	(float)projvec[7],	(float)projvec[11], (float)projvec[15]
	);
	matR->initialize(
		(float)valuesR[0],	(float)valuesR[1],	(float)valuesR[2],	(float)valuesR[3],
		(float)valuesR[4],	(float)valuesR[5],	(float)valuesR[6],	(float)valuesR[7],
		(float)valuesR[8],	(float)valuesR[9],	(float)valuesR[10], (float)valuesR[11],
		(float)valuesR[12],	(float)valuesR[13],	(float)valuesR[14], (float)valuesR[15]
	);

	if (matQ->determinant3x3() < 0.0f)
	{
		// important: account for possible reflection in the Q orthogonal matrix
		matQ->data()[0]		*= -1.0f;
		matQ->data()[4]		*= -1.0f;
		matQ->data()[8]		*= -1.0f;
		matQ->data()[12]	*= -1.0f;

		matR->data()[0]		*= -1.0f;
		matR->data()[1]		*= -1.0f;
		matR->data()[2]		*= -1.0f;
		matR->data()[3]		*= -1.0f;
	}

	return std::make_pair(matQ, matR);
}

std::pair<Quaternion::Ptr, Matrix4x4::Ptr>
Matrix4x4::decomposeQR(Quaternion::Ptr quaternion, Matrix4x4::Ptr matR) const
{
	if (quaternion == nullptr)
		quaternion = Quaternion::create();
	if (matR == nullptr)
		matR = Matrix4x4::create();

	static auto matQ = Matrix4x4::create();

	decomposeQR(matQ, matR);
	quaternion->fromMatrix(matQ);

	return std::make_pair(quaternion, matR);
}

Matrix4x4::Ptr
Matrix4x4::lock()
{
	_lock = true;
	return shared_from_this();
}

Matrix4x4::Ptr
Matrix4x4::unlock()
{
	if (_hasChanged)
		changed()->execute(shared_from_this());
	_lock = false;

	return shared_from_this();
}

void
Matrix4x4::decompose(Vector3::Ptr		translation,
					 Quaternion::Ptr	rotation,
					 Vector3::Ptr		scaling) const
{
	static auto matrixR = Matrix4x4::create();

	decomposeQR(rotation, matrixR);
	
	const std::vector<float>& matR = matrixR->_m;
	scaling->setTo(matR[0], matR[5], matR[10]);

#ifdef DEBUG
	if (fabsf(matR[1]) > 1e-3f || fabsf(matR[2]) > 1e-3f || fabsf(matR[6]) > 1e-3f)
		std::cout << "Warning: Matrix decomposition assumes the following composition order : scaling, rotation, and translation." << std::endl;
#endif // DEBUG

	translation->setTo(_m[3], _m[7], _m[11]);
}

Matrix4x4::Ptr
Matrix4x4::recompose(Vector3Ptr		translation,
					 QuaternionPtr	rotation,
					 Vector3Ptr		scaling)
{
	static auto matrixRotation = Matrix4x4::create();

	matrixRotation->fromQuaternion(rotation);

	const std::vector<float>& matRot	= matrixRotation->_m;

	const float sx	= scaling->x();
	const float sy	= scaling->y();
	const float sz	= scaling->z();

	return initialize(
		sx * matRot[0],	sy * matRot[1],	sz * matRot[2],		translation->x(),
		sx * matRot[4],	sy * matRot[5],	sz * matRot[6],		translation->y(),
		sx * matRot[8],	sy * matRot[9],	sz * matRot[10],	translation->z(),
		0.0f,			0.0f,			0.0f,				1.0f
	);
}

Matrix4x4::Ptr
Matrix4x4::interpolateTo(Matrix4x4::Ptr	target, 
						 float			ratio)
{
	static auto quaternion1 = Quaternion::create();
	static auto quaternion2 = Quaternion::create();
	static auto matrixR1	= Matrix4x4::create();
	static auto matrixR2	= Matrix4x4::create();

	decomposeQR(quaternion1, matrixR1);
	target->decomposeQR(quaternion2, matrixR2);
	
	// handle the special case of rotation interpolation with quaternions
	quaternion1 = quaternion1->slerp(quaternion2, ratio);

	const float weight1 = 1.0f - ratio;
	const float weight2 = ratio;

	const auto&	mat1	= matrixR1->_m;
	const auto&	mat2	= matrixR2->_m;
	for (uint i = 0; i < 16; ++i)
		_m[i] = weight1 * mat1[i] + weight2 * mat2[i];

	return append(quaternion1); // interpQ * interpR
}