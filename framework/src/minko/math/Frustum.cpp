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

	_planes[(int)FrustumPosition::LEFT]	->setTo(data[12] + data[0], data[13] + data[1], data[14] + data[2], data[15] + data[3])->normalize();
	_planes[(int)FrustumPosition::TOP]		->setTo(data[12] - data[4], data[13] - data[5], data[14] - data[6], data[15] - data[7])->normalize();
	_planes[(int)FrustumPosition::RIGHT]	->setTo(data[12] - data[0], data[13] - data[1], data[14] - data[2], data[15] - data[3])->normalize();
	_planes[(int)FrustumPosition::BOTTOM]	->setTo(data[12] + data[4], data[13] + data[5], data[14] + data[6], data[15] + data[7])->normalize();
	_planes[(int)FrustumPosition::FAR]		->setTo(data[12] - data[8], data[13] - data[9], data[14] - data[10], data[15] - data[11])->normalize();
	_planes[(int)FrustumPosition::NEAR]	->setTo(data[8], data[9], data[10], data[11])->normalize();
}

FrustumPosition
Frustum::testBoundingBox(math::Box::Ptr box)
{
	int result = 0;
	
	// bottom left front
	float xblf = box->bottomLeft()->x();
	float yblf = box->bottomLeft()->y();
	float zblf = box->bottomLeft()->z();

	// top right back
	float xtrb = box->topRight()->x();
	float ytrb = box->topRight()->y();
	float ztrb = box->topRight()->z();

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
	float xtlf = xtrb;
	float ytlf = ytrb;
	float ztlf = zblf;

	// top right front
	float xtrf = xblf;
	float ytrf = ytrb;
	float ztrf = zblf;

	for (uint planeId = 0; planeId < _planes.size(); ++planeId)
	{
		float pa = _planes[planeId]->x();
		float pb = _planes[planeId]->y();
		float pc = _planes[planeId]->z();
		float pd = _planes[planeId]->w();

		
		_blfResult[planeId] = pa * xblf + pb * yblf + pc * zblf + pd < 0.;
		_brfResult[planeId] = pa * xbrf + pb * ybrf + pc * zbrf + pd < 0.;
		_blbResult[planeId] = pa * xblb + pb * yblb + pc * zblb + pd < 0.;
		_brbResult[planeId] = pa * xbrb + pb * ybrb + pc * zbrb + pd < 0.;
		
		_tlfResult[planeId] = pa * xtlf + pb * ytlf + pc * ztlf + pd < 0.;
		_trfResult[planeId] = pa * xtrf + pb * ytrf + pc * ztrf + pd < 0.;
		_tlbResult[planeId] = pa * xtlb + pb * ytlb + pc * ztlb + pd < 0.;
		_trbResult[planeId] = pa * xtrb + pb * ytrb + pc * ztrb + pd < 0.;
	}

	if ((_blfResult[(int)FrustumPosition::LEFT]		&& _trbResult[(int)FrustumPosition::RIGHT]) ||
		(_blfResult[(int)FrustumPosition::RIGHT]	&& _trbResult[(int)FrustumPosition::LEFT]) ||
		(_blfResult[(int)FrustumPosition::TOP]		&& _trbResult[(int)FrustumPosition::BOTTOM]) ||
		(_blfResult[(int)FrustumPosition::BOTTOM]	&& _trbResult[(int)FrustumPosition::TOP]))
		return FrustumPosition::AROUND;

	for (uint planeId = 0; planeId < _planes.size(); ++planeId)
	{
		if (_blfResult[planeId] &&
			_brfResult[planeId] &&
			_blbResult[planeId] &&
			_brbResult[planeId] &&
			_tlfResult[planeId] &&
			_trfResult[planeId] &&
			_tlbResult[planeId] &&
			_trbResult[planeId]
			)
			return static_cast<FrustumPosition>(planeId);
	}

	return FrustumPosition::INSIDE;
}

Frustum::Frustum()
{
	_planes[(int)FrustumPosition::LEFT]		= Vector4::create();
	_planes[(int)FrustumPosition::RIGHT]	= Vector4::create();
	_planes[(int)FrustumPosition::TOP]		= Vector4::create();
	_planes[(int)FrustumPosition::BOTTOM]	= Vector4::create();
	_planes[(int)FrustumPosition::FAR]		= Vector4::create();
	_planes[(int)FrustumPosition::NEAR]		= Vector4::create();
}