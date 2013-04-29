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
	operator-(Vector4::ptr value)
	{
		return Vector4::create(_x - value->_x, _y - value->_y, _z - value->_z, _w - value->_w);
	}

	inline
	ptr
	operator+(Vector4::ptr value)
	{
		return Vector4::create(_x + value->_x, _y + value->_y, _z + value->_z, _w + value->_w);
	}

	inline
	ptr
	operator+=(Vector4::ptr value)
	{
		_x += value->_x;
		_y += value->_y;
		_z += value->_z;
		_w += value->_w;

		return std::static_pointer_cast<Vector4>(shared_from_this());
	}

	inline
	ptr
	operator-=(Vector4::ptr value)
	{
		_x -= value->_x;
		_y -= value->_y;
		_z -= value->_z;
		_w -= value->_w;

		return std::static_pointer_cast<Vector4>(shared_from_this());
	}

	inline
	bool
	operator==(Vector4::ptr value)
	{
		return _x == value->_x && _y == value->_y && _z == value->_z && _w == value->_w;
	}

protected:
	Vector4(float x, float y, float z, float w) :
		Vector3(x, y, z),
		_w(w)
	{
	}
};
