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

#include "minko/render/CubeTexture.hpp"

#include "minko/render/AbstractContext.hpp"

using namespace minko;
using namespace minko::render;

CubeTexture::CubeTexture(AbstractContext::Ptr    context,
                         unsigned int            width,
                         unsigned int            height,
                         TextureFormat           format,
                         bool                    mipMapping,
                         bool                    optimizeForRenderToTexture,
                         bool                    resizeSmoothly,
                         const std::string&      filename) :
    AbstractTexture(
        TextureType::CubeTexture,
        context,
        width,
        height,
        format,
        mipMapping,
        optimizeForRenderToTexture,
        resizeSmoothly,
        filename
    ),
    _data(std::vector<std::vector<unsigned char>>(6))
{
    // keep only the GPU relevant size of each face
    _widthGPU    = std::min(math::clp2(width >> 2), MAX_SIZE);
    _heightGPU    = std::min(math::clp2(height / 3), MAX_SIZE);
}

void
CubeTexture::data(unsigned char*    data,
                  int                ,
                  int                )
{
    const unsigned int faceWidth    = _width >> 2;
    const unsigned int faceHeight    = _height / 3;
    const unsigned int faceSize        = faceWidth * faceHeight * sizeof(int);

    std::vector<unsigned char> rgba;

    std::pair<uint, uint> faceXY [6] = {
        std::make_pair(2, 1), // positive x
        std::make_pair(0, 1), // negative x
        std::make_pair(1, 0), // positive y
        std::make_pair(1, 2), // negative y
        std::make_pair(1, 1), // positive z
        std::make_pair(3, 1)  // negative z
    };

    for (int faceId = 0; faceId < 6; ++faceId)
    {
        rgba = std::vector<unsigned char>(faceSize, 0);

        const uint startX    = std::min(faceXY[faceId].first    * faceWidth,    _width - 1);
        const uint startY    = std::min(faceXY[faceId].second * faceHeight,    _height - 1);
        const uint endX        = std::min(startX + faceWidth - 1,                _width - 1);
        const uint endY        = std::min(startY + faceHeight - 1,                _height - 1);

        for (uint y = startY; y <= endY; ++y)
        {
            uint idx = ((y - startY) * faceWidth) << 2;

            for (uint x = startX; x <= endX; ++x)
            {
                unsigned int xy = x + _width * y;

                if (_format == TextureFormat::RGBA)
                {
                    xy <<= 2;
                    for (unsigned int k = 0; k < 4; ++k)
                        rgba[idx++] = data[xy++];
                }
                else if (_format == TextureFormat::RGB)
                {
                    xy *= 3;
                    for (unsigned int k = 0; k < 3; ++k)
                        rgba[idx++] = data[xy++];
                    rgba[idx++] = std::numeric_limits<unsigned char>::max();
                }
            }
        }

        assert(math::isp2(_widthGPU) && math::isp2(_heightGPU));

        resizeData(faceWidth, faceHeight, rgba, _widthGPU, _heightGPU, _resizeSmoothly, _data[faceId]);
    }
}

void
CubeTexture::upload()
{
    if (_id == -1)
        _id = _context->createTexture(
            _type,
            _widthGPU,
            _heightGPU,
            _mipMapping,
            _optimizeForRenderToTexture
        );

    const unsigned int numFacePixels = _widthGPU * _heightGPU;
    if (numFacePixels == 0)
        return;

    for (int faceId = 0; faceId < 6; ++faceId)
    {
        std::vector<unsigned char>& faceData = _data[faceId];

        assert(faceData.size() == (numFacePixels << 2));

        const auto face = static_cast<Face>(faceId);

        _context->uploadCubeTextureData(
            _id,
            face,
            _widthGPU,
            _heightGPU,
            0,
            &faceData.front()
        );
    }

    if (_mipMapping)
        _context->generateMipmaps(_id);
}

void
CubeTexture::dispose()
{
    if (_id != -1)
    {
        _context->deleteTexture(_id);
        _id = -1;
    }

    disposeData();
}

void
CubeTexture::disposeData()
{
    for (auto& face : _data)
    {
        face.clear();
        face.shrink_to_fit();
    }
}

//void
//CubeTexture::uploadMipLevel(uint    level,
//                            unsigned char*)
//{
//}