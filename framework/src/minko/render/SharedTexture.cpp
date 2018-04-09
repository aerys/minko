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

#include "minko/render/AbstractContext.hpp"
#include "minko/render/SharedTexture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
# include "minko/render/AndroidSharedTextureImpl.hpp"
#endif

using namespace minko;
using namespace minko::render;

SharedTexture::SharedTexture(AbstractContext::Ptr    context,
                             uint                    width,
                             uint                    height,
                             TextureFormat           format,
                             const std::string&      filename) :
    AbstractTexture(
        TextureType::SharedTexture,
        context, 
        width, 
        height, 
        format, 
        false, 
        false, 
        false, 
        filename
    ),
    _sharedTextureImpl(nullptr)
{
#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    _sharedTextureImpl = std::make_shared<AndroidSharedTextureImpl>();
#endif
}

void
SharedTexture::resize(unsigned int width, unsigned int height, bool resizeSmoothly)
{
    const auto previousWidth = this->width();
    const auto previousHeight = this->height();

    _width = width;
    _widthGPU = width;

    _height = height;
    _heightGPU = height;
}

void
SharedTexture::upload()
{
    if (_id != -1)
        return;

    _id = _context->createTexture(_type, _widthGPU, _heightGPU, false);
}

void
SharedTexture::dispose()
{
    if (_id != -1)
    {
        _context->deleteTexture(_id);
        _id = -1;
    }

    disposeData();
}

void
SharedTexture::disposeData()
{
}