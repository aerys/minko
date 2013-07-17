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

#include "Texture.hpp"

#include "minko/render/AbstractContext.hpp"

using namespace minko::render;

Texture::Texture(std::shared_ptr<render::AbstractContext>	context,
				 const unsigned int							width,
				 const unsigned int							height,
                 bool                                       optimizeForRenderToTexture) :
	AbstractResource(context),
	_width(width),
	_height(height),
    _optimizeForRenderToTexture(optimizeForRenderToTexture)
{
}

void
Texture::data(unsigned char* data, DataFormat format)
{
	auto size = _width * _height * sizeof(int);

	_data.resize(size);
	
	if (format == DataFormat::RGBA)
	{
		std::memcpy(&_data[0], data, size);
	}
	else if (format == DataFormat::RGB)
	{
		for (unsigned int i = 0, j = 0; j < size; i += 3, j += 4)
		{
			unsigned char r = data[i];
			unsigned char g = data[i + 1];
			unsigned char b = data[i + 2];

			_data[j] = data[i];
			_data[j + 1] = data[i + 1];
			_data[j + 2] = data[i + 2];
			_data[j + 3] = 0;
		}
	}
}

void
Texture::upload()
{
    if (_id == -1)
    	_id = _context->createTexture(_width, _height, false, _optimizeForRenderToTexture);
	
    if (_data.size())
        _context->uploadTextureData(_id, _width, _height, 0, &_data[0]);
}

void
Texture::dispose()
{
    if (_id != -1)
    {
	    _context->deleteTexture(_id);
	    _id = -1;
    }
}