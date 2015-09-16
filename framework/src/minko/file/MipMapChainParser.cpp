/*
Copyright (c) 2015 Aerys

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

#include "minko/file/MipMapChainParser.hpp"

#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::render;

MipMapChainParser::MipMapChainParser()
{
}

MipMapChainParser::~MipMapChainParser()
{
}

Texture::Ptr
MipMapChainParser::parseTexture(std::shared_ptr<AbstractContext>   context,
                                uint             		           width,
                                uint             		           height,
                                const unsigned char* 	           data,
                                bool                               parseMipMaps,
                                bool                               mipMapping,
                                bool                               smooth,
                                TextureFormat                      format,
                                const std::string&                 filename)
{
    int numLevels = mipMapping && parseMipMaps ? std::ceil(math::log2(float(width))) : 1;
    int actualHeight = height;

    if (mipMapping && parseMipMaps && width * 2 - 1 != height)
    {
        int diff = height - width * 2 - 1;

        actualHeight = height + (diff + 1) / 2;
    }

    uint bytesPerPixel = format == TextureFormat::RGB ? 3 : 4;
    std::vector<unsigned char> rgba(width * actualHeight * sizeof(int), 0);
    Texture::Ptr texture = Texture::create(
        context,
        width,
        actualHeight,
        mipMapping,
        false,
        smooth,
        TextureFormat::RGBA,
        filename
    );

    // FIXME: offset data to start parsing where width < MAX_SIZE

    parseMipMap(&rgba[0], data, width, height, math::ivec2(0), width, actualHeight, bytesPerPixel);
    texture->data(&rgba[0]);
    texture->upload();

    if (mipMapping && parseMipMaps)
    {
        uint dataOffset = width * actualHeight * bytesPerPixel;

        for (uint level = 1; level < numLevels; level++)
        {
            // incomplete mipmap chain
            if (dataOffset > width * height * bytesPerPixel)
                break;

            parseMipMap(&rgba[0], data + dataOffset, width, height, math::ivec2(0), width >> level, actualHeight >> level, bytesPerPixel);

            dataOffset += width * (actualHeight >> level) * bytesPerPixel;
            texture->uploadMipLevel(level, &rgba[0]);
        }
    }

    return texture;
}

CubeTexture::Ptr
MipMapChainParser::parseCubeTexture(std::shared_ptr<AbstractContext>   context,
                                    uint             		           width,
                                    uint             		           height,
                                    const unsigned char* 	           data,
                                    bool                               parseMipMaps,
                                    bool                               mipMapping,
                                    bool                               smooth,
                                    TextureFormat                      format,
                                    const std::string&                 filename)
{
    int faceSize = width / 4;

    CubeTexture::Ptr texture = CubeTexture::create(
        context,
        faceSize,
        faceSize,
        mipMapping,
        false,
        smooth,
        TextureFormat::RGBA,
        filename
    );

    std::array<CubeTexture::Face, 6> faces = {
        CubeTexture::Face::POSITIVE_X,
        CubeTexture::Face::NEGATIVE_X,
        CubeTexture::Face::POSITIVE_Y,
        CubeTexture::Face::NEGATIVE_Y,
        CubeTexture::Face::POSITIVE_Z,
        CubeTexture::Face::NEGATIVE_Z
    };

    // horizontal cross layout
    std::array<math::ivec2, 6> faceOffset = {
        math::ivec2(2, 1), // positive x
        math::ivec2(0, 1), // negative x
        math::ivec2(1, 0), // positive y
        math::ivec2(1, 2), // negative y
        math::ivec2(1, 1), // positive z
        math::ivec2(3, 1)  // negative z
    };

    std::vector<unsigned char> rgba(faceSize * faceSize * sizeof(int), 0);
    auto bytesPerPixel = format == TextureFormat::RGBA ? 4 : 3;

    for (auto i = 0; i < 6; ++i)
    {
        auto offset = faceOffset[i] * faceSize;
        auto face = faces[i];

        this->parseMipMap(&rgba[0], data, width, height, offset, faceSize, faceSize, bytesPerPixel);
        texture->data(&rgba[0], face);
    }
    texture->upload();

    auto eof = width * height * bytesPerPixel;

    if (mipMapping && parseMipMaps)
        for (auto i = 0; i < 6; ++i)
        {
            auto face = faces[i];
    		uint level = 1;
            uint dataOffset = width * faceSize * 3 * bytesPerPixel;

            for (int size = faceSize / 2; size >= 1; size /= 2, ++level)
            {
                auto offset = faceOffset[i] * size;

                // incomplete mipmap chain
                if (dataOffset >= eof)
                    break;

                this->parseMipMap(&rgba[0], data + dataOffset, width, height, offset, size, size, bytesPerPixel);

                // uint s = math::clp2(size);
                //
                // if (s != size)
                // {
                //     std::vector<unsigned char> resized(s * s * sizeof(int));
                //
                //     AbstractTexture::resizeData(size, size, &mipMapData[0], s, s, smooth, resized);
                //     // mipMapData = std::move(resized);
                //     mipMapData = resized;
                // }

                texture->uploadMipLevel(level, &rgba[0], face);

                dataOffset += width * size * 3 * bytesPerPixel;
            }
        }

    return texture;
}

void
MipMapChainParser::parseMipMap(unsigned char*       out,
                               const unsigned char* data,
                               uint                 width,
                               uint                 height,
                               math::ivec2          offset,
                               uint                 mipMapWidth,
                               uint                 mipMapHeight,
                               uint                 bytesPerPixel)
{
    const uint endX = offset.x + mipMapWidth;
    const uint endY = offset.y + mipMapHeight;
    uint idx = 0;

    for (uint y = offset.y; y < endY; ++y)
    {
        for (uint x = offset.x; x < endX; ++x)
        {
            uint xy = (x + y * width) * bytesPerPixel;

            for (uint i = 0; i < bytesPerPixel; ++i)
                out[idx++] = data[xy++];
            for (uint i = bytesPerPixel; i < 4u; ++i)
                out[idx++] = std::numeric_limits<unsigned char>::max();
        }
    }
}
