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

#include "minko/render/VertexBuffer.hpp"

#include "minko/Signal.hpp"
#include "minko/render/AbstractContext.hpp"

using namespace minko;
using namespace minko::render;

static const std::string ATTRNAME_POSITION = "position";

VertexBuffer::VertexBuffer(std::shared_ptr<AbstractContext> context) :
	AbstractResource(context),
	std::enable_shared_from_this<VertexBuffer>(),
	_data(),
	_vertexSize(0),
	_vertexSizeChanged(Signal<Ptr, int>::create())
{
}

VertexBuffer::VertexBuffer(std::shared_ptr<AbstractContext>	context,
						   float*							data,
						   const unsigned int				size,
						   const unsigned int				offset) :
	AbstractResource(context),
	_data(data + offset, data + offset + size),
	_vertexSize(0),
	_vertexSizeChanged(Signal<Ptr, int>::create())
{
	upload();
}

VertexBuffer::VertexBuffer(std::shared_ptr<AbstractContext>		context,
						   std::vector<float>::const_iterator	begin,
						   std::vector<float>::const_iterator	end) :
	AbstractResource(context),
	_data(begin, end),
	_vertexSize(0),
	_vertexSizeChanged(Signal<Ptr, int>::create())
{
	upload();
}

VertexBuffer::VertexBuffer(std::shared_ptr<AbstractContext> context, float* begin, float* end) :
	AbstractResource(context),
	_data(begin, end),
	_vertexSize(0),
	_vertexSizeChanged(Signal<Ptr, int>::create())
{
	upload();
}

void
VertexBuffer::upload(uint offset, uint numVertices)
{
	if (_data.empty())
		return;

    if (_id == -1)
    	_id = _context->createVertexBuffer(_data.size());

    _context->uploadVertexBufferData(
    	_id,
    	offset * _vertexSize,
    	numVertices == 0 ? _data.size() : numVertices * _vertexSize,
    	&_data[0]
    );

	//updatePositionBounds();
}

void
VertexBuffer::dispose()
{
    if (_id != -1)
    {
	    _context->deleteVertexBuffer(_id);
	    _id = -1;
    }

    disposeData();
}

void
VertexBuffer::disposeData()
{
    _data.clear();
    _data.shrink_to_fit();
}

void
VertexBuffer::addAttribute(const std::string& 	name,
						   const unsigned int	size,
						   const unsigned int	offset)
{
	if (hasAttribute(name))
		throw std::invalid_argument("name");

	_attributes.push_back({ &_id, &_vertexSize, name, size, offset });

	vertexSize(_vertexSize + size);

	if (name == ATTRNAME_POSITION)
		invalidatePositionBounds();
}

bool
VertexBuffer::hasAttribute(const std::string& attributeName) const
{
	auto it = std::find_if(_attributes.begin(), _attributes.end(), [&](const VertexAttribute& attr)
	{
		return attr.name == attributeName;
	});

	return it != _attributes.end();
}

void
VertexBuffer::removeAttribute(const std::string& attributeName)
{
	auto it = std::find_if(_attributes.begin(), _attributes.end(), [&](const VertexAttribute attr)
	{
		return attr.name == attributeName;
	});

	if (it == _attributes.end())
		throw std::invalid_argument("attributeName = " + attributeName);

	vertexSize(_vertexSize - it->size);
    _attributes.erase(it);

	if (attributeName == ATTRNAME_POSITION)
		invalidatePositionBounds();
}

const VertexBuffer::VertexAttribute&
VertexBuffer::attribute(const std::string& attributeName) const
{
    auto it = std::find_if(_attributes.begin(), _attributes.end(), [&](const VertexAttribute attr)
    {
        return attr.name == attributeName;
    });

    if (it == _attributes.end())
        throw std::invalid_argument("attributeName = " + attributeName);

    return *it;
}

void
VertexBuffer::vertexSize(unsigned int value)
{
	int offset = value - _vertexSize;

	_vertexSize = value;
	_vertexSizeChanged->execute(shared_from_this(), offset);
}


const math::vec3&
VertexBuffer::minPosition()
{
	if (!_validMinMax)
		updatePositionBounds();

	return _minPosition;
}

const math::vec3&
VertexBuffer::maxPosition()
{
	if (!_validMinMax)
		updatePositionBounds();

	return _maxPosition;;
}

void
VertexBuffer::invalidatePositionBounds()
{
	_validMinMax = false;
}

void
VertexBuffer::updatePositionBounds()
{
	invalidatePositionBounds();

	if (!hasAttribute(ATTRNAME_POSITION) || numVertices() == 0)
	{
		_minPosition = math::vec3(0.f);
		_maxPosition = math::vec3(0.f);
		
		return;
	}

	const auto&			xyzAttr = attribute(ATTRNAME_POSITION);
	const unsigned int	size	= std::max(0u, std::min(3u, xyzAttr.size));
	const unsigned int	offset	= xyzAttr.offset;

	float minXYZ[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
	float maxXYZ[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

	unsigned int		vidx	= offset;
	while (vidx < _data.size())
	{
		for (unsigned int k = 0; k < size; ++k)
		{
			const float vk = _data[vidx + k];

			minXYZ[k] = std::min(minXYZ[k], vk);
			maxXYZ[k] = std::max(maxXYZ[k], vk);
		}

		vidx += _vertexSize;
	}

	_minPosition = math::vec3(minXYZ[0], minXYZ[1], minXYZ[2]);
	_maxPosition = math::vec3(maxXYZ[0], maxXYZ[1], maxXYZ[2]);
	_validMinMax = true;
}

math::vec3
VertexBuffer::centerPosition()
{
	return (_minPosition + _maxPosition) * .5f;
}
