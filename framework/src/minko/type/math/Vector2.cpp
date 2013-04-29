#include "Vector2.hpp"

Vector2::ptr
Vector2::create(float x, float y)
{
	return std::shared_ptr<Vector2>(new Vector2(x, y));
}

Vector2::Vector2(float x, float y) :
	_x(x),
	_y(y)
{
}
