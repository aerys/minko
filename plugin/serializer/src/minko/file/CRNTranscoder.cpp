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

#include "minko/file/CRNTranscoder.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/MipFilter.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

#ifndef MINKO_NO_CRNLIB
# include "crnlib.h"
# include "crn_decomp.h"
# include "dds_defs.h"
#endif

using namespace minko;
using namespace minko::file;
using namespace minko::render;

bool
CRNTranscoder::transcode(std::shared_ptr<render::AbstractTexture>  texture,
                         std::shared_ptr<WriterOptions>            writerOptions,
                         render::TextureFormat                     outFormat,
                         std::vector<unsigned char>&               out)
{
#ifndef MINKO_NO_CRNLIB
    const auto textureFormatToCRNTextureFomat = std::unordered_map<TextureFormat, crn_format>
    {
        { TextureFormat::RGB_DXT1,      crn_format::cCRNFmtDXT1 },
        { TextureFormat::RGBA_DXT1,     crn_format::cCRNFmtDXT1 },
        { TextureFormat::RGBA_DXT3,     crn_format::cCRNFmtDXT3 },
        { TextureFormat::RGBA_DXT5,     crn_format::cCRNFmtDXT5 }
    };

    const auto startTimeStamp = std::clock();

    const auto generateMipmaps = writerOptions->generateMipmaps();

    switch (texture->type())
    {
    case TextureType::Texture2D:
    {
        auto texture2d = std::static_pointer_cast<Texture>(texture);

        auto texture2dData = std::vector<unsigned char>(
            texture2d->data().begin(),
            texture2d->data().end()
        );

        crn_comp_params compressorParameters;

        compressorParameters.m_width = texture2d->width();
        compressorParameters.m_height = texture2d->height();
        compressorParameters.m_pImages[0][0] = reinterpret_cast<const unsigned int*>(texture2dData.data());

        compressorParameters.set_flag(cCRNCompFlagDXT1AForTransparency, outFormat == TextureFormat::RGBA_DXT1);
        compressorParameters.set_flag(cCRNCompFlagHierarchical, false);

        compressorParameters.m_file_type = cCRNFileTypeDDS;
        compressorParameters.m_format = textureFormatToCRNTextureFomat.at(outFormat);
        compressorParameters.m_dxt_compressor_type = crn_dxt_compressor_type::cCRNDXTCompressorRYG;
        compressorParameters.m_dxt_quality = crn_dxt_quality::cCRNDXTQualityUber;
        compressorParameters.m_quality_level = cCRNMaxQualityLevel;

        crn_mipmap_params compressorMipParameters;
        compressorMipParameters.m_mode = generateMipmaps ? cCRNMipModeGenerateMips : cCRNMipModeNoMips;

        unsigned int actualQualityLevel;
        float actualBitrate;

        auto ddsTextureDataSize = 0u;

        auto ddsFileRawData = crn_compress(
            compressorParameters,
            compressorMipParameters,
            ddsTextureDataSize,
            &actualQualityLevel,
            &actualBitrate
        );

        const auto width = texture->width();
        const auto height = texture->height();
        const auto numMipmaps = generateMipmaps ? math::getp2(texture->width()) + 1u : 0u;

        auto ddsFileData = reinterpret_cast<const char*>(ddsFileRawData);

        unsigned int ddsFilecode = 0u;
        memcpy(&ddsFilecode, ddsFileData, 4u);

        if (ddsFilecode != crnlib::cDDSFileSignature)
            return false;

        crnlib::DDSURFACEDESC2 ddsHeader;
        memcpy(&ddsHeader, ddsFileData + 4u, crnlib::cDDSSizeofDDSurfaceDesc2);

        auto mipOffset = crnlib::cDDSSizeofDDSurfaceDesc2 + 4u;

        for (auto i = 0u; i < numMipmaps; ++i)
        {
            const auto mipWidth = width >> i;
            const auto mipHeight = height >> i;
            const auto mipBlocksX = (mipWidth + 3) >> 2;
            const auto mipBlocksY = (mipHeight + 3) >> 2;
            const auto mipRowPitch = mipBlocksX * crnd::crnd_get_bytes_per_dxt_block(compressorParameters.m_format);
            const auto mipDataSize = mipRowPitch * mipBlocksY;

            auto mipData = std::vector<unsigned char>(mipDataSize);

            std::copy(
                ddsFileData + mipOffset,
                ddsFileData + mipOffset + mipDataSize,
                mipData.begin()
            );

            mipOffset += mipDataSize;

            out.insert(
                out.end(),
                mipData.begin(),
                mipData.end()
            );
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
