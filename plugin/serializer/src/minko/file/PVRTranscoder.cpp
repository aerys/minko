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
#include "minko/render/MipFilter.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

#ifndef MINKO_NO_PVRTEXTOOL
# include "PVRTextureDefines.h"
# include "PVRTexture.h"
# include "PVRTextureUtilities.h"
#endif

#ifndef MINKO_NO_PVRTEXTOOL
# ifdef DEBUG
#  define MINKO_DEBUG_WRITE_PVR_TEXTURE_TO_DISK
# endif
#endif

using namespace minko;
using namespace minko::file;
using namespace minko::render;

bool
PVRTranscoder::transcode(std::shared_ptr<render::AbstractTexture>  texture,
                         std::shared_ptr<WriterOptions>            writerOptions,
                         render::TextureFormat                     outFormat,
                         std::vector<unsigned char>&               out,
                         const Options&                            options)
{
#ifndef MINKO_NO_PVRTEXTOOL
    const auto textureFormatToPvrTextureFomat = std::unordered_map<TextureFormat, unsigned long long>
    {
        { TextureFormat::RGB,               pvrtexture::PVRStandard8PixelType.PixelTypeID },
        { TextureFormat::RGBA,              pvrtexture::PVRStandard8PixelType.PixelTypeID },

        { TextureFormat::RGB_DXT1,          ePVRTPF_DXT1                                  },
        { TextureFormat::RGBA_DXT3,         ePVRTPF_DXT3                                  },
        { TextureFormat::RGBA_DXT5,         ePVRTPF_DXT5                                  },

        { TextureFormat::RGB_ETC1,          ePVRTPF_ETC1                                  },

        { TextureFormat::RGB_PVRTC1_2BPP,   ePVRTPF_PVRTCI_2bpp_RGB                       },
        { TextureFormat::RGB_PVRTC1_4BPP,   ePVRTPF_PVRTCI_4bpp_RGB                       },
        { TextureFormat::RGBA_PVRTC1_2BPP,  ePVRTPF_PVRTCI_2bpp_RGBA                      },
        { TextureFormat::RGBA_PVRTC1_4BPP,  ePVRTPF_PVRTCI_4bpp_RGBA                      },
        { TextureFormat::RGBA_PVRTC2_2BPP,  ePVRTPF_PVRTCII_2bpp                          },
        { TextureFormat::RGBA_PVRTC2_4BPP,  ePVRTPF_PVRTCII_4bpp                          }
    };

    const auto startTimeStamp = std::clock();

    auto pvrTexture = std::unique_ptr<pvrtexture::CPVRTexture>();

    switch (texture->type())
    {
    case TextureType::Texture2D:
    {
        auto texture2d = std::static_pointer_cast<Texture>(texture);

        pvrtexture::CPVRTextureHeader pvrHeader(
            textureFormatToPvrTextureFomat.at(texture->format()),
            texture->height(),
            texture->width(),
            1u,
            1u,
            1u,
            1u,
            ePVRTCSpacesRGB
        );

        pvrTexture = std::make_unique<pvrtexture::CPVRTexture>(pvrHeader, texture2d->data().data());

        const auto generateMipmaps = writerOptions->generateMipmaps() && (texture2d->width() == texture2d->height());

        if (generateMipmaps)
        {
            static const auto mipFilterToPvrMipFilter = std::unordered_map<MipFilter, pvrtexture::EResizeMode>
            {
                { MipFilter::NONE,      pvrtexture::eResizeNearest },
                { MipFilter::NEAREST,   pvrtexture::eResizeNearest },
                { MipFilter::LINEAR,    pvrtexture::eResizeLinear }
            };

            if (!pvrtexture::GenerateMIPMaps(
                *pvrTexture,
                mipFilterToPvrMipFilter.at(writerOptions->mipFilter())))
            {
                return false;
            }
        }

        auto compressorQuality = pvrtexture::ePVRTCNormal;

        if (options._flags & Options::fastCompression)
        {
            switch (outFormat)
            {
            case TextureFormat::RGB_ETC1:
                compressorQuality = pvrtexture::eETCFast;
                break;

            default:
                compressorQuality = pvrtexture::ePVRTCFast;
                break;
            }
        }
        else
        {
            switch (outFormat)
            {
            case TextureFormat::RGB_ETC1:
                compressorQuality = pvrtexture::eETCSlow;
                break;

            default:
                compressorQuality = pvrtexture::ePVRTCNormal;
                break;
            }
        }

        if (!pvrtexture::Transcode(
            *pvrTexture,
            textureFormatToPvrTextureFomat.at(outFormat),
            ePVRTVarTypeUnsignedByteNorm,
            ePVRTCSpacesRGB,
            compressorQuality))
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

    const auto duration = (std::clock() - startTimeStamp) / static_cast<double>(CLOCKS_PER_SEC);

    LOG_INFO("compressing texture: "
        << texture->width()
        << "x"
        << texture->height()
        << " from "
        << TextureFormatInfo::name(texture->format())
        << " to "
        << TextureFormatInfo::name(outFormat)
        << " with duration of "
        << duration
    );

#ifdef MINKO_DEBUG_WRITE_PVR_TEXTURE_TO_DISK
    static auto pvrTextureId = 0;

    auto debugOutputFileName = std::string("debug_") + TextureFormatInfo::name(outFormat) + "_" + std::to_string(pvrTextureId++);

    debugOutputFileName = writerOptions->outputAssetUriFunction()(debugOutputFileName);

    pvrTexture->saveFile(debugOutputFileName.c_str());
#endif

    return true;
#else
    return false;
#endif
}
