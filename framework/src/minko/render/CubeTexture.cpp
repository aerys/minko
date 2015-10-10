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
#include "minko/render/TextureFormatInfo.hpp"

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
    _widthGPU = std::min(math::clp2(width), MAX_SIZE);
    _heightGPU = std::min(math::clp2(height), MAX_SIZE);
}

void
CubeTexture::data(unsigned char*     data,
                  Face               face,
                  int                widthGPU,
                  int                heightGPU)
{
    assert(math::isp2(_widthGPU) && math::isp2(_heightGPU));

    resizeData(_width, _height, data, _widthGPU, _heightGPU, _resizeSmoothly, _data[static_cast<uint>(face)]);
}

void
CubeTexture::resize(unsigned int width, unsigned int height, bool resizeSmoothly)
{
    assert(math::isp2(width) && math::isp2(height));

    const auto previousWidth = this->width();
    const auto previousHeight = this->height();

    for (int faceId = 0; faceId < 6; ++faceId)
    {
        auto previousData = _data[faceId];

        resizeData(previousWidth, previousHeight, &previousData[0], width, height, resizeSmoothly, _data[faceId]);
    }

    _width = width << 2;
    _widthGPU = width;

    _height = height * 3;
    _heightGPU = height;
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

    // if (_mipMapping)
    //     _context->generateMipmaps(_id);
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

void
CubeTexture::uploadMipLevel(uint            level,
                            unsigned char*  data,
                            Face            face)
{
    const auto width = (_widthGPU >> level);
    const auto height = (_heightGPU >> level);

    _context->uploadCubeTextureData(
        _id,
        face,
        width,
        height,
        level,
        data
    );
}
