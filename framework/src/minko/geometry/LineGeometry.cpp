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

#include "minko/geometry/LineGeometry.hpp"

#include "minko/render/AbstractContext.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"

using namespace minko;
using namespace minko::geometry;
using namespace minko::render;

/*static*/ const uint		 LineGeometry::MAX_NUM_LINES		= 1048576;
/*static*/ const std::string LineGeometry::ATTRNAME_START_POS	= "startPosition";
/*static*/ const std::string LineGeometry::ATTRNAME_STOP_POS    = "stopPosition";
/*static*/ const std::string LineGeometry::ATTRNAME_WEIGHTS		= "weights";
/*static*/ const std::string LineGeometry::ATTRNAME_DASHOFFSET	= "dashOffset";

LineGeometry::LineGeometry():
	Geometry("line"),
	_currentX(0.f),
	_currentY(0.f),
	_currentZ(0.f),
    _dashOffset(0.f),
	_numLines(0),
	_vertexBuffer(nullptr),
	_indexBuffer(nullptr)
{
}

void
LineGeometry::initialize(AbstractContext::Ptr context, int numSegments)
{
	if (context == nullptr)
		throw std::invalid_argument("context");

	_vertexBuffer = VertexBuffer::create(context);
	_indexBuffer = IndexBuffer::create(context);

	_vertexBuffer->addAttribute(ATTRNAME_START_POS,	3, 0);
	_vertexBuffer->addAttribute(ATTRNAME_STOP_POS, 3, 3);
	_vertexBuffer->addAttribute(ATTRNAME_WEIGHTS, 3, 6);
    _vertexBuffer->addAttribute(ATTRNAME_DASHOFFSET, 1, 9);

    if (numSegments > 0)
    {
        _vertexBuffer->data().reserve(4 * numSegments * _vertexBuffer->vertexSize());
        _indexBuffer->data().reserve(6 * numSegments);
    }
}

math::vec3
LineGeometry::currentXYZ() const
{
	return math::vec3(_currentX, _currentY, _currentZ);
}

LineGeometry::Ptr
LineGeometry::moveTo(const math::vec3& xyz)
{
	return moveTo(xyz.x, xyz.y, xyz.z);
}

LineGeometry::Ptr
LineGeometry::lineTo(const math::vec3& xyz, unsigned int numSegments)
{
	return lineTo(xyz.x, xyz.y, xyz.z, numSegments);
}

LineGeometry::Ptr
LineGeometry::lineTo(float x, float y, float z, unsigned int numSegments)
{
	if (numSegments == 0)
		return moveTo(x, y, z);

	const uint vertexSize = _vertexBuffer->vertexSize();
    const auto oldVertexDataSize = _vertexBuffer->data().size();
    const auto oldIndexDataSize = _indexBuffer->data().size();

    auto& vertexData = _vertexBuffer->data();
    auto& indexData = _indexBuffer->data();

    vertexData.resize(oldVertexDataSize + 4 * numSegments * vertexSize);
    indexData.resize(oldIndexDataSize + 6 * numSegments);

	const float invNumSegments	= 1.f / (float)numSegments;
	const float stepX = (x - _currentX) * invNumSegments;
	const float stepY = (y - _currentY) * invNumSegments;
	const float stepZ = (z - _currentZ) * invNumSegments;
	// Dashes are only used with 2D lines
    const float segmentLength = math::length(math::vec2(stepX, stepY) * _viewport);
	auto vid = oldVertexDataSize;
	auto iid = oldIndexDataSize;

	for (unsigned int segmentId = 0; segmentId < numSegments; ++segmentId)
	{
        if (_numLines >= MAX_NUM_LINES)
        {
            throw std::logic_error(
                "Maximal number of segments (" + std::to_string(_numLines) + ") for line geometry reached."
            );
        }

		const float nextX = _currentX + stepX;
		const float nextY = _currentY + stepY;
		const float nextZ = _currentZ + stepZ;
        const float nextDashOffset = _dashOffset + segmentLength;

		for (unsigned int k = 0; k < 4; ++k)
		{
            const bool  isStart = k < 2;
			const float wStart = isStart ? 1.f : 0.f;
			const float	wStop = isStart ? 0.f : 1.f;
			const float lineSpread = 0 < k && k < 3 ? 1.f : -1.f;
            const float dashOffset = (isStart ? _dashOffset : nextDashOffset) * 2.f;

			// start position
			vertexData[vid++] = _currentX;
			vertexData[vid++] = _currentY;
			vertexData[vid++] = _currentZ;

			// stop position
			vertexData[vid++] = nextX;
			vertexData[vid++] = nextY;
			vertexData[vid++] = nextZ;

			// weights attribute
			vertexData[vid++] = wStart;
			vertexData[vid++] = wStop;
			vertexData[vid++] = lineSpread;

            // dash offset attribute
            vertexData[vid++] = dashOffset;
		}

		const unsigned int iOffset = (_numLines << 2);
		indexData[iid++] = iOffset;
		indexData[iid++] = iOffset + 2;
		indexData[iid++] = iOffset + 1;

		indexData[iid++] = iOffset;
		indexData[iid++] = iOffset + 3;
		indexData[iid++] = iOffset + 2;

		_currentX = nextX;
		_currentY = nextY;
		_currentZ = nextZ;
        _dashOffset = nextDashOffset;
		++_numLines;
	}

#ifdef DEBUG
	assert(vid == vertexData.size());
	assert(iid == indexData.size());
#endif

	return std::static_pointer_cast<LineGeometry>(shared_from_this());
}

void
LineGeometry::upload()
{
	_indexBuffer->upload();
	_vertexBuffer->upload();

	addVertexBuffer(_vertexBuffer);
	indices(_indexBuffer);

    computeCenterPosition();
}
