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

#include "minko/file/DDSImage.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/Texture.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::render;

#define DDSF_MAGIC              0x20534444

#define DDSF_CAPS               0x00000001
#define DDSF_HEIGHT             0x00000002
#define DDSF_WIDTH              0x00000004
#define DDSF_PITCH              0x00000008
#define DDSF_PIXELFORMAT        0x00001000
#define DDSF_MIPMAPCOUNT        0x00020000
#define DDSF_LINEARSIZE         0x00080000
#define DDSF_DEPTH              0x00800000

#define DDSF_ALPHAPIXELS        0x00000001
#define DDSF_FOURCC             0x00000004
#define DDSF_RGB                0x00000040
#define DDSF_RGBA               0x00000041

#define DDSF_COMPLEX            0x00000008
#define DDSF_TEXTURE            0x00001000
#define DDSF_MIPMAP             0x00400000

#define DDSF_CUBEMAP            0x00000200
#define DDSF_CUBEMAP_POSITIVEX  0x00000400
#define DDSF_CUBEMAP_NEGATIVEX  0x00000800
#define DDSF_CUBEMAP_POSITIVEY  0x00001000
#define DDSF_CUBEMAP_NEGATIVEY  0x00002000
#define DDSF_CUBEMAP_POSITIVEZ  0x00004000
#define DDSF_CUBEMAP_NEGATIVEZ  0x00008000
#define DDSF_CUBEMAP_ALL_FACES  0x0000FC00
#define DDSF_VOLUME             0x00200000

#define FOURCC_DXT1             0x31545844
#define FOURCC_DXT3             0x33545844
#define FOURCC_DXT5             0x35545844

DDSImage::DDSImage()
{
}

bool
DDSImage::load(const std::vector<unsigned char>& src,
               std::vector<unsigned char>& dst,
               unsigned int& width,
               unsigned int& height,
               unsigned int& size,
               render::TextureType& textureType,
               render::TextureFormat& textureFormat)
{
    auto magicNumber = *reinterpret_cast<const unsigned int*>(src.data());

    auto srcOffset = sizeof(magicNumber);

    _header = *reinterpret_cast<const DDSHeader*>(src.data() + srcOffset);

    assert(magicNumber == DDSF_MAGIC);

    textureType = TextureType::Texture2D;

    if (_header._caps2 & DDSF_CUBEMAP)
        textureType = TextureType::CubeTexture;

    if (_header._pixelFormat._flags & DDSF_FOURCC)
    {
        switch (_header._pixelFormat._fourCC)
        {
        case FOURCC_DXT1:
            textureFormat = TextureFormat::RGB_DXT1;
            break;
        case FOURCC_DXT3:
            textureFormat = TextureFormat::RGBA_DXT3;
            break;
        case FOURCC_DXT5:
            textureFormat = TextureFormat::RGBA_DXT5;
            break;
        default:
            break;
        }
    }

    width = _header._width;
    height = _header._height;

    size = dxtcSize(width, height, textureFormat);

    auto textureCount = textureType == TextureType::CubeTexture ? 6 : 1;

    dst.resize(size * textureCount);
    
    srcOffset += _header._size;
    auto dstOffset = 0;

    for (auto i = 0; i < textureCount; ++i)
    {
        std::copy(src.begin() + srcOffset, src.begin() + srcOffset + size, dst.begin() + dstOffset);
        
        srcOffset += size;
        dstOffset += size;

        // TODO
        // handle mip levels
    }

    return true;
}

void
DDSImage::save(std::ostream& outputStream,
               AbstractTexture::Ptr texture,
               TextureFormat textureFormat,
               const std::vector<unsigned char>& compressedData)
{
    auto headerSize = sizeof(_header);

    memset(&_header, 0, headerSize);

    _header._size = headerSize;

    _header._flags = DDSF_CAPS | DDSF_WIDTH | DDSF_HEIGHT | DDSF_PIXELFORMAT;

    _header._width = texture->width();
    _header._height = texture->height();

    auto size = 0;

    auto textureData = std::vector<unsigned char>();

    if (texture->type() == TextureType::Texture2D)
    {
        auto texture2d = std::static_pointer_cast<Texture>(texture);

        size = texture2d->data().size();

        if (compressedData.size() == 0)
            textureData.assign(texture2d->data().begin(), texture2d->data().end());
        else
            textureData.assign(compressedData.begin(), compressedData.end());
    }
    else
    {
        auto cubeTexture = std::static_pointer_cast<CubeTexture>(texture);

        _header._caps1 |= DDSF_COMPLEX;
        _header._caps2 |= DDSF_CUBEMAP | DDSF_CUBEMAP_ALL_FACES;

        // TODO
        // handle cube texture
    }

    _header._flags |= DDSF_LINEARSIZE;
    _header._pitchOrLinearSize = size;

    _header._pixelFormat._size = sizeof(_header._pixelFormat);

    _header._pixelFormat._flags = DDSF_FOURCC;

    switch (textureFormat)
    {
    case TextureFormat::RGB_DXT1:
        _header._pixelFormat._fourCC |= FOURCC_DXT1;
        break;

    case TextureFormat::RGBA_DXT3:
        _header._pixelFormat._fourCC |= FOURCC_DXT3;
        break;

    case TextureFormat::RGBA_DXT5:
        _header._pixelFormat._fourCC |= FOURCC_DXT5;
        break;

    default:
        break;
    }

    _header._caps1 |= DDSF_TEXTURE;

    auto mipMapCount = 0;

    // TODO
    // handle mipmaps

    if (mipMapCount > 0)
    {
        _header._caps1 |= DDSF_COMPLEX | DDSF_MIPMAP;
    }

    unsigned int magicNumber = DDSF_MAGIC;

    outputStream.write(reinterpret_cast<const char*>(&magicNumber), sizeof(magicNumber));

    outputStream.write(reinterpret_cast<const char*>(&_header), headerSize);

    outputStream.write(reinterpret_cast<const char*>(textureData.data()), textureData.size());
}

unsigned int
DDSImage::dxtcSize(unsigned int width, unsigned int height, TextureFormat textureFormat)
{
    return ((width + 3) / 4) * ((height + 3) / 4) * (textureFormat == TextureFormat::RGB_DXT1 ? 8 : 16);
}
