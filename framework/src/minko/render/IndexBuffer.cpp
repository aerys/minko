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

#include "minko/render/IndexBuffer.hpp"

#include "minko/render/AbstractContext.hpp"

using namespace minko;
using namespace minko::render;



void
IndexBuffer::upload(uint	offset, 
					int		count)
{   
	assert(!_data.empty() && count <= (int)_data.size());

	if (_id == -1)
    	_id = _context->createIndexBuffer(_data.size());
	
	const auto oldNumIndices	= _numIndices;
	_numIndices					= count >= 0 ? count : _data.size();

	_context->uploaderIndexBufferData(
		_id, 
		offset, 
		_numIndices, 
		&_data[0]
	);

	if (_numIndices != oldNumIndices)
		_changed->execute(shared_from_this());
}

void
IndexBuffer::dispose()
{
    if (_id != -1)
	    _context->deleteIndexBuffer(_id);

	_id			= -1;
	_numIndices	= 0;

	_changed->execute(shared_from_this());
}
