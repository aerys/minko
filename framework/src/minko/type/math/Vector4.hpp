#pragma once

#include "minko/Common.hpp"
#include "minko/type/math/Vector3.hpp"

class Vector4 :
	public Vector3
{
public:
	typedef std::shared_ptr<Vector4>	ptr;

protected:
	float _w;

public:
	inline static
	ptr
	create(float x, float y, float z, float w = 1.)
	{
		return std::shared_ptr<Vector4>(new Vector4(x, y, z, w));
	}

	inline
	float
	w()
	{
		return _w;
	}

	inline
	void
	w(float w)
	{
		_w = w;
	}

	inline
	ptr
	copyFrom(ptr value)
	{
		_x = value->_x;
		_y = value->_y;
		_z = value->_z;
		_w = value->_w;

		return std::static_pointer_cast<Vector4>(shared_from_this());
	}

	ptr
	normalize()
	{
		float l = sqrtf(_x * _x + _y * _y + _z * _z + _w * _w);

		if (l != 0.)
		{
			_x /= l;
			_y /= l;
			_z /= l;
			_w /= l;
		}

		return std::static_pointer_cast<Vector4>(shared_from_this());
	}

	ptr
	cross(ptr value)
	{
		_x = _y * value->_z - _z * value->_y;
		_y = _z * value->_w - _w * value->_z;
		_z = _w * value->_x - _x * value->_w;
		_w = _x * value->_y - _y * value->_x;

		return std::static_pointer_cast<Vector4>(shared_from_this());
	}

	float
	dot(ptr value)
	{
		return _x * value->_x + _y * value->_y + _z * value->_z + _w * value->_w;
	}

protected:
	Vector4(float x, float y, float z, float w) :
		Vector3(x, y, z),
		_w(w)
	{
	}
};

inline
Vector4::ptr
operator-(Vector4::ptr value)
{
	return Vector4::create(-value->x(), -value->y(), -value->z(), -value->w());
}

inline
Vector4::ptr
operator-(Vector4::ptr a, Vector4::ptr b)
{
	return Vector4::create(a->x() - b->x(), a->y() - b->y(), a->z() - b->z(), a->w() - b->w());
}

inline
Vector4::ptr
operator+(Vector4::ptr a, Vector4::ptr b)
{
	return Vector4::create(a->x() + b->x(), a->y() + b->y(), a->z() + b->z(), a->w() + b->w());
}

inline
Vector4::ptr
operator+=(Vector4::ptr a, Vector4::ptr b)
{
	a->x(a->x() + b->x());
	a->y(a->y() + b->y());
	a->z(a->z() + b->z());
	a->w(a->w() + b->w());

	return a;
}

inline
Vector4::ptr
operator-=(Vector4::ptr a, Vector4::ptr b)
{
	a->x(a->x() - b->x());
	a->y(a->y() - b->y());
	a->z(a->z() - b->z());
	a->z(a->w() - b->w());

	return a;
}
