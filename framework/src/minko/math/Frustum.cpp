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

#include "minko/math/Frustum.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/math/Box.hpp"

using namespace minko;
using namespace minko::math;

void
Frustum::updateFromMatrix(std::shared_ptr<math::Matrix4x4> matrix)
{
	std::shared_ptr<math::Matrix4x4>	clone	= math::Matrix4x4::create(matrix);
	std::vector<float>					data	= clone->data();

	_planes[FrustumPosition::LEFT]	->setTo(data[12] + data[0], data[13] + data[1], data[14] + data[2], data[15] + data[3])->normalize();
	_planes[FrustumPosition::TOP]	->setTo(data[12] - data[4], data[13] - data[5], data[14] - data[6], data[15] - data[7])->normalize();
	_planes[FrustumPosition::RIGHT]	->setTo(data[12] - data[0], data[13] - data[1], data[14] - data[2], data[15] - data[3])->normalize();
	_planes[FrustumPosition::BOTTOM]->setTo(data[12] + data[4], data[13] + data[5], data[14] + data[6], data[15] + data[7])->normalize();
	_planes[FrustumPosition::FAR]	->setTo(data[12] - data[8], data[13] - data[9], data[14] - data[10], data[15] - data[11])->normalize();
	_planes[FrustumPosition::NEAR]	->setTo(data[8], data[9], data[10], data[11])->normalize();
}

FrustumPosition
Frustum::testBoundingBox(std::shared_ptr<math::Box> box)
{
	int result = 0;
	
	float x1 = box->bottomLeft()->x();
	float y1 = box->bottomLeft()->y();
	float z1 = box->bottomLeft()->z();

	float x2 = box->topRight()->x();
	float y2 = box->topRight()->y();
	float z2 = box->topRight()->z();

	std::unordered_map<FrustumPosition, bool> blResult;
	std::unordered_map<FrustumPosition, bool> trResult;

	uint numOutside = 0;

	for (auto it = _planes.begin(); it != _planes.end(); ++it)
	{
		float pa = it->second->x();
		float pb = it->second->y();
		float pc = it->second->z();
		float pd = it->second->w();

		blResult[it->first] = pa * x1 + pb * y1 + pc * z1 + pd < 0.;
		trResult[it->first] = pa * x2 + pb * y2 + pc * z2 + pd < 0.;
	}

	if (blResult[FrustumPosition::LEFT] && trResult[FrustumPosition::RIGHT] ||
		blResult[FrustumPosition::RIGHT] && trResult[FrustumPosition::LEFT] ||
		blResult[FrustumPosition::TOP] && trResult[FrustumPosition::BOTTOM] ||
		blResult[FrustumPosition::BOTTOM] && trResult[FrustumPosition::TOP])
		return FrustumPosition::AROUND;

	for (auto it2 = blResult.begin(); it2 != blResult.end(); ++it2)
	{
		if (blResult[it2->first] && trResult[it2->first])
			return it2->first;
	}

	return FrustumPosition::INSIDE;
}

Frustum::Frustum()
{
	_planes[FrustumPosition::LEFT]		= Vector4::create();
	_planes[FrustumPosition::RIGHT]		= Vector4::create();
	_planes[FrustumPosition::TOP]		= Vector4::create();
	_planes[FrustumPosition::BOTTOM]	= Vector4::create();
	_planes[FrustumPosition::FAR]		= Vector4::create();
	_planes[FrustumPosition::NEAR]		= Vector4::create();
}