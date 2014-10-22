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

#include "minko/file/PVRWriter.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/Texture.hpp"

#include "PVRTextureDefines.h"
#include "PVRTexture.h"
#include "PVRTextureUtilities.h"

using namespace minko;
using namespace minko::file;
using namespace minko::render;

bool
PVRWriter::write(std::shared_ptr<render::AbstractTexture>  texture,
                 std::shared_ptr<WriterOptions>            options,
                 render::TextureFormat                     outFormat,
                 std::vector<unsigned char>&               out)
{
    static const auto textureFormatToPvrTextureFomat = std::unordered_map<TextureFormat, EPVRTPixelFormat>
    {
        { TextureFormat::RGB_DXT1, ePVRTPF_DXT1 },
        { TextureFormat::RGBA_DXT3, ePVRTPF_DXT3 },
        { TextureFormat::RGBA_DXT5, ePVRTPF_DXT5 },

        { TextureFormat::RGB_PVRTC1, ePVRTPF_PVRTCI_4bpp_RGB },
        { TextureFormat::RGBA_PVRTC1, ePVRTPF_PVRTCI_4bpp_RGBA },
        { TextureFormat::RGB_PVRTC2, ePVRTPF_PVRTCII_4bpp },
        { TextureFormat::RGBA_PVRTC2, ePVRTPF_PVRTCII_4bpp }
    };

    std::shared_ptr<pvrtexture::CPVRTexture> pvrTexture;

    if (texture->type() == TextureType::Texture2D)
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
    }
    else
    {
        // TODO
        // handle CubeTexture

        return false;
    }

    static auto uid = 0;

    auto fileName = std::string("tmpTexture") + std::to_string(uid++) + ".pvr";

    fileName = options->outputAssetUriFunction()(fileName);

    pvrTexture->saveFile(fileName.c_str());

    std::ifstream file;

    file.open(fileName, std::ios::in | std::ios::ate | std::ios::binary);

    if (!file.is_open())
        return false;

    auto size = file.tellg();

    out.resize(size, 0);

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(out.data()), size);

    if (!file)
        return false;

    file.close();

    if (std::remove(fileName.c_str()) != 0)
    {
        LOG_WARNING("failed to remove temporary texture file " << fileName);
    }

    return true;
}
