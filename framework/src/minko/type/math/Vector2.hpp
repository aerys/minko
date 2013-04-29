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
	operator-(Vector2::ptr value)
	{
		return Vector2::create(_x - value->_x, _y - value->_y);
	}

	inline
	ptr
	operator+(Vector2::ptr value)
	{
		return Vector2::create(_x + value->_x, _y + value->_y);
	}

	inline
	ptr
	operator+=(Vector2::ptr value)
	{
		_x += value->_x;
		_y += value->_y;

		return std::static_pointer_cast<Vector2>(shared_from_this());
	}

	inline
	ptr
	operator-=(Vector2::ptr value)
	{
		_x -= value->_x;
		_y -= value->_y;

		return std::static_pointer_cast<Vector2>(shared_from_this());
	}

	inline
	bool
	operator==(Vector2::ptr value)
	{
		return _x == value->_x && _y == value->_y;
	}

protected:
	Vector2(float x, float y) :
		_x(x),
		_y(y)
	{
	}
};