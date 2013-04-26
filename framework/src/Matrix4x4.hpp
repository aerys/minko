#pragma once

#include <memory>
#include <string>
#include <vector>
#include <sstream>

class Matrix4x4 :
	public std::enable_shared_from_this<Matrix4x4>
{
private:
	std::vector<float> _m;

public:
	typedef std::shared_ptr<Matrix4x4>	ptr;

	static
	ptr
	create();

	static
	ptr
	create(ptr value);

	static
	ptr
	create(float m00, float m01, float m02, float m03,
		   float m10, float m11, float m12, float m13,
		   float m20, float m21, float m22, float m23,
		   float m30, float m31, float m32, float m33);

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

	Matrix4x4
	operator*(Matrix4x4& value);

private:
	Matrix4x4();

	Matrix4x4(ptr value);

	Matrix4x4(float m00, float m01, float m02, float m03,
			  float m10, float m11, float m12, float m13,
			  float m20, float m21, float m22, float m23,
			  float m30, float m31, float m32, float m33);
};

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
