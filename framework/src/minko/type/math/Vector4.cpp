#include "Vector4.hpp"

Vector4::ptr
Vector4::create(float x, float y, float z, float w)
{
	return std::shared_ptr<Vector4>(new Vector4(x, y, z, w));
}

Vector4::Vector4(float x, float y, float z, float w) :
	Vector3(x, y, z),
	_w(w)
{
}