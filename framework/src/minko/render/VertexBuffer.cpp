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
#include "minko/math/Vector3.hpp"

using namespace minko;
using namespace minko::math;
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
    if (_id == -1)
    	_id = _context->createVertexBuffer(_data.size());

    _context->uploadVertexBufferData(
    	_id,
    	offset * _vertexSize,
    	numVertices == 0 ? _data.size() : numVertices * _vertexSize,
    	&_data[0]
    );

	updatePositionBounds();
}

void
VertexBuffer::dispose()
{
    if (_id != -1)
    {
	    _context->deleteVertexBuffer(_id);
	    _id = -1;
    }

	_data.clear();
	// _attributes.clear();
	// _vertexSize	= 0;
}

void
VertexBuffer::addAttribute(const std::string& 	name,
						   const unsigned int	size,
						   const unsigned int	offset)
{
	if (hasAttribute(name))
		throw std::invalid_argument("name");

	_attributes.push_back(
		VertexBuffer::AttributePtr(new VertexBuffer::Attribute(name, size, offset == 0 ? _vertexSize : offset))
	);

	vertexSize(_vertexSize + size);

	if (name == ATTRNAME_POSITION)
		invalidatePositionBounds();
}

bool
VertexBuffer::hasAttribute(const std::string& attributeName) const
{
	auto it = std::find_if(_attributes.begin(), _attributes.end(), [&](AttributePtr attr)
	{
		return std::get<0>(*attr) == attributeName;
	});

	return it != _attributes.end();
}

void
VertexBuffer::removeAttribute(const std::string& attributeName)
{
	auto it = std::find_if(_attributes.begin(), _attributes.end(), [&](AttributePtr attr)
	{
		return std::get<0>(*attr) == attributeName;
	});

	if (it == _attributes.end())
		throw std::invalid_argument("attributeName = " + attributeName);

	vertexSize(_vertexSize - std::get<1>(**it));

	if (attributeName == ATTRNAME_POSITION)
		invalidatePositionBounds();
}

VertexBuffer::AttributePtr
VertexBuffer::attribute(const std::string& attributeName) const
{
	for (auto& attr : _attributes)
		if (std::get<0>(*attr) == attributeName)
			return attr;

	throw std::invalid_argument("attributeName = " + attributeName);
}

void
VertexBuffer::vertexSize(unsigned int value)
{
	int offset = value - _vertexSize;

	_vertexSize = value;
	_vertexSizeChanged->execute(shared_from_this(), offset);
}


Vector3::Ptr
VertexBuffer::minPosition(Vector3::Ptr output)
{
	if (_minPosition == nullptr)
		updatePositionBounds();

	if (output == nullptr)
		output = Vector3::create();

	return _minPosition
		? output->copyFrom(_minPosition)
		: output->setTo(0.0f, 0.0f, 0.0f);
}

Vector3::Ptr
VertexBuffer::maxPosition(Vector3::Ptr output)
{
	if (_maxPosition == nullptr)
		updatePositionBounds();

	if (output == nullptr)
		output = Vector3::create();

	return _maxPosition
		? output->copyFrom(_maxPosition)
		: output->setTo(0.0f, 0.0f, 0.0f);
}

void
VertexBuffer::invalidatePositionBounds()
{
	_minPosition = nullptr;
	_maxPosition = nullptr;
}

void
VertexBuffer::updatePositionBounds()
{
	invalidatePositionBounds();

	if (!hasAttribute(ATTRNAME_POSITION) || numVertices() == 0)
		return;

	auto				xyzAttr = attribute(ATTRNAME_POSITION);
	const unsigned int	size	= std::max(0, std::min(3, (int)std::get<1>(*xyzAttr)));
	const unsigned int	offset	= std::get<2>(*xyzAttr);

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

	_minPosition = Vector3::create(minXYZ[0], minXYZ[1], minXYZ[2]);
	_maxPosition = Vector3::create(maxXYZ[0], maxXYZ[1], maxXYZ[2]);
}

Vector3::Ptr
VertexBuffer::centerPosition(Vector3::Ptr output)
{
	if (output == nullptr)
		output = Vector3::create();
	
	return output->copyFrom(minPosition())->add(maxPosition())->scaleBy(0.5f);
}