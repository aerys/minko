#include "Vector3.hpp"

Vector3::ptr
Vector3::create(float x, float y, float z)
{
	return std::shared_ptr<Vector3>(new Vector3(x, y, z));
}

Vector3::Vector3(float x, float y, float z) :
	Vector2(x, y),
	_z(z)
{
}