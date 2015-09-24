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

#include "minko/math/Frustum.hpp"

#include "minko/math/Box.hpp"

using namespace minko;

void
math::Frustum::updateFromMatrix(const math::mat4& matrix)
{
    const auto transposedMatrix = math::transpose(matrix);

    _planes[static_cast<int>(PlanePosition::LEFT)] = math::normalize(transposedMatrix[3] + transposedMatrix[0]);
    _planes[static_cast<int>(PlanePosition::RIGHT)] = math::normalize(transposedMatrix[3] - transposedMatrix[0]);

    _planes[static_cast<int>(PlanePosition::BOTTOM)] = math::normalize(transposedMatrix[3] + transposedMatrix[1]);
    _planes[static_cast<int>(PlanePosition::TOP)] = math::normalize(transposedMatrix[3] - transposedMatrix[1]);

    _planes[static_cast<int>(PlanePosition::NEAR)] = math::normalize(transposedMatrix[3] + transposedMatrix[2]);
    _planes[static_cast<int>(PlanePosition::FAR)] = math::normalize(transposedMatrix[3] - transposedMatrix[2]);
}

math::ShapePosition
math::Frustum::testBoundingBox(math::Box::Ptr box)
{
	return testBoundingBox(box, 0u).first;
}

std::pair<math::ShapePosition, unsigned int>
math::Frustum::testBoundingBox(math::Box::Ptr box, unsigned int basePlaneId)
{
	int result = 0;
	
	// bottom left front
	float xblf = box->bottomLeft().x;
	float yblf = box->bottomLeft().y;
	float zblf = box->bottomLeft().z;

	// top right back
	float xtrb = box->topRight().x;
	float ytrb = box->topRight().y;
	float ztrb = box->topRight().z;

	// bottom right front
	float xbrf = xtrb;
	float ybrf = yblf;
	float zbrf = zblf;

	// bottom left back
	float xblb = xblf;
	float yblb = yblf;
	float zblb = ztrb;

	// bottom right back
	float xbrb = xtrb;
	float ybrb = yblf;
	float zbrb = ztrb;

	// top left back
	float xtlb = xblf;
	float ytlb = ytrb;
	float ztlb = ztrb;

	// top left front
	float xtlf = xtlb;
	float ytlf = ytrb;
	float ztlf = zblf;

	// top right front
	float xtrf = xbrf;
	float ytrf = ytrb;
	float ztrf = zblf;

	for (auto i = 0u; i < _planes.size(); ++i)
	{
		const auto planeId = (basePlaneId + i) % _planes.size();

		float pa = _planes[planeId].x;
		float pb = _planes[planeId].y;
		float pc = _planes[planeId].z;
		float pd = _planes[planeId].w;

		_blfResult[planeId] = pa * xblf + pb * yblf + pc * zblf + pd < 0.f;
		_brfResult[planeId] = pa * xbrf + pb * ybrf + pc * zbrf + pd < 0.f;
		_blbResult[planeId] = pa * xblb + pb * yblb + pc * zblb + pd < 0.f;
		_brbResult[planeId] = pa * xbrb + pb * ybrb + pc * zbrb + pd < 0.f;
		
		_tlfResult[planeId] = pa * xtlf + pb * ytlf + pc * ztlf + pd < 0.f;
		_trfResult[planeId] = pa * xtrf + pb * ytrf + pc * ztrf + pd < 0.f;
		_tlbResult[planeId] = pa * xtlb + pb * ytlb + pc * ztlb + pd < 0.f;
		_trbResult[planeId] = pa * xtrb + pb * ytrb + pc * ztrb + pd < 0.f;

		if (_blfResult[planeId] &&
			_brfResult[planeId] &&
			_blbResult[planeId] &&
			_brbResult[planeId] &&
			_tlfResult[planeId] &&
			_trfResult[planeId] &&
			_tlbResult[planeId] &&
			_trbResult[planeId])
		{
			return std::make_pair(static_cast<ShapePosition>(planeId), planeId);
		}
	}

	if (((_blfResult[(int)PlanePosition::LEFT]	&& _trbResult[(int)PlanePosition::RIGHT]) ||
		(_blfResult[(int)PlanePosition::RIGHT]	&& _trbResult[(int)PlanePosition::LEFT])) &&
		((_blfResult[(int)PlanePosition::TOP]	&& _trbResult[(int)PlanePosition::BOTTOM]) ||
		(_blfResult[(int)PlanePosition::BOTTOM] && _trbResult[(int)PlanePosition::TOP])) &&
        ((_blfResult[(int)PlanePosition::NEAR]	&& _trbResult[(int)PlanePosition::FAR]) ||
		(_blfResult[(int)PlanePosition::FAR]	&& _trbResult[(int)PlanePosition::NEAR])))
		return std::make_pair(ShapePosition::AROUND, 0u);

	return std::make_pair(ShapePosition::INSIDE, 0u);
}

bool
math::Frustum::cast(std::shared_ptr<Ray> ray, float& distance)
{
	return false;
}
