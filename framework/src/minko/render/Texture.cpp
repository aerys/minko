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

#include "minko/render/Texture.hpp"

#include "minko/render/AbstractContext.hpp"

using namespace minko;
using namespace minko::render;

Texture::Texture(AbstractContext::Ptr	context,
				 uint					width,
				 uint					height,
				 bool					mipMapping,
				 bool					optimizeForRenderToTexture,
				 bool					resizeSmoothly,
				 const std::string&		filename) :
	AbstractTexture(TextureType::Texture2D, context, width, height, mipMapping, optimizeForRenderToTexture, resizeSmoothly, filename),
	_data()
{
}

void
Texture::data(unsigned char*	data,
			  TextureFormat		format,
			  int				widthGPU,
			  int				heightGPU)
{
	if (widthGPU >= 0)
	{
		if (widthGPU > (int)MAX_SIZE)
			throw std::invalid_argument("widthGPU");

		_width		= widthGPU;
		_widthGPU	= widthGPU;
	}
	if (heightGPU >= 0)
	{
		if (heightGPU > (int)MAX_SIZE)
			throw std::invalid_argument("heightGPU");

		_height		= heightGPU;
		_heightGPU	= heightGPU;
	}

	const auto size = _width * _height * sizeof(int);

	std::vector<unsigned char> rgba(size, 0);

	if (format == TextureFormat::RGBA)
	{
		std::memcpy(&rgba[0], data, size);
	}
	else if (format == TextureFormat::RGB)
	{
		for (unsigned int i = 0, j = 0; j < size; i += 3, j += 4)
		{
			rgba[j]		= data[i];
			rgba[j + 1] = data[i + 1];
			rgba[j + 2] = data[i + 2];
			rgba[j + 3] = std::numeric_limits<unsigned char>::max();
		}
	}

	assert(math::isp2(_widthGPU) && math::isp2(_heightGPU));
	resizeData(_width, _height, rgba, _widthGPU, _heightGPU, _resizeSmoothly, _data);
}

void
Texture::upload()
{
    if (_id == -1)
    	_id = _context->createTexture(
			_type,
			_widthGPU,
			_heightGPU,
			_mipMapping,
			_optimizeForRenderToTexture
		);

    if (!_data.empty())
    {
        _context->uploadTexture2dData(
			_id,
			_widthGPU,
			_heightGPU,
			0,
			&_data.front()
		);

        if (_mipMapping)
            _context->generateMipmaps(_id);
    }
}

void
Texture::uploadMipLevel(uint			level,
						unsigned char*	data)
{
	_context->uploadTexture2dData(
		_id,
		getMipmapWidth(level),
		getMipmapHeight(level),
		level,
		data
	);
}

void
Texture::dispose()
{
    if (_id != -1)
    {
	    _context->deleteTexture(_id);
	    _id = -1;
    }

	disposeData();
}

void
Texture::disposeData()
{
	_data.clear();
	_data.shrink_to_fit();
}
