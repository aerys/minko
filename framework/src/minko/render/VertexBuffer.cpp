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

#include "minko/render/VertexBuffer.hpp"

#include "minko/Signal.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/VertexAttribute.hpp"

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
						   const float*						data,
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
    	&_data[offset * _vertexSize]
    );

	//updatePositionBounds();
}

void
VertexBuffer::upload(uint offset, uint numVertices, const std::vector<float>& data)
{
    if (data.empty())
        return;

    if (_id == -1)
        _id = _context->createVertexBuffer(data.size());

    _context->uploadVertexBufferData(
        _id,
        offset * _vertexSize,
        numVertices == 0 ? data.size() : numVertices * _vertexSize,
        const_cast<float*>(data.data())
    );
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

    auto actualOffset = offset;
    if (actualOffset == 0)
        actualOffset = _vertexSize;

	_attributes.push_back({ &_id, &_vertexSize, name, size, actualOffset });

	vertexSize(_vertexSize + size);
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
	auto it = std::find_if(_attributes.begin(), _attributes.end(), [&](const VertexAttribute& attr)
	{
		return attr.name == attributeName;
	});

	if (it == _attributes.end())
		throw std::invalid_argument("attributeName = " + attributeName);

	vertexSize(_vertexSize - it->size);
    _attributes.erase(it);
}

const VertexAttribute&
VertexBuffer::attribute(const std::string& attributeName) const
{
    auto it = std::find_if(_attributes.begin(), _attributes.end(), [&](const VertexAttribute& attr)
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