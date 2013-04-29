#pragma once

#include "minko/Common.hpp"

class Vector2 :
	public std::enable_shared_from_this<Vector2>
{
public:
	typedef std::shared_ptr<Vector2>	ptr;

protected:
	float _x;
	float _y;

public:
	inline static
	ptr
	create(float x, float y)
	{
		return std::shared_ptr<Vector2>(new Vector2(x, y));
	}

	inline
	float
	x()
	{
		return _x;
	}

	inline
	float
	y()
	{
		return _y;
	}

	inline
	void
	x(float x)
	{
		_x = x;
	}

	inline
	void
	y(float y)
	{
		_y = y;
	}

	inline
	ptr
	copyFrom(ptr value)
	{
		return setTo(value->_x, value->_y);
	}

	inline
	ptr
	setTo(float x, float y)
	{
		_x = x;
		_y = y;

		return std::static_pointer_cast<Vector2>(shared_from_this());
	}

	ptr
	normalize()
	{
		float l = sqrtf(_x * _x + _y * _y);
		
		if (l != 0.)
		{
			_x /= l;
			_y /= l;
		}

		return shared_from_this();
	}

	inline
	ptr
	cross(ptr value)
	{
		float x = _y * value->_x - _x * value->_y;
		float y = _x * value->_y - _y * value->_x;

		_x = x;
		_y = y;

		return std::static_pointer_cast<Vector2>(shared_from_this());
	}

	inline
	float
	dot(ptr value)
	{
		return _x * value->_x + _y * value->_y;
	}

	inline
	ptr
	operator-()
	{
		return create(-_x, -_y);
	}

	inline
	ptr
	operator-(ptr value)
	{
		return create(_x - value->_x, _y - value->_y);
	}

	inline
	ptr
	operator+(ptr value)
	{
		return create(_x + value->_x, _y + value->_y);
	}

	inline
	ptr
	operator+=(ptr value)
	{
		_x += value->_x;
		_y += value->_y;

		return std::static_pointer_cast<Vector2>(shared_from_this());
	}

	inline
	ptr
	operator-=(ptr value)
	{
		_x -= value->_x;
		_y -= value->_y;

		return std::static_pointer_cast<Vector2>(shared_from_this());
	}

protected:
	Vector2(float x, float y) :
		_x(x),
		_y(y)
	{
	}
};

inline
Vector2::ptr
operator-(Vector2::ptr value)
{
	return Vector2::create(-value->x(), -value->y());
}

inline
Vector2::ptr
operator-(Vector2::ptr a, Vector2::ptr b)
{
	return Vector2::create(a->x() - b->x(), a->y() - b->y());
}

inline
Vector2::ptr
operator+(Vector2::ptr a, Vector2::ptr b)
{
	return Vector2::create(a->x() + b->x(), a->y() + b->y());
}

inline
Vector2::ptr
operator+=(Vector2::ptr a, Vector2::ptr b)
{
	a->x(a->x() + b->x());
	a->y(a->y() + b->y());

	return a;
}

inline
Vector2::ptr
operator-=(Vector2::ptr a, Vector2::ptr b)
{
	a->x(a->x() - b->x());
	a->y(a->y() - b->y());

	return a;
}
