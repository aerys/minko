/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/math/Box.hpp"

#include "minko/math/Ray.hpp"

using namespace minko;
using namespace minko::math;

Box::Box() :
	_topRight(0.),
	_bottomLeft(0.)
{

}

float
Box::distance(const math::vec3& position)
{
    const auto withinBounds =
        position.x > _bottomLeft.x &&
        position.y > _bottomLeft.y &&
        position.z > _bottomLeft.z &&
        position.x < _topRight.x &&
        position.y < _topRight.y &&
        position.z < _topRight.z;

    if (withinBounds)
        return 0.f;

    auto squareDistance = 0.f;

    for (auto i = 0u; i < 3u; ++i)
    {
        if (position[i] < _bottomLeft[i])
        {
            const auto delta = _bottomLeft[i] - position[i];

            squareDistance += delta * delta;
        }
        else if (position[i] > _topRight[i])
        {
            const auto delta = position[i] - _topRight[i];

            squareDistance += delta * delta;
        }
    }

    return math::sqrt(squareDistance);
}

bool
Box::cast(Ray::Ptr ray, float& distance)
{
	math::vec3 near, far;

	if (_topRight.z > _bottomLeft.z)
	{
		near = _bottomLeft;
		far = _topRight;
	}
	else
	{
		near = _topRight;
		far = _bottomLeft;
	}

	auto t0x = (near.x - ray->origin().x) / ray->direction().x;
	auto t1x = (far.x - ray->origin().x) / ray->direction().x;

	if (t0x > t1x)
		std::swap(t0x, t1x);

	auto tmin = t0x;
	auto tmax = t1x;

	auto t0y = (near.y - ray->origin().y) / ray->direction().y;
	auto t1y = (far.y - ray->origin().y) / ray->direction().y;

	if (t0y > t1y)
		std::swap(t0y, t1y);

	if (t0y > tmax || tmin > t1y)
		return false;

	if (t0y > tmin)
		tmin = t0y;
	if (t1y < tmax)
		tmax = t1y;

	auto t0z = (near.z - ray->origin().z) / ray->direction().z;
	auto t1z = (far.z - ray->origin().z) / ray->direction().z;

	if (t0z > t1z)
		std::swap(t0z, t1z);
	
	if (t0z > tmax || tmin > t1z)
		return false;

	if (t0z > tmin)
		tmin = t0z;
	if (t1z < tmax)
		tmax = t1z;

	distance = tmin;

	return true;
}

std::array<math::vec3, 8>
Box::getVertices()
{
	std::array<math::vec3, 8> vertices = 
	 { {
		math::vec3(_topRight),
		math::vec3(_topRight.x - width(), _topRight.y, _topRight.z),
		math::vec3(_topRight.x - width(), _topRight.y, _topRight.z - depth()),
		math::vec3(_topRight.x, _topRight.y, _topRight.z - depth()),
		math::vec3(_bottomLeft),
		math::vec3(_bottomLeft.x + width(), _bottomLeft.y, _bottomLeft.z),
		math::vec3(_bottomLeft.x + width(), _bottomLeft.y, _bottomLeft.z + depth()),
		math::vec3(_bottomLeft.x, _bottomLeft.y, _bottomLeft.z + depth()),
	} };

	return vertices;
}

Box::Ptr
Box::merge(Ptr box2)
{
	return merge(shared_from_this(), box2, shared_from_this());
}

Box::Ptr
Box::merge(Ptr box1, Ptr box2, Ptr out)
{
	if (out == nullptr)
		out = create();

	out->topRight(vec3(
		std::max(box1->_topRight.x, box2->_topRight.x),
		std::max(box1->_topRight.y, box2->_topRight.y),
		std::max(box1->_topRight.z, box2->_topRight.z)
	));

	out->bottomLeft(vec3(
		std::min(box1->_bottomLeft.x, box2->_bottomLeft.x),
		std::min(box1->_bottomLeft.y, box2->_bottomLeft.y),
		std::min(box1->_bottomLeft.z, box2->_bottomLeft.z)
	));

	return out;
}

ShapePosition
Box::testBoundingBox(std::shared_ptr<math::Box> box)
{
	if (box->bottomLeft().x > this->topRight().x)
		return ShapePosition::LEFT;

	if (box->topRight().x < this->bottomLeft().x)
		return ShapePosition::RIGHT;

	if (box->bottomLeft().y > this->topRight().y)
		return ShapePosition::BOTTOM;

	if (box->topRight().y < this->bottomLeft().y)
		return ShapePosition::TOP;

	if (box->topRight().z < this->bottomLeft().z)
		return ShapePosition::FAR;

	if (box->bottomLeft().z > this->topRight().z)
		return ShapePosition::NEAR;

	if (this->bottomLeft().x > box->bottomLeft().x && 
		this->bottomLeft().y > box->bottomLeft().y && 
		this->bottomLeft().z > box->bottomLeft().z &&
		this->topRight().x < box->topRight().x && 
		this->topRight().y < box->topRight().y && 
		this->topRight().z < box->topRight().z)
		return ShapePosition::INSIDE;

	return ShapePosition::AROUND;
}

void
Box::updateFromMatrix(const math::mat4& matrix)
{
    _bottomLeft = math::vec3(matrix * math::vec4(_bottomLeft, 1.f));
    _topRight = math::vec3(matrix * math::vec4(_topRight, 1.f));
}