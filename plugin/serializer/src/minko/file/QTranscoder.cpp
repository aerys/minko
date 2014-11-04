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

#include "minko/file/QTranscoder.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/MipFilter.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

#ifndef MINKO_NO_QCOMPRESS
# include "QCompressLib.h"
#endif

using namespace minko;
using namespace minko::file;
using namespace minko::render;

bool
QTranscoder::transcode(std::shared_ptr<render::AbstractTexture>  texture,
                       std::shared_ptr<WriterOptions>            writerOptions,
                       render::TextureFormat                     outFormat,
                       std::vector<unsigned char>&               out)
{
#ifndef MINKO_NO_PVRTEXTOOL
    const auto textureFormatToQTextureFomat = std::unordered_map<TextureFormat, unsigned long long>
    {
        { TextureFormat::RGB,           Q_FORMAT_RGB_8I         },
        { TextureFormat::RGBA,          Q_FORMAT_RGBA_8I        },

        { TextureFormat::RGB_DXT1,      Q_FORMAT_S3TC_DXT1_RGB  },
        { TextureFormat::RGBA_DXT3,     Q_FORMAT_S3TC_DXT3_RGBA },
        { TextureFormat::RGBA_DXT5,     Q_FORMAT_S3TC_DXT5_RGBA },

        { TextureFormat::RGB_ETC1,      Q_FORMAT_ETC1_RGB8      },

        { TextureFormat::RGB_ATITC,     Q_FORMAT_ATITC_RGB      },
        { TextureFormat::RGBA_ATITC,    Q_FORMAT_ATITC_RGBA     }
    };

    const auto startTimeStamp = std::clock();

    switch (texture->type())
    {
    case TextureType::Texture2D:
    {
        auto texture2d = std::static_pointer_cast<Texture>(texture);

        auto qSrcTexture = TQonvertImage();
        qSrcTexture.nWidth = texture2d->width();
        qSrcTexture.nHeight = texture2d->height();
        qSrcTexture.nFormat = textureFormatToQTextureFomat.at(texture2d->format());
        qSrcTexture.nDataSize = texture2d->data().size();
        qSrcTexture.pData = texture2d->data().data();

        if (writerOptions->generateMipmaps())
        {
            const auto numMipmaps = CountNumMipLevels(texture2d->width(), texture2d->height());

            auto qRawDstTextures = new TQonvertImage*[numMipmaps];
            for (auto i = 0; i < numMipmaps; ++i)
                qRawDstTextures[i] = CreateEmptyTexture();

            if (!MipMapAndCompress(
                &qSrcTexture,
                qRawDstTextures,
                textureFormatToQTextureFomat.at(outFormat),
                texture2d->width(),
                texture2d->height(),
                numMipmaps))
            {
                return false;
            }

            auto qDstTextures = std::vector<TQonvertImage>();

            for (auto i = 0; i < numMipmaps; ++i)
                qDstTextures.push_back(*qRawDstTextures[i]);

            auto textureSize = 0;
            for (const auto& mipLevelTexture : qDstTextures)
                textureSize += mipLevelTexture.nDataSize;

            out.resize(textureSize);

            auto mipLevelOffset = 0;
            for (const auto& mipLevelTexture : qDstTextures)
            {
                std::copy(
                    mipLevelTexture.pData,
                    mipLevelTexture.pData + mipLevelTexture.nDataSize,
                    out.begin() + mipLevelOffset
                );

                mipLevelOffset += mipLevelTexture.nDataSize;
            }

            for (auto i = 0; i < numMipmaps; ++i)
            {
                delete qRawDstTextures[i]->pFormatFlags;
                delete qRawDstTextures[i]->pData;
            }

            delete[] qRawDstTextures;
        }
        else
        {
            auto qDstTexture = TQonvertImage();

            if (!Compress(
                &qSrcTexture,
                &qDstTexture,
                textureFormatToQTextureFomat.at(outFormat),
                texture->width(),
                texture->height()))
            {
                return false;
            }

            out.assign(qDstTexture.pData, qDstTexture.pData + qDstTexture.nDataSize);

            delete qDstTexture.pFormatFlags;
            delete qDstTexture.pData;
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

    return true;
#else
    return false;
#endif
}
