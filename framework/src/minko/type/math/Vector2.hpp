#pragma once

#include <memory>

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
	create(float x, float y);

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

protected:
	Vector2(float x, float y);
};