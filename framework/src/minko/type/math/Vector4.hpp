#pragma once

#include <memory>

#include "Vector3.hpp"

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
	create(float x, float y, float z, float w);

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

	Vector4&
	operator-(Vector4& value);

	bool
	operator==(Vector4& value);

	ptr
	incrementBy(ptr value);

	ptr
	decrementBy(ptr value);

protected:
	Vector4(float x, float y, float z, float w);
};