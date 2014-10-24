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

#include "minko/file/PVRTranscoder.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/Texture.hpp"

#ifndef MINKO_NO_PVRTEXTOOL
# include "PVRTextureDefines.h"
# include "PVRTexture.h"
# include "PVRTextureUtilities.h"
#endif

using namespace minko;
using namespace minko::file;
using namespace minko::render;

bool
PVRTranscoder::transcode(std::shared_ptr<render::AbstractTexture>  texture,
                         std::shared_ptr<WriterOptions>            options,
                         render::TextureFormat                     outFormat,
                         std::vector<unsigned char>&               out)
{
#ifndef MINKO_NO_PVRTEXTOOL
    static const auto textureFormatToPvrTextureFomat = std::unordered_map<TextureFormat, EPVRTPixelFormat>
    {
        { TextureFormat::RGB_DXT1, ePVRTPF_DXT1 },
        { TextureFormat::RGBA_DXT3, ePVRTPF_DXT3 },
        { TextureFormat::RGBA_DXT5, ePVRTPF_DXT5 },

        { TextureFormat::RGB_ETC1, ePVRTPF_ETC1 },

        { TextureFormat::RGB_PVRTC1, ePVRTPF_PVRTCI_4bpp_RGB },
        { TextureFormat::RGBA_PVRTC1, ePVRTPF_PVRTCI_4bpp_RGBA },
        { TextureFormat::RGBA_PVRTC2, ePVRTPF_PVRTCII_4bpp }
    };

    std::shared_ptr<pvrtexture::CPVRTexture> pvrTexture;

    switch (texture->type())
    {
    case TextureType::Texture2D:
    {
        auto texture2d = std::static_pointer_cast<Texture>(texture);

        pvrtexture::CPVRTextureHeader pvrHeader(
            pvrtexture::PVRStandard8PixelType.PixelTypeID,
            texture->height(),
            texture->width(),
            1u,
            1u,
            1u,
            1u,
            ePVRTCSpacesRGB
        );

        pvrTexture = std::make_shared<pvrtexture::CPVRTexture>(pvrHeader, texture2d->data().data());

        if (!pvrtexture::Transcode(
            *pvrTexture,
            textureFormatToPvrTextureFomat.at(outFormat),
            ePVRTVarTypeUnsignedByteNorm,
            ePVRTCSpacesRGB))
        {
            return false;
        }

        break;
    }
    case TextureType::CubeTexture:
    {
        // TODO
        // handle CubeTexture

        return false;
    }
    }

    const auto size = pvrTexture->getDataSize();
    const auto data = reinterpret_cast<const unsigned char*>(pvrTexture->getDataPtr());

    out.assign(data, data + size);

    return true;
#else
    return false;
#endif
}
