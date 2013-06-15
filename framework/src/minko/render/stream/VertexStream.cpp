#include "VertexStream.hpp"
#include "minko/render/stream/VertexAttribute.hpp"

using namespace minko::render::stream;

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
