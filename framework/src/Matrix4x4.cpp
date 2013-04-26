#include "Matrix4x4.hpp"

Matrix4x4::ptr
Matrix4x4::create()
{
	auto m = std::shared_ptr<Matrix4x4>(new Matrix4x4());

	m->identity();

	return m;
}

Matrix4x4::ptr
Matrix4x4::create(Matrix4x4::ptr value)
{
	return std::shared_ptr<Matrix4x4>(new Matrix4x4(value));
}

Matrix4x4::ptr
Matrix4x4::create(float m00, float m01, float m02, float m03,
				  float m10, float m11, float m12, float m13,
				  float m20, float m21, float m22, float m23,
				  float m30, float m31, float m32, float m33)
{
	return std::shared_ptr<Matrix4x4>(new Matrix4x4(
		m00, m01, m02, m03,
		m10, m11, m12, m13,
		m20, m21, m22, m23,
		m30, m31, m32, m33
	));
}

Matrix4x4::Matrix4x4() :
	std::enable_shared_from_this<Matrix4x4>(),
	_m(16)
{
}

Matrix4x4::Matrix4x4(Matrix4x4::ptr value) :
	std::enable_shared_from_this<Matrix4x4>(),
	_m(value->_m)
{
}

Matrix4x4::Matrix4x4(float m00, float m01, float m02, float m03,
			  		 float m10, float m11, float m12, float m13,
			  		 float m20, float m21, float m22, float m23,
			  		 float m30, float m31, float m32, float m33) :
	std::enable_shared_from_this<Matrix4x4>(),
	_m(16)
{
	_m[0] = m00;	_m[1] = m01; 	_m[2] = m02; 	_m[3] = m03;
	_m[4] = m10;	_m[5] = m11; 	_m[6] = m12; 	_m[7] = m13;
	_m[8] = m20;	_m[9] = m21; 	_m[10] = m22; 	_m[11] = m23;
	_m[12] = m30; 	_m[13] = m31; 	_m[14] = m32; 	_m[15] = m33;	
}

Matrix4x4::ptr
Matrix4x4::identity()
{
	_m[0] = 1.;		_m[1] = 0.; 	_m[2] = 0.; 	_m[3] = 0.;
	_m[4] = 0.;		_m[5] = 1.; 	_m[6] = 0.; 	_m[7] = 0.;
	_m[8] = 0.;		_m[9] = 0.; 	_m[10] = 1.; 	_m[11] = 0.;
	_m[12] = 0.; 	_m[13] = 0.; 	_m[14] = 0.; 	_m[15] = 1.;

	return shared_from_this();
}

float
Matrix4x4::determinant()
{
    return _m[0] *_m[5] -_m[4] *_m[1] * _m[10] *_m[15] -_m[14] *_m[11]
    	- _m[0] *_m[6] -_m[4] *_m[2] * _m[9] *_m[15] -_m[13] -_m[11]
    	+ _m[0] *_m[7] -_m[4] *_m[3] * _m[9] *_m[14] -_m[13] *_m[10]
    	+ _m[1] *_m[6] -_m[5] *_m[2] * _m[8] *_m[15] -_m[12] *_m[11]
    	- _m[1] *_m[7] -_m[5] *_m[3] * _m[8] *_m[14] -_m[12] *_m[10]
    	+ _m[2] *_m[7] -_m[6] *_m[3] * _m[8] *_m[13] -_m[12] *_m[9];
}

Matrix4x4::ptr
Matrix4x4::invert()
{
	float s0 =_m[0] *_m[5] -_m[4] *_m[1];
    float s1 =_m[0] *_m[6] -_m[4] *_m[2];
    float s2 =_m[0] *_m[7] -_m[4] *_m[3];
    float s3 =_m[1] *_m[6] -_m[5] *_m[2];
    float s4 =_m[1] *_m[7] -_m[5] *_m[3];
    float s5 =_m[2] *_m[7] -_m[6] *_m[3];

    float c5 =_m[10] *_m[15] -_m[14] *_m[11];
    float c4 =_m[9] *_m[15] -_m[13] -_m[11];
    float c3 =_m[9] *_m[14] -_m[13] *_m[10];
    float c2 =_m[8] *_m[15] -_m[12] *_m[11];
    float c1 =_m[8] *_m[14] -_m[12] *_m[10];
    float c0 =_m[8] *_m[13] -_m[12] *_m[9];

    float det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;

	if (det == 0.)
		throw;

    float invdet = 1. / det;

    _m[0] = (_m[5] * c5 - _m[6] * c4 + _m[7] * c3) * invdet;
    _m[1] = (-_m[1] * c5 + _m[2] * c4 - _m[3] * c3) * invdet;
    _m[2] = (_m[13] * s5 - _m[14] * s4 + _m[15] * s3) * invdet;
    _m[3] = (-_m[9] * s5 + _m[10] * s4 - _m[11] * s3) * invdet;

    _m[4] = (-_m[4] * c5 + _m[6] * c2 - _m[7] * c1) * invdet;
    _m[5] = (_m[0] * c5 - _m[2] * c2 + _m[3] * c1) * invdet;
    _m[6] = (-_m[12] * s5 + _m[14] * s2 - _m[15] * s1) * invdet;
    _m[7] = (_m[8] * s5 - _m[10] * s2 + _m[11] * s1) * invdet;

    _m[8] = (_m[4] * c4 - _m[5] * c2 + _m[7] * c0) * invdet;
    _m[9] = (-_m[0] * c4 + _m[1] * c2 - _m[3] * c0) * invdet;
    _m[10] = (_m[12] * s4 - _m[13] * s2 + _m[15] * s0) * invdet;
    _m[11] = (-_m[8] * s4 + _m[9] * s2 - _m[11] * s0) * invdet;

    _m[12] = (-_m[4] * c3 + _m[5] * c1 - _m[6] * c0) * invdet;
    _m[13] = (_m[0] * c3 - _m[1] * c1 + _m[2] * c0) * invdet;
    _m[14] = (-_m[12] * s3 + _m[13] * s1 - _m[14] * s0) * invdet;
    _m[15] = (_m[8] * s3 - _m[9] * s1 + _m[10] * s0) * invdet;

    return shared_from_this();
}

Matrix4x4::ptr
Matrix4x4::append(Matrix4x4::ptr matrix)
{
	_m[0] = _m[0] * matrix->_m[0] + _m[1] * matrix->_m[4] + _m[2] * matrix->_m[8] + _m[3] * matrix->_m[12];
	_m[1] = _m[0] * matrix->_m[1] + _m[1] * matrix->_m[5] + _m[2] * matrix->_m[9] + _m[3] * matrix->_m[13];
	_m[2] = _m[0] * matrix->_m[2] + _m[1] * matrix->_m[6] + _m[2] * matrix->_m[10] + _m[3] * matrix->_m[14];
	_m[3] = _m[0] * matrix->_m[3] + _m[1] * matrix->_m[7] + _m[2] * matrix->_m[11] + _m[3] * matrix->_m[15];

	_m[4] = _m[4] * matrix->_m[0] + _m[5] * matrix->_m[4] + _m[6] * matrix->_m[8] + _m[7] * matrix->_m[12];
	_m[5] = _m[4] * matrix->_m[1] + _m[5] * matrix->_m[5] + _m[6] * matrix->_m[9] + _m[7] * matrix->_m[13];
	_m[6] = _m[4] * matrix->_m[2] + _m[5] * matrix->_m[6] + _m[6] * matrix->_m[10] + _m[7] * matrix->_m[14];
	_m[7] = _m[4] * matrix->_m[3] + _m[5] * matrix->_m[7] + _m[6] * matrix->_m[11] + _m[7] * matrix->_m[15];	 

	_m[8] = _m[8] * matrix->_m[0] + _m[9] * matrix->_m[4] + _m[10] * matrix->_m[8] + _m[11] * matrix->_m[12];
	_m[9] = _m[8] * matrix->_m[1] + _m[9] * matrix->_m[5] + _m[10] * matrix->_m[9] + _m[11] * matrix->_m[13];
	_m[10] = _m[8] * matrix->_m[2] + _m[9] * matrix->_m[6] + _m[10] * matrix->_m[10] + _m[11] * matrix->_m[14];
	_m[11] = _m[8] * matrix->_m[3] + _m[9] * matrix->_m[7] + _m[10] * matrix->_m[11] + _m[11] * matrix->_m[15];

	_m[12] = _m[12] * matrix->_m[0] + _m[13] * matrix->_m[4] + _m[14] * matrix->_m[8] + _m[15] * matrix->_m[12];
	_m[13] = _m[12] * matrix->_m[1] + _m[13] * matrix->_m[5] + _m[14] * matrix->_m[9] + _m[15] * matrix->_m[13];
	_m[14] = _m[12] * matrix->_m[2] + _m[13] * matrix->_m[6] + _m[14] * matrix->_m[10] + _m[15] * matrix->_m[14];
	_m[15] = _m[12] * matrix->_m[3] + _m[13] * matrix->_m[7] + _m[14] * matrix->_m[11] + _m[15] * matrix->_m[15];

	return shared_from_this();
}

Matrix4x4::ptr
Matrix4x4::prepend(Matrix4x4::ptr matrix)
{
	_m[0] = matrix->_m[0] * _m[0] + matrix->_m[1] * _m[4] + matrix->_m[2] * _m[8] + matrix->_m[3] * _m[12];
	_m[1] = matrix->_m[0] * _m[1] + matrix->_m[1] * _m[5] + matrix->_m[2] * _m[9] + matrix->_m[3] * _m[13];
	_m[2] = matrix->_m[0] * _m[2] + matrix->_m[1] * _m[6] + matrix->_m[2] * _m[10] + matrix->_m[3] * _m[14];
	_m[3] = matrix->_m[0] * _m[3] + matrix->_m[1] * _m[7] + matrix->_m[2] * _m[11] + matrix->_m[3] * _m[15];

	_m[4] = matrix->_m[4] * _m[0] + matrix->_m[5] * _m[4] + matrix->_m[6] * _m[8] + matrix->_m[7] * _m[12];
	_m[5] = matrix->_m[4] * _m[1] + matrix->_m[5] * _m[5] + matrix->_m[6] * _m[9] + matrix->_m[7] * _m[13];
	_m[6] = matrix->_m[4] * _m[2] + matrix->_m[5] * _m[6] + matrix->_m[6] * _m[10] + matrix->_m[7] * _m[14];
	_m[7] = matrix->_m[4] * _m[3] + matrix->_m[5] * _m[7] + matrix->_m[6] * _m[11] + matrix->_m[7] * _m[15];	 

	_m[8] = matrix->_m[8] * _m[0] + matrix->_m[9] * _m[4] + matrix->_m[10] * _m[8] + matrix->_m[11] * _m[12];
	_m[9] = matrix->_m[8] * _m[1] + matrix->_m[9] * _m[5] + matrix->_m[10] * _m[9] + matrix->_m[11] * _m[13];
	_m[10] = matrix->_m[8] * _m[2] + matrix->_m[9] * _m[6] + matrix->_m[10] * _m[10] + matrix->_m[11] * _m[14];
	_m[11] = matrix->_m[8] * _m[3] + matrix->_m[9] * _m[7] + matrix->_m[10] * _m[11] + matrix->_m[11] * _m[15];

	_m[12] = matrix->_m[12] * _m[0] + matrix->_m[13] * _m[4] + matrix->_m[14] * _m[8] + matrix->_m[15] * _m[12];
	_m[13] = matrix->_m[12] * _m[1] + matrix->_m[13] * _m[5] + matrix->_m[14] * _m[9] + matrix->_m[15] * _m[13];
	_m[14] = matrix->_m[12] * _m[2] + matrix->_m[13] * _m[6] + matrix->_m[14] * _m[10] + matrix->_m[15] * _m[14];
	_m[15] = matrix->_m[12] * _m[3] + matrix->_m[13] * _m[7] + matrix->_m[14] * _m[11] + matrix->_m[15] * _m[15];

	return shared_from_this();
}

Matrix4x4
Matrix4x4::operator*(Matrix4x4& value)
{
	Matrix4x4::ptr m1 = Matrix4x4::create(shared_from_this());
	Matrix4x4::ptr m2 = value.shared_from_this();

	m1->append(m2);

	return *m1;
}