#pragma once

#include <memory>

#include "Vector3.hpp"

class Vector4 :
	private std::enable_shared_from_this<Vector4>,
	public Vector3
{
public:
	typedef std::shared_ptr<Vector3>	ptr;

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

	ptr
	subtract(ptr value)
	{
		return Vector4::shared_from_this();
	}

protected:
	Vector4(float x, float y, float z, float w);
};