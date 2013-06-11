#include "IndexStream.hpp"
#include "minko/render/context/AbstractContext.hpp"

using namespace minko::render::stream;

void
IndexStream::upload()
{
	_buffer = _context->createIndexBuffer(_data.size());
	_context->uploaderIndexBufferData(_buffer, 0, _data.size(), &_data[0]);
}