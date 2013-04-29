#pragma once

#include "minko/Common.hpp"
#include "minko/type/math/Vector2.hpp"

class Vector3 :
	public Vector2
{
public:
	typedef std::shared_ptr<Vector3>	ptr;

protected:
	float _z;

public:
	inline static
	ptr
	create(float x, float y, float z)
	{
		return std::shared_ptr<Vector3>(new Vector3(x, y, z));
	}

	inline
	float
	z()
	{
		return _z;
	}

	inline
	void
	z(float z)
	{
		_z = z;
	}

	inline
	ptr
	operator-(Vector3::ptr value)
	{
		return Vector3::create(_x - value->_x, _y - value->_y, _z - value->_z);
	}

	inline
	ptr
	operator+(Vector3::ptr value)
	{
		return Vector3::create(_x + value->_x, _y + value->_y, _z + value->_z);
	}

	inline
	ptr
	operator+=(Vector3::ptr value)
	{
		_x += value->_x;
		_y += value->_y;
		_z += value->_z;

		return std::static_pointer_cast<Vector3>(shared_from_this());
	}

	inline
	ptr
	operator-=(Vector3::ptr value)
	{
		_x -= value->_x;
		_y -= value->_y;
		_z -= value->_z;

		return std::static_pointer_cast<Vector3>(shared_from_this());
	}

	inline
	bool
	operator==(Vector3::ptr value)
	{
		return _x == value->_x && _y == value->_y && _z == value->_z;
	}

protected:
	Vector3(float x, float y, float z) :
		Vector2(x, y),
		_z(z)
	{
	}
};