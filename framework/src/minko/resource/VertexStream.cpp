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

#include "VertexStream.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/resource/VertexAttribute.hpp"

using namespace minko::resource;

void
VertexStream::upload()
{
	if (_buffer != -1)
		_context->deleteVertexBuffer(_buffer);

	_buffer = _context->createVertexBuffer(_data.size());
	if (_buffer < 0)
		throw;

	_context->uploadVertexBufferData(_buffer, 0, _data.size(), &_data[0]);
}

VertexStream::VertexStream(std::shared_ptr<AbstractContext> context) :
	_context(context),
	_buffer(-1)
{
}

VertexStream::VertexStream(std::shared_ptr<AbstractContext>	context,
						   float*							data,
						   const unsigned int				size,
						   const unsigned int				offset) :
	_context(context),
	_data(data + offset, data + offset + size),
	_buffer(-1)
{
	upload();
}

VertexStream::VertexStream(std::shared_ptr<AbstractContext>		context,
						   std::vector<float>::const_iterator	begin,
						   std::vector<float>::const_iterator	end) :
	_context(context),
	_data(begin, end),
	_buffer(-1)
{
	upload();
}

VertexStream::VertexStream(std::shared_ptr<AbstractContext> context, float* begin, float* end) :
	_context(context),
	_data(begin, end),
	_buffer(-1)
{
	upload();
}

void
VertexStream::addAttribute(std::shared_ptr<VertexAttribute> attribute)
{
	if (hasAttribute(attribute))
		throw std::invalid_argument("attribute");

	_attributes.push_back(attribute);
}

bool
VertexStream::hasAttribute(std::shared_ptr<VertexAttribute> attribute)
{
	return std::find(_attributes.begin(), _attributes.end(), attribute) != _attributes.end();
}

std::shared_ptr<VertexAttribute>
VertexStream::attribute(const std::string& attributeName)
{
	for (auto attr : _attributes)
		if (attr->name() == attributeName)
			return attr;

	return nullptr;
}
