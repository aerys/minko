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

Vector4::ptr
Vector4::incrementBy(ptr value)
{
	_x += value->_x;
	_y += value->_y;
	_z += value->_z;
	_w += value->_w;

	return std::static_pointer_cast<Vector4>(shared_from_this());
}

Vector4::ptr
Vector4::decrementBy(ptr value)
{
	_x -= value->_x;
	_y -= value->_y;
	_z -= value->_z;
	_w -= value->_w;

	return std::static_pointer_cast<Vector4>(shared_from_this());
}

Vector4::ptr
Vector4::operator-(Vector4::ptr value)
{
	return Vector4::create(_x - value->_x, _y - value->_y, _z - value->_z, _w - value->_w);
}

bool
Vector4::operator==(Vector4::ptr value)
{
	return _x == value->_x && _y == value->_y && _z == value->_z && _w == value->_w;
}
