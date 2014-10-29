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

#include "minko/log/Logger.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

using namespace minko;
using namespace minko::render;

Texture::Texture(AbstractContext::Ptr    context,
                 uint                    width,
                 uint                    height,
                 bool                    mipMapping,
                 bool                    optimizeForRenderToTexture,
                 bool                    resizeSmoothly,
                 const std::string&        filename) :
    AbstractTexture(TextureType::Texture2D, context, width, height, mipMapping, optimizeForRenderToTexture, resizeSmoothly, filename),
    _data()
{
}

void
Texture::data(unsigned char*    data,
              TextureFormat     format,
              int               widthGPU,
              int               heightGPU)
{
    _format = format;

    if (widthGPU >= 0)
    {
        if (widthGPU > (int)MAX_SIZE)
            throw std::invalid_argument("widthGPU");

        _width        = widthGPU;
        _widthGPU    = widthGPU;
    }
    if (heightGPU >= 0)
    {
        if (heightGPU > (int)MAX_SIZE)
            throw std::invalid_argument("heightGPU");

        _height        = heightGPU;
        _heightGPU    = heightGPU;
    }

    assert(math::isp2(_widthGPU) && math::isp2(_heightGPU));

    if (!TextureFormatInfo::isCompressed(_format))
    {
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
                rgba[j] = data[i];
                rgba[j + 1] = data[i + 1];
                rgba[j + 2] = data[i + 2];
                rgba[j + 3] = std::numeric_limits<unsigned char>::max();
            }
        }

        resizeData(_width, _height, rgba, _widthGPU, _heightGPU, _resizeSmoothly, _data);
    }
    else
    {
        const auto size = _width * _height * TextureFormatInfo::numBitsPerPixel(_format) / 8;
        _data.resize(size);

        std::memcpy(_data.data(), data, size);
    }
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
        if (TextureFormatInfo::isCompressed(_format))
        {
            _context->uploadCompressedTexture2dData(
                _id,
                _format,
                _widthGPU,
                _heightGPU,
                _data.size(),
                0,
                _data.data()
            );
        }
        else
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
}

void
Texture::uploadMipLevel(uint            level,
                        unsigned char*  data)
{
    const auto width = getMipmapWidth(level);
    const auto height = getMipmapHeight(level);

    if (TextureFormatInfo::isCompressed(_format))
    {
        const auto size = std::max(
            TextureFormatInfo::mipLevelMinSize(_format),
            static_cast<int>(TextureFormatInfo::numBitsPerPixel(_format) / 8.0f * width * height)
        );

        _context->uploadCompressedTexture2dData(
            _id,
            _format,
            width,
            height,
            size,
            level,
            data
        );
    }
    else
    {
        _context->uploadTexture2dData(
            _id,
            width,
            height,
            level,
            data
        );
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

    disposeData();
}

void
Texture::disposeData()
{
    _data.clear();
    _data.shrink_to_fit();
}
