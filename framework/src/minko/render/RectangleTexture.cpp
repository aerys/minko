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

#include "minko/render/RectangleTexture.hpp"

#include "minko/render/AbstractContext.hpp"

#include "minko/log/Logger.hpp"

using namespace minko;
using namespace minko::render;

RectangleTexture::RectangleTexture(AbstractContext::Ptr    context,
                                   uint                    width,
                                   uint                    height,
                                   TextureFormat           format,
                                   const std::string&      filename) :
    AbstractTexture(TextureType::Texture2D, context, width, height, format, false, false, false, filename),
    _data()
{
}

void
RectangleTexture::data(unsigned char*    data,
                       int               widthGPU,
                       int               heightGPU)
{
    if (widthGPU >= 0)
    {
        if (widthGPU > (int)MAX_SIZE)
            throw std::invalid_argument("widthGPU");

        _width              = widthGPU;
        _widthGPU           = widthGPU;
    }
    if (heightGPU >= 0)
    {
        if (heightGPU > (int)MAX_SIZE)
            throw std::invalid_argument("heightGPU");

        _height             = heightGPU;
        _heightGPU          = heightGPU;
    }

    const auto size = _width * _height * sizeof(int);

    _data.resize(size);

    if (_format == TextureFormat::RGBA)
    {
        std::memcpy(&_data[0], data, size);
    }
    else if (_format == TextureFormat::RGB)
    {
        for (unsigned int i = 0, j = 0; j < size; i += 3, j += 4)
        {
            _data[j]        = data[i];
            _data[j + 1] = data[i + 1];
            _data[j + 2] = data[i + 2];
            _data[j + 3] = std::numeric_limits<unsigned char>::max();
        }
    }
}

void
RectangleTexture::resize(unsigned int width, unsigned int height, bool resizeSmoothly)
{
    const auto previousWidth = this->width();
    const auto previousHeight = this->height();

    auto previousData = _data;

    resizeData(previousWidth, previousHeight, &previousData[0], width, height, resizeSmoothly, _data);

    _width = width;
    _widthGPU = width;

    _height = height;
    _heightGPU = height;
}

void
RectangleTexture::upload()
{
    if (_id == -1)
        _id = _context->createRectangleTexture(
            _type,
            _widthGPU,
            _heightGPU
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
    }
}

void
RectangleTexture::dispose()
{
    if (_id != -1)
    {
        _context->deleteTexture(_id);
        _id = -1;
    }

    disposeData();
}

void
RectangleTexture::disposeData()
{
    _data.clear();
    _data.shrink_to_fit();
}
