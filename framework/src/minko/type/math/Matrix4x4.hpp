#pragma once

#include <sstream>

#include "minko/Common.hpp"
#include "Vector3.hpp"

class Matrix4x4 :
	public std::enable_shared_from_this<Matrix4x4>
{
private:
	std::vector<float> _m;

public:
	typedef std::shared_ptr<Matrix4x4>	ptr;

	inline static
	ptr
	create()
	{
		auto m = std::shared_ptr<Matrix4x4>(new Matrix4x4());

		m->identity();

		return m;
	}

	inline static
	ptr
	create(ptr value)
	{
		return std::shared_ptr<Matrix4x4>(new Matrix4x4(value));
	}

	ptr
	initialize(float m00, float m01, float m02, float m03,
		   	   float m10, float m11, float m12, float m13,
		       float m20, float m21, float m22, float m23,
		       float m30, float m31, float m32, float m33);

	ptr
	translation(float x, float y, float z);

	ptr
	rotationX(float radians);

	ptr
	rotationY(float radians);
	
	ptr
	rotationZ(float radians);

	inline
	const std::vector<float>
	values()
	{
		return _m;
	}

	ptr
	identity();

	ptr
	append(ptr matrix);

	ptr
	prepend(ptr matrix);

	float
	determinant();

	ptr
	invert();

	ptr
	perspectiveFoV(float fov,
                   float ratio,
                   float zNear,
                   float zFar);

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
	ptr
	view(Vector3::const_ptr 	eye,
         Vector3::const_ptr 	lookAt,
         Vector3::const_ptr 	up	= 0);

private:
	Matrix4x4();

	Matrix4x4(ptr value);

	Matrix4x4(float m00, float m01, float m02, float m03,
			  float m10, float m11, float m12, float m13,
			  float m20, float m21, float m22, float m23,
			  float m30, float m31, float m32, float m33);
};

/*bool
operator==(Matrix4x4& a, Matrix4x4& b)
{
	for (auto i = 0; i < 16; ++i)
		if (a.values()[i] != b.values()[i])
			return false;

	return true;
}*/

inline
Matrix4x4::ptr
operator*(Matrix4x4::ptr a, Matrix4x4::ptr b)
{
	return Matrix4x4::create(a)->append(b);
}

inline
Matrix4x4::ptr
operator*=(Matrix4x4::ptr a, Matrix4x4::ptr b)
{
	return a->append(b);
}

namespace std
{
	template<typename Matrix4x4>
	string
	to_string(Matrix4x4 value)
	{
		const std::vector<float> values = value.values();
		std::stringstream stream;

		stream << "(" << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << ", "
			<< values[4] << ", " << values[5] << ", " << values[6] << ", " << values[7] << ", "
			<< values[8] << ", " << values[9] << ", " << values[10] << ", " << values[11] << ", "
			<< values[12] << ", " << values[13] << ", " << values[14] << ", " << values[15] << ")";

		return stream.str();
	}
}
