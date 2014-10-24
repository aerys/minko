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

#include "minko/file/TextureContainer.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/Texture.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::render;

#define MINKO_TEX_MAGIC_NUMBER 0x00544558

bool
TextureContainer::load(const std::vector<unsigned char>&  in,
                       std::vector<unsigned char>&        out,
                       unsigned int&                      width,
                       unsigned int&                      height,
                       unsigned int&                      size,
                       render::TextureType&               textureType,
                       render::TextureFormat&             textureFormat)
{
    const Header header = *reinterpret_cast<const Header*>(in.data());

    const auto headerSize = sizeof(header);

    if (header._magicNumber != MINKO_TEX_MAGIC_NUMBER)
    {
        LOG_ERROR("wrong texture container magic number");

        return false;
    }

    width = header._width;
    height = header._height;

    size = header._size;

    out.resize(size, 0);

    textureFormat = static_cast<TextureFormat>(header._pixelFormat);

    if (header._numFaces == 1)
    {
        textureType = TextureType::Texture2D;

        // TODO fixme
        // handle mipmaps

        std::copy(
            in.begin() + headerSize,
            in.end(),
            out.begin()
        );
    }
    else
    {
        // TODO fixme
        // handle CubeTexture
        // handle mipmaps

        return false;
    }

    return true;
}

bool
TextureContainer::save(AbstractTexture::Ptr                 texture,
                       TextureFormat                        format,
                       const std::vector<unsigned char>&    in,
                       std::vector<unsigned char>&          out)
{
    auto header = Header();

    const auto headerSize = sizeof(header);
    const auto size = in.size();

    auto texture2d = std::dynamic_pointer_cast<Texture>(texture);

    if (texture2d != nullptr)
    {
        header._magicNumber = MINKO_TEX_MAGIC_NUMBER;
    
        header._width = texture2d->width();
        header._height = texture2d->height();

        header._size = size;
        header._pixelFormat = static_cast<unsigned int>(format);

        // TODO
        header._numBitsPerPixel = 0;

        header._rBitMask = 0xff000000;
        header._gBitMask = 0x00ff0000;
        header._bBitMask = 0x0000ff00;
        header._aBitMask = 0x000000ff;

        header._numFaces = 1;

        // TODO
        header._numMipMaps = 0;
    }
    else
    {
        // TODO fixme
        // handle CubeTexture

        return false;
    }

    std::ostringstream outStream;

    outStream.write(reinterpret_cast<const char*>(&header), headerSize);
    outStream.write(reinterpret_cast<const char*>(in.data()), size);

    out.resize(headerSize + size);

    const auto outStreamString = outStream.str();
    
    std::copy(
        outStreamString.begin(),
        outStreamString.end(),
        out.begin()
    );

    return true;
}
