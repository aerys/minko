#include "Matrix4x4.hpp"

Matrix4x4::ptr
Matrix4x4::translation(float x, float y, float z)
{
	initialize(
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1
	);

	return shared_from_this();
}

Matrix4x4::ptr
Matrix4x4::rotationX(float radians)
{
	initialize(
		1, 	0, 				0, 				0,
		0, 	cosf(radians),	-sinf(radians), 0,
		0,	sinf(radians),	cosf(radians),	0,
		0,	0,				0,				1
	);

	return shared_from_this();
}

Matrix4x4::ptr
Matrix4x4::rotationY(float radians)
{
	initialize(
		cosf(radians),	0,	sinf(radians),	0,
		0,				1,	0,				0,
		-sinf(radians),	0,	cosf(radians),	0,
		0,				0,	0,				1
	);

	return shared_from_this();
}

Matrix4x4::ptr
Matrix4x4::rotationZ(float radians)
{
	initialize(
		cosf(radians),	-sinf(radians),	0,	0,
		sinf(radians),	cosf(radians),	0,	0,
		0,				0,				1,	0,
		0,				0,				0,	1
	);

	return shared_from_this();
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

Matrix4x4::ptr
Matrix4x4::initialize(float m00, float m01, float m02, float m03,
			  		  float m10, float m11, float m12, float m13,
			  		  float m20, float m21, float m22, float m23,
			  		  float m30, float m31, float m32, float m33)
{
	_m[0] = m00;	_m[1] = m01; 	_m[2] = m02; 	_m[3] = m03;
	_m[4] = m10;	_m[5] = m11; 	_m[6] = m12; 	_m[7] = m13;
	_m[8] = m20;	_m[9] = m21; 	_m[10] = m22; 	_m[11] = m23;
	_m[12] = m30; 	_m[13] = m31; 	_m[14] = m32; 	_m[15] = m33;

	return shared_from_this();
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
    return _m[0] * _m[5] - _m[4] * _m[1] * _m[10] * _m[15] - _m[14] * _m[11]
    	- _m[0] * _m[6] - _m[4] * _m[2] * _m[9] * _m[15] - _m[13] * _m[11]
    	+ _m[0] * _m[7] - _m[4] * _m[3] * _m[9] * _m[14] - _m[13] * _m[10]
    	+ _m[1] * _m[6] - _m[5] * _m[2] * _m[8] * _m[15] - _m[12] * _m[11]
    	- _m[1] * _m[7] - _m[5] * _m[3] * _m[8] * _m[14] - _m[12] * _m[10]
    	+ _m[2] * _m[7] - _m[6] * _m[3] * _m[8] * _m[13] - _m[12] * _m[9];
}

Matrix4x4::ptr
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
		throw;

    float invdet = 1. / det;

	float m0 = (_m[5] * c5 - _m[6] * c4 + _m[7] * c3) * invdet;
	float m1 = (-_m[1] * c5 + _m[2] * c4 - _m[3] * c3) * invdet;
	float m2 = (_m[13] * s5 - _m[14] * s4 + _m[15] * s3) * invdet;
	float m3 = (-_m[9] * s5 + _m[10] * s4 - _m[11] * s3) * invdet;

	float m4 = (-_m[4] * c5 + _m[6] * c2 - _m[7] * c1) * invdet;
	float m5 = (_m[0] * c5 - _m[2] * c2 + _m[3] * c1) * invdet;
	float m6 = (-_m[12] * s5 + _m[14] * s2 - _m[15] * s1) * invdet;
	float m7 = (_m[8] * s5 - _m[10] * s2 + _m[11] * s1) * invdet;

	float m8 = (_m[4] * c4 - _m[5] * c2 + _m[7] * c0) * invdet;
	float m9 = (-_m[0] * c4 + _m[1] * c2 - _m[3] * c0) * invdet;
	float m10 = (_m[12] * s4 - _m[13] * s2 + _m[15] * s0) * invdet;
	float m11 = (-_m[8] * s4 + _m[9] * s2 - _m[11] * s0) * invdet;

	float m12 = (-_m[4] * c3 + _m[5] * c1 - _m[6] * c0) * invdet;
	float m13 = (_m[0] * c3 - _m[1] * c1 + _m[2] * c0) * invdet;
	float m14 = (-_m[12] * s3 + _m[13] * s1 - _m[14] * s0) * invdet;
	float m15 = (_m[8] * s3 - _m[9] * s1 + _m[10] * s0) * invdet;

    _m[0] = m0;
    _m[1] = m1;
    _m[2] = m2;
    _m[3] = m3;

    _m[4] = m4;
    _m[5] = m5;
    _m[6] = m6;
    _m[7] = m7;

    _m[8] = m8;
    _m[9] = m9;
    _m[10] = m10;
    _m[11] = m11;

    _m[12] = m12;
    _m[13] = m13;
    _m[14] = m14;
    _m[15] = m15;

    return shared_from_this();
}

Matrix4x4::ptr
Matrix4x4::append(Matrix4x4::ptr matrix)
{
	Matrix4x4 m = *matrix;

	float m0 = _m[0] * m._m[0] + _m[1] * m._m[4] + _m[2] * m._m[8] + _m[3] * m._m[12];
	float m1 = _m[0] * m._m[1] + _m[1] * m._m[5] + _m[2] * m._m[9] + _m[3] * m._m[13];
	float m2 = _m[0] * m._m[2] + _m[1] * m._m[6] + _m[2] * m._m[10] + _m[3] * m._m[14];
	float m3 = _m[0] * m._m[3] + _m[1] * m._m[7] + _m[2] * m._m[11] + _m[3] * m._m[15];
	float m4 = _m[4] * m._m[0] + _m[5] * m._m[4] + _m[6] * m._m[8] + _m[7] * m._m[12];
	float m5 = _m[4] * m._m[1] + _m[5] * m._m[5] + _m[6] * m._m[9] + _m[7] * m._m[13];
	float m6 = _m[4] * m._m[2] + _m[5] * m._m[6] + _m[6] * m._m[10] + _m[7] * m._m[14];
	float m7 = _m[4] * m._m[3] + _m[5] * m._m[7] + _m[6] * m._m[11] + _m[7] * m._m[15];
	float m8 = _m[8] * m._m[0] + _m[9] * m._m[4] + _m[10] * m._m[8] + _m[11] * m._m[12];
	float m9 = _m[8] * m._m[1] + _m[9] * m._m[5] + _m[10] * m._m[9] + _m[11] * m._m[13];
	float m10 = _m[8] * m._m[2] + _m[9] * m._m[6] + _m[10] * m._m[10] + _m[11] * m._m[14];
	float m11 = _m[8] * m._m[3] + _m[9] * m._m[7] + _m[10] * m._m[11] + _m[11] * m._m[15];
	float m12 = _m[12] * m._m[0] + _m[13] * m._m[4] + _m[14] * m._m[8] + _m[15] * m._m[12];
	float m13 = _m[12] * m._m[1] + _m[13] * m._m[5] + _m[14] * m._m[9] + _m[15] * m._m[13];
	float m14 = _m[12] * m._m[2] + _m[13] * m._m[6] + _m[14] * m._m[10] + _m[15] * m._m[14];
	float m15 = _m[12] * m._m[3] + _m[13] * m._m[7] + _m[14] * m._m[11] + _m[15] * m._m[15];

	_m[0] = m0;
	_m[1] = m1;
	_m[2] = m2;
	_m[3] = m3;

	_m[4] = m4;
	_m[5] = m5;
	_m[6] = m6;
	_m[7] = m7;

	_m[8] = m8;
	_m[9] = m9;
	_m[10] = m10;
	_m[11] = m11;

	_m[12] = m12;
	_m[13] = m13;
	_m[14] = m14;
	_m[15] = m15;

	return shared_from_this();
}

Matrix4x4::ptr
Matrix4x4::prepend(Matrix4x4::ptr matrix)
{
	Matrix4x4 m = *matrix;

	float m0 = m._m[0] * _m[0] + m._m[1] * _m[4] + m._m[2] * _m[8] + m._m[3] * _m[12];
	float m1 = m._m[0] * _m[1] + m._m[1] * _m[5] + m._m[2] * _m[9] + m._m[3] * _m[13];
	float m2 = m._m[0] * _m[2] + m._m[1] * _m[6] + m._m[2] * _m[10] + m._m[3] * _m[14];
	float m3 = m._m[0] * _m[3] + m._m[1] * _m[7] + m._m[2] * _m[11] + m._m[3] * _m[15];

	float m4 = m._m[4] * _m[0] + m._m[5] * _m[4] + m._m[6] * _m[8] + m._m[7] * _m[12];
	float m5 = m._m[4] * _m[1] + m._m[5] * _m[5] + m._m[6] * _m[9] + m._m[7] * _m[13];
	float m6 = m._m[4] * _m[2] + m._m[5] * _m[6] + m._m[6] * _m[10] + m._m[7] * _m[14];
	float m7 = m._m[4] * _m[3] + m._m[5] * _m[7] + m._m[6] * _m[11] + m._m[7] * _m[15];

	float m8 = m._m[8] * _m[0] + m._m[9] * _m[4] + m._m[10] * _m[8] + m._m[11] * _m[12];
	float m9 = m._m[8] * _m[1] + m._m[9] * _m[5] + m._m[10] * _m[9] + m._m[11] * _m[13];
	float m10 = m._m[8] * _m[2] + m._m[9] * _m[6] + m._m[10] * _m[10] + m._m[11] * _m[14];
	float m11 = m._m[8] * _m[3] + m._m[9] * _m[7] + m._m[10] * _m[11] + m._m[11] * _m[15];

	float m12 = m._m[12] * _m[0] + m._m[13] * _m[4] + m._m[14] * _m[8] + m._m[15] * _m[12];
	float m13 = m._m[12] * _m[1] + m._m[13] * _m[5] + m._m[14] * _m[9] + m._m[15] * _m[13];
	float m14 = m._m[12] * _m[2] + m._m[13] * _m[6] + m._m[14] * _m[10] + m._m[15] * _m[14];
	float m15 = m._m[12] * _m[3] + m._m[13] * _m[7] + m._m[14] * _m[11] + m._m[15] * _m[15];

	_m[0] = m0;
	_m[1] = m1;
	_m[2] = m2;
	_m[3] = m3;

	_m[4] = m4;
	_m[5] = m5;
	_m[6] = m6;
	_m[7] = m7;

	_m[8] = m8;
	_m[9] = m9;
	_m[10] = m10;
	_m[11] = m11;

	_m[12] = m12;
	_m[13] = m13;
	_m[14] = m14;
	_m[15] = m15;

	return shared_from_this();
}

Matrix4x4::ptr
Matrix4x4::perspectiveFoV(float fov,
                          float ratio,
                          float zNear,
                          float zFar)
{
	float fd = 1. / tanf(fov * 0.5);

	return initialize(
		fd / ratio,	0.,								0.,		0.,
		0.,			fd,								0.,		0.,
		0.,			0.,			 zFar / (zFar - zNear),		1.,
		0.,			0.,	-zNear * zFar / (zFar - zNear),		0.
	);
}

/**
 * Builds a (left-handed) view transform.
 * <br /><br />
 * Eye : eye position, At : eye zAxis, Up : up vector
 * <br /><br />
 * zaxis = normal(At - Eye)<br />
 * xaxis = normal(cross(Up, zaxis))<br />
 * yaxis = cross(zaxis, xaxis)<br />
 * <br />
 * [      xaxis.x          yaxis.x            zaxis.x  	     0 ]<br />
 * [      xaxis.y          yaxis.y            zaxis.y        0 ]<br />
 * [      xaxis.z          yaxis.z            zaxis.z        0 ]<br />
 * [ -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)    1 ]<br />
 *
 * @return Returns a left-handed view Matrix3D to convert world coordinates into eye coordinates
 *
 */
Matrix4x4::ptr
Matrix4x4::view(Vector3::const_ptr 	eye,
         		Vector3::const_ptr 	lookAt,
         		Vector3::const_ptr 	upAxis)
{
	Vector3::ptr	zAxis = lookAt - eye;

	zAxis->normalize();

	if (upAxis == 0)
	{
		if (zAxis->x() == 0. && zAxis->y() != 0. && zAxis->z() == 0.)
			upAxis = Vector3::xAxis();
		else
			upAxis = Vector3::yAxis();
	}

	Vector3::ptr xAxis = Vector3::create(upAxis)->cross(zAxis)->normalize();
	Vector3::ptr yAxis = Vector3::create(zAxis)->cross(xAxis)->normalize();

	if ((xAxis->x() == 0. && xAxis->y() == 0. && xAxis->z() == 0.)
		|| (yAxis->x() == 0. && yAxis->y() == 0. && yAxis->z() == 0.))
	{
		throw std::invalid_argument(
			"the eye direction (look at - eye position) and the up vector appear to be the same"
		);
	}

	float m41 = -xAxis->dot(eye);
	float m42 = -yAxis->dot(eye);
	float m43 = -zAxis->dot(eye);

	return initialize(
		xAxis->x(),	yAxis->x(),	zAxis->x(),	0.,
		xAxis->y(),	yAxis->y(),	zAxis->y(),	0.,
		xAxis->z(),	yAxis->z(),	zAxis->z(),	0.,
		m41,		m42,		m43,		1.
	);

	return shared_from_this();
}
