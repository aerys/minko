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

using namespace minko;
using namespace minko::render;

Texture::Texture(std::shared_ptr<render::AbstractContext>	context,
				 const unsigned int							width,
				 const unsigned int							height,
                 bool                                       mipMapping,
                 bool                                       optimizeForRenderToTexture) :
	AbstractResource(context),
	_width(width),
	_height(height),
	_widthGPU(math::clp2(width)),
	_heightGPU(math::clp2(height)),
    _mipMapping(mipMapping),
    _optimizeForRenderToTexture(optimizeForRenderToTexture)
{
}

void
Texture::data(unsigned char* data, DataFormat format)
{
	const auto size = _width * _height * sizeof(int);

	std::vector<unsigned char> rgba(size, 0);
	
	if (format == DataFormat::RGBA)
	{
		std::memcpy(&rgba[0], data, size);
	}
	else if (format == DataFormat::RGB)
	{
		for (unsigned int i = 0, j = 0; j < size; i += 3, j += 4)
		{
			rgba[j]		= data[i];
			rgba[j + 1] = data[i + 1];
			rgba[j + 2] = data[i + 2];
			rgba[j + 3] = UCHAR_MAX;
		}
	}

	processData(rgba, _data);
}

void
Texture::processData(std::vector<unsigned char>&	inData, 
					 std::vector<unsigned char>&	outData) const
{
#ifdef DEBUG_TEXTURE
	assert(inData.size() == _width * _height * sizeof(int));
#endif // DEBUG_TEXTURE

	outData.clear();

	if (inData.empty() || _widthGPU == 0 || _heightGPU == 0)
		return;

	if (_widthGPU == _width && _heightGPU == _height)
	{
		outData.swap(inData);
		return;
	}

	const auto	size	= _widthGPU * _heightGPU * sizeof(int);
	const float	xFactor = ((float)_width - 1.0f)/((float)_widthGPU - 1.0f); 
	const float	yFactor = ((float)_height - 1.0f)/((float)_heightGPU - 1.0f);

	outData.resize(size);

	uint	idx	= 0;
	float	y	= 0.0f;
	for (uint q = 0; q < _heightGPU; ++q)
	{
		uint		j	= (uint) floorf(y);
		const float dy	= y - (float)j;
		if (j >= _height)
			j = _height - 1;

		float		x	= 0.0f;
		for (uint p = 0; p < _widthGPU; ++p)
		{
			uint		i	= (uint)floorf(x);
			const float dx	= x - (float)i;
			const float dxy = dx * dy;
			if (i >= _width)
				i = _width - 1;

			const uint ijTL	= (i + _width * j) << 2;
			const uint ijTR	= i < _width - 1						? ijTL + 4						: ijTL;
			const uint ijBL = j < _height - 1						? ijTL + (_width << 2)			: ijTL;
			const uint ijBR = (i < _width - 1) && (j < _height - 1)	? ijTL + ((_width + 1) << 2)	: ijTL;

			const float	wTL	= 1.0 - dx - dy + dxy; 
			const float wTR = dx - dxy;
			const float wBL = dy - dxy;
			const float wBR = dxy;
			
			for (uint k = 0; k < 4; ++k)
			{
				const float color = wTL * inData[ijTL + k] + 
					wTR * inData[ijTR + k] + 
					wBL * inData[ijBL + k] + 
					wBR * inData[ijBR + k];

				outData[idx + k] = (unsigned char)floorf(color);
			}

			idx	+= 4;
			x	+= xFactor;
		}
		y += yFactor;
	}


#ifdef DEBUG_TEXTURE
	assert(outData.size() == _widthGPU * _heightGPU * sizeof(int));
#endif // DEBUG_TEXTURE
}

void
Texture::upload()
{
    if (_id == -1)
    	_id = _context->createTexture(_widthGPU, _heightGPU, _mipMapping, _optimizeForRenderToTexture);
	
    if (_data.size())
    {
        _context->uploadTextureData(_id, _widthGPU, _heightGPU, 0, &_data[0]);
        if (_mipMapping)
            _context->generateMipmaps(_id);
    }
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
