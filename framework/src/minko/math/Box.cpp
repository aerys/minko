/*
Copyright (c) 2013 Aerys

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

bool
Box::cast(Ray::Ptr ray, float& distance)
{
	math::Vector3 near, far;

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

std::array<math::Vector3, 8>
Box::getVertices()
{
	std::array<math::Vector3, 8> vertices = 
	 { {
		math::Vector3(_topRight),
		math::Vector3(_topRight.x - width(), _topRight.y, _topRight.z),
		math::Vector3(_topRight.x - width(), _topRight.y, _topRight.z - depth()),
		math::Vector3(_topRight.x, _topRight.y, _topRight.z - depth()),
		math::Vector3(_bottomLeft),
		math::Vector3(_bottomLeft.x + width(), _bottomLeft.y, _bottomLeft.z),
		math::Vector3(_bottomLeft.x + width(), _bottomLeft.y, _bottomLeft.z + depth()),
		math::Vector3(_bottomLeft.x, _bottomLeft.y, _bottomLeft.z + depth()),
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

	out->topRight(Vector3(
		std::max(box1->_topRight.x, box2->_topRight.x),
		std::max(box1->_topRight.y, box2->_topRight.y),
		std::max(box1->_topRight.z, box2->_topRight.z)
	));

	out->bottomLeft(Vector3(
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
Box::updateFromMatrix(const math::Matrix4x4& matrix)
{
	// FIXME
}