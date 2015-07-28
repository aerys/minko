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

#include "minko/render/AbstractTexture.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/TextureFormat.hpp"

using namespace minko;
using namespace minko::render;

const uint AbstractTexture::MAX_SIZE = 4096;

AbstractTexture::AbstractTexture(TextureType			type,
								AbstractContext::Ptr	context,
								unsigned int			width,
								unsigned int			height,
                                TextureFormat           format,
								bool					mipMapping,
								bool					optimizeForRenderToTexture,
                                bool					resizeSmoothly,
								const std::string&		filename) :
	AbstractResource(context),
    _sampler(uuid(), &_id),
	_type(type),
    _format(format),
	_width(width),
	_height(height),
	_widthGPU(std::min(math::clp2(width), MAX_SIZE)),
	_heightGPU(std::min(math::clp2(height), MAX_SIZE)),
    _mipMapping(mipMapping),
	_resizeSmoothly(resizeSmoothly),
	_optimizeForRenderToTexture(optimizeForRenderToTexture),
	_filename(filename)
{
}

/*static*/
void
AbstractTexture::resizeData(unsigned int 				width,
						    unsigned int 				height,
							unsigned char* 				data,
							unsigned int 				newWidth,
							unsigned int 				newHeight,
							bool 						resizeSmoothly,
							std::vector<unsigned char>&	newData)
{
	newData.clear();

	if (newWidth == 0 || newHeight == 0)
		return;

	if (newWidth == width && newHeight == height)
	{
		newData.resize(width * height * sizeof(int));
		std::memcpy(&newData[0], data, width * height * sizeof(int));
		return;
	}

	const auto	size = newWidth * newHeight * sizeof(int);
	const float	xFactor = ((float)width - 1.0f)/((float)newWidth - 1.0f);
	const float	yFactor = ((float)height - 1.0f)/((float)newHeight - 1.0f);

	newData.resize(size);

	uint idx = 0;
	float y = 0.0f;
	for (uint q = 0; q < newHeight; ++q)
	{
		uint j = (uint) floorf(y);
		const float dy = y - (float)j;

		if (j >= height)
			j = height - 1;

		float x	= 0.0f;
		for (uint p = 0; p < newWidth; ++p)
		{
			uint i = (uint)floorf(x);

			if (i >= width)
				i = width - 1;

			const uint ijTL	= (i + width * j) << 2;

			if (resizeSmoothly)
			{
				// bilinear interpolation

				const float dx	= x - (float)i;
				const float dxy = dx * dy;

				const uint ijTR	= i < width - 1 ? ijTL + 4 : ijTL;
				const uint ijBL = j < height - 1 ? ijTL + (width << 2) : ijTL;
				const uint ijBR = (i < width - 1) && (j < height - 1) ? ijTL + ((width + 1) << 2) : ijTL;

				const float	wTL	= 1.0f - dx - dy + dxy;
				const float wTR = dx - dxy;
				const float wBL = dy - dxy;
				const float wBR = dxy;

				for (uint k = 0; k < 4; ++k)
				{
					const float color = wTL * data[ijTL + k] +
						wTR * data[ijTR + k] +
						wBL * data[ijBL + k] +
						wBR * data[ijBR + k];

					newData[idx + k] = (unsigned char)floorf(color);
				}
			}
			else
			{
				// nearest pixel color

				for (uint k = 0; k < 4; ++k)
					newData[idx + k] = data[ijTL + k];
			}

			idx	+= 4;
			x += xFactor;
		}
		y += yFactor;
	}

#ifdef DEBUG_TEXTURE
	assert(newData.size() == newWidth * newHeight * sizeof(int));
#endif // DEBUG_TEXTURE
}

void
AbstractTexture::activateMipMapping()
{
    if (_mipMapping)
        return;

    _mipMapping = true;

    _context->activateMipMapping(_id);
}

uint
AbstractTexture::getMipmapWidth(uint level) const
{
	assert(math::isp2(_widthGPU));

	const uint p = math::getp2(_widthGPU);
	return 1 << (p - level);
	// return uint(powf(2.0f, (log2f(_widthGPU) - level)))
}

uint
AbstractTexture::getMipmapHeight(uint level) const
{
	assert(math::isp2(_heightGPU));

	const uint p = math::getp2(_heightGPU);

	return 1 << (p - level);
}
