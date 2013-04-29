#pragma once

#include "minko/Common.hpp"
#include "minko/type/math/Vector2.hpp"

class Vector3 :
	public Vector2
{
public:
	typedef std::shared_ptr<Vector3>	ptr;
	typedef std::shared_ptr<Vector3>	const_ptr;

protected:
	float _z;

public:
	inline static
	ptr
	create(float x = 0., float y = 0., float z = 0.)
	{
		return std::shared_ptr<Vector3>(new Vector3(x, y, z));
	}

	inline static
	const_ptr
	createConst(float x, float y, float z)
	{
		return std::shared_ptr<Vector3>(new Vector3(x, y, z));
	}

	inline static
	const_ptr
	upAxis()
	{
		static const_ptr upAxis = createConst(0., 1., 0.);

		return upAxis;
	}

	inline static
	const_ptr
	xAxis()
	{
		static const_ptr xAxis = createConst(1., 0., 0.);

		return xAxis;
	}

	inline static
	const_ptr
	yAxis()
	{
		static const_ptr yAxis = createConst(0., 1., 0.);

		return yAxis;
	}

	inline static
	const_ptr
	zAxis()
	{
		static const_ptr zAxis = createConst(0., 0., 1.);

		return zAxis;
	}

	inline static
	const_ptr
	zero()
	{
		static const_ptr zAxis = createConst(0., 0., 0.);

		return zAxis;
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
	copyFrom(ptr value)
	{
		_x = value->_x;
		_y = value->_y;
		_z = value->_z;

		return std::static_pointer_cast<Vector3>(shared_from_this());
	}

	inline
	ptr
	normalize()
	{
		float l = sqrtf(_x * _x + _y * _y + _z * _z);

		if (l != 0.)
		{
			_x /= l;
			_y /= l;
			_z /= l;
		}

		return std::static_pointer_cast<Vector3>(shared_from_this());
	}

	inline
	ptr
	cross(ptr value)
	{
		_x = _y * value->_z - _z * value->_y;
		_y = _z * value->_x - _x * value->_z;
		_z = _x * value->_y - _y * value->_x;

		return std::static_pointer_cast<Vector3>(shared_from_this());
	}

	inline
	float
	dot(ptr value)
	{
		return _x * value->_x + _y * value->_y + _z * value->_z;
	}

	inline
	ptr
	operator-()
	{
		return create(-_x, -_y, -_z);
	}

	inline
	ptr
	operator-(ptr value)
	{
		return create(_x - value->_x, _y - value->_y, _z - value->_z);
	}

	inline
	ptr
	operator+(ptr value)
	{
		return create(_x + value->_x, _y + value->_y, _z + value->_z);
	}

	inline
	ptr
	operator+=(ptr value)
	{
		_x += value->_x;
		_y += value->_y;
		_z += value->_z;

		return std::static_pointer_cast<Vector3>(shared_from_this());
	}

	inline
	ptr
	operator-=(ptr value)
	{
		_x -= value->_x;
		_y -= value->_y;
		_z -= value->_z;

		return std::static_pointer_cast<Vector3>(shared_from_this());
	}

protected:
	Vector3(float x, float y, float z) :
		Vector2(x, y),
		_z(z)
	{
	}
};
