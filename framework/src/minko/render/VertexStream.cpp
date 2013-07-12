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

using namespace minko::render;
using namespace minko::render;

VertexStream::VertexStream(std::shared_ptr<AbstractContext> context) :
	AbstractResource(context)
{
}

VertexStream::VertexStream(std::shared_ptr<AbstractContext>	context,
						   float*							data,
						   const unsigned int				size,
						   const unsigned int				offset) :
	AbstractResource(context),
	_data(data + offset, data + offset + size)
{
	upload();
}

VertexStream::VertexStream(std::shared_ptr<AbstractContext>		context,
						   std::vector<float>::const_iterator	begin,
						   std::vector<float>::const_iterator	end) :
	AbstractResource(context),
	_data(begin, end)
{
	upload();
}

VertexStream::VertexStream(std::shared_ptr<AbstractContext> context, float* begin, float* end) :
	AbstractResource(context),
	_data(begin, end)
{
	upload();
}

void
VertexStream::upload()
{
	if (_id != -1)
		_context->deleteVertexBuffer(_id);

	_id = _context->createVertexBuffer(_data.size());
	if (_id < 0)
		throw;
	
	_context->uploadVertexBufferData(_id, 0, _data.size(), &_data[0]);
}

void
VertexStream::dispose()
{
	_context->deleteVertexBuffer(_id);
	_id = -1;
}

void
VertexStream::addAttribute(const std::string& 	name,
						   const unsigned int	size,
						   const unsigned int	offset)
{
	if (hasAttribute(name))
		throw std::invalid_argument("name");

	_attributes.push_back(VertexStream::AttributePtr(
		new VertexStream::Attribute(name, size, offset)
	));
}

bool
VertexStream::hasAttribute(const std::string& attributeName)
{
	for (auto& attr : _attributes)
		if (std::get<0>(*attr) == attributeName)
			return true;

	return false;
}

VertexStream::AttributePtr
VertexStream::attribute(const std::string& attributeName)
{
	for (auto& attr : _attributes)
		if (std::get<0>(*attr) == attributeName)
			return attr;

	throw std::invalid_argument("attributeName");
}
