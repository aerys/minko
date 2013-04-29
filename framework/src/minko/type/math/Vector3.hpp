#pragma once

#include <memory>

#include "Vector2.hpp"

class Vector3 :
	private std::enable_shared_from_this<Vector3>,
	public Vector2
{
public:
	typedef std::shared_ptr<Vector3>	ptr;

protected:
	float _z;

public:
	inline static
	ptr
	create(float x, float y, float z);

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

protected:
	Vector3(float x, float y, float z);
};