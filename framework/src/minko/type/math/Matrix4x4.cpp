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

Matrix4x4
Matrix4x4::operator*(Matrix4x4& value)
{
	Matrix4x4::ptr m1 = Matrix4x4::create(shared_from_this());
	Matrix4x4::ptr m2 = value.shared_from_this();

	m1->append(m2);

	return *m1;
}

bool
Matrix4x4::operator==(Matrix4x4& value)
{
	std::vector<float> m = value._m;

	for (auto i = 0; i < 16; ++i)
		if (_m[i] != m[i])
			return false;

	return true;
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
 * Eye : eye position, At : eye direction, Up : up vector
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
Matrix4x4::view(Vector4::ptr 	eye,
         		Vector4::ptr 	lookAt,
         		Vector4::ptr 	up)
{
/*	Vector4::ptr	direction = Vector4::subtract(lookAt, eye, TMP_VECTOR4);
	
	float eye_X		: Number = eye._vector.x;
	float eye_Y		: Number = eye._vector.y;
	float eye_Z		: Number = eye._vector.z;
	
	float z_axis_X	: Number = direction._vector.x;
	float z_axis_Y	: Number = direction._vector.y;
	float z_axis_Z	: Number = direction._vector.z;
	
	float up_axis_x	: Number;
	float up_axis_y	: Number;
	float up_axis_z	: Number;
	
	if (up != null)
	{
		// if up axis was given, take it. An error will be raised later if it is colinear to direction
		up_axis_x = up._vector.x;
		up_axis_y = up._vector.y;
		up_axis_z = up._vector.z;	
	}
	else
	{
		// if direction is colinear to (0, 1, 0), take (1, 0, 0) as up vector
		if (z_axis_X == 0 && z_axis_Y != 0 && z_axis_Z == 0)
		{
			up_axis_x = 1;
			up_axis_y = 0;
			up_axis_z = 0;
		}
			// else, take (0, 0, 1)
		else
		{
			up_axis_x = 0;
			up_axis_y = 1;
			up_axis_z = 0;
		}
	}
	
	var l : Number;
	
	l = 1 / Math.sqrt(z_axis_X * z_axis_X + z_axis_Y * z_axis_Y + z_axis_Z * z_axis_Z);
	
	z_axis_X *= l;
	z_axis_Y *= l;
	z_axis_Z *= l;
	
	var x_axis_X : Number = up_axis_y * z_axis_Z - z_axis_Y * up_axis_z;
	var x_axis_Y : Number = up_axis_z * z_axis_X - z_axis_Z * up_axis_x;
	var x_axis_Z : Number = up_axis_x * z_axis_Y - z_axis_X * up_axis_y;
	
	l = 1 / Math.sqrt(x_axis_X * x_axis_X + x_axis_Y * x_axis_Y + x_axis_Z * x_axis_Z);
	
	x_axis_X *= l;
	x_axis_Y *= l;
	x_axis_Z *= l;
	
	var y_axis_X : Number = z_axis_Y * x_axis_Z - x_axis_Y * z_axis_Z;
	var y_axis_Y : Number = z_axis_Z * x_axis_X - x_axis_Z * z_axis_X;
	var y_axis_Z : Number = z_axis_X * x_axis_Y - x_axis_X * z_axis_Y;
	
	l = 1 / Math.sqrt(y_axis_X * y_axis_X + y_axis_Y * y_axis_Y + y_axis_Z * y_axis_Z);
	
	y_axis_X *= l;
	y_axis_Y *= l;
	y_axis_Z *= l;
	
	if ((x_axis_X == 0 && x_axis_Y == 0 && x_axis_Z == 0) 
		|| (y_axis_X == 0 && y_axis_Y == 0 && y_axis_Z == 0))
	{
		throw new Error(
			'Invalid argument(s): the eye direction (look at - eye position) '
			+ 'and the up vector appear to be the same.'
		);
	}
	
	var	m41	: Number	= -(x_axis_X * eye_X + x_axis_Y * eye_Y + x_axis_Z * eye_Z);
	var	m42	: Number	= -(y_axis_X * eye_X + y_axis_Y * eye_Y + y_axis_Z * eye_Z);
	var	m43	: Number	= -(z_axis_X * eye_X + z_axis_Y * eye_Y + z_axis_Z * eye_Z);
	
	return initialize(
		x_axis_X,	y_axis_X,	z_axis_X,	0.,
		x_axis_Y,	y_axis_Y,	z_axis_Y,	0.,
		x_axis_Z,	y_axis_Z,	z_axis_Z,	0.,
		m41,		m42,		m43,		1.
	);*/

	return shared_from_this();
}
