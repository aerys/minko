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
# define CRND_HEADER_FILE_ONLY
# include "crn_core.h"
# include "crn_console.h"
# include "crn_decomp.h"
# include "dds_defs.h"
#endif

using namespace minko;
using namespace minko::file;
using namespace minko::render;


#ifndef MINKO_NO_CRNLIB
static
crn_dxt_compressor_type
compressorTypeFromQualityFactor(TextureFormat format, float qualityFactor)
{
    static const auto qualityFactorToCRNCompressorType = std::vector<std::pair<float, crn_dxt_compressor_type>>
    {
        { 0.1f,     crn_dxt_compressor_type::cCRNDXTCompressorRYG },
        { 0.5f,     crn_dxt_compressor_type::cCRNDXTCompressorCRNF },
        { 1.f,      crn_dxt_compressor_type::cCRNDXTCompressorCRN }
    };

    auto compressorType = crn_dxt_compressor_type();

    for (const auto& qualityFactorToCompressorTypePair : qualityFactorToCRNCompressorType)
    {
        if (qualityFactor <= qualityFactorToCompressorTypePair.first)
        {
            compressorType = qualityFactorToCompressorTypePair.second;

            break;
        }
    }

    return compressorType;
}

static
void
fillTextureData(const crn_uint32*                           data[6][16],
                std::shared_ptr<render::Texture>            texture,
                const std::string&                          textureType,
                std::shared_ptr<WriterOptions>              writerOptions)
{
    const auto mipMapping = writerOptions->generateMipMaps(textureType);
    const auto preserveMipMaps = writerOptions->preserveMipMaps(textureType);

    if (mipMapping && preserveMipMaps)
    {
        const auto width = texture->width();
        const auto height = texture->height();
        const auto numMipMaps = AbstractTexture::numMipMaps(width, height);

        auto offset = 0u;

        for (auto i = 0u; i < numMipMaps; ++i)
        {
            const auto mipMapSize = TextureFormatInfo::textureSize(texture->format(), width >> i, height >> i);

            data[0][i] = reinterpret_cast<const unsigned int*>(texture->data().data() + offset);

            offset += mipMapSize;
        }
    }
    else
    {
        data[0][0] = reinterpret_cast<const unsigned int*>(texture->data().data());
    }
}

#endif

bool
CRNTranscoder::transcode(std::shared_ptr<render::AbstractTexture>  texture,
                         const std::string&                        textureType,
                         std::shared_ptr<WriterOptions>            writerOptions,
                         render::TextureFormat                     outFormat,
                         std::vector<unsigned char>&               out)
{
#ifndef MINKO_NO_CRNLIB
    crnlib::console::disable_output();

    const auto textureFormatToCRNTextureFomat = std::unordered_map<TextureFormat, crn_format, Hash<TextureFormat>>
    {
        { TextureFormat::RGB_DXT1,      crn_format::cCRNFmtDXT1 },
        { TextureFormat::RGBA_DXT1,     crn_format::cCRNFmtDXT1 },
        { TextureFormat::RGBA_DXT3,     crn_format::cCRNFmtDXT3 },
        { TextureFormat::RGBA_DXT5,     crn_format::cCRNFmtDXT5 }
    };

    const auto startTimeStamp = std::clock();

    const auto generateMipmaps = writerOptions->generateMipMaps(textureType);
    const auto numMipMaps = generateMipmaps ? AbstractTexture::numMipMaps(texture->width(), texture->height()) : 1u;

    switch (texture->type())
    {
    case TextureType::Texture2D:
    {
        auto texture2d = std::static_pointer_cast<Texture>(texture);

        auto texture2dData = std::vector<unsigned char>(
            texture2d->data().begin(),
            texture2d->data().end()
        );

        const auto useSRGBSpace = writerOptions->useTextureSRGBSpace(textureType);

        crn_comp_params compressorParameters;

        compressorParameters.m_faces = 1u;

        compressorParameters.m_width = texture2d->width();
        compressorParameters.m_height = texture2d->height();

        fillTextureData(compressorParameters.m_pImages, texture2d, textureType, writerOptions);

        compressorParameters.set_flag(cCRNCompFlagDXT1AForTransparency, outFormat == TextureFormat::RGBA_DXT1);
        compressorParameters.set_flag(cCRNCompFlagHierarchical, false);
        compressorParameters.set_flag(cCRNCompFlagPerceptual, false);

        compressorParameters.m_file_type = cCRNFileTypeDDS;
        compressorParameters.m_format = textureFormatToCRNTextureFomat.at(outFormat);
        compressorParameters.m_dxt_compressor_type = compressorTypeFromQualityFactor(
            outFormat,
            writerOptions->compressedTextureQualityFactor(textureType)
        );
        compressorParameters.m_dxt_quality = crn_dxt_quality::cCRNDXTQualityUber;
        compressorParameters.m_quality_level = cCRNMaxQualityLevel;

        crn_mipmap_params compressorMipParameters;
        compressorMipParameters.m_mode = cCRNMipModeNoMips;

        if (generateMipmaps)
        {
            if (writerOptions->preserveMipMaps(textureType))
            {
                compressorParameters.m_levels = numMipMaps;
                compressorMipParameters.m_mode = cCRNMipModeUseSourceMips;
            }
            else
            {
                compressorMipParameters.m_mode = cCRNMipModeGenerateMips;
            }
        }

        compressorMipParameters.m_gamma_filtering = useSRGBSpace;

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

        auto ddsFileData = reinterpret_cast<const char*>(ddsFileRawData);

        unsigned int ddsFilecode = 0u;
        memcpy(&ddsFilecode, ddsFileData, 4u);

        if (ddsFilecode != crnlib::cDDSFileSignature)
            return false;

        crnlib::DDSURFACEDESC2 ddsHeader;
        memcpy(&ddsHeader, ddsFileData + 4u, crnlib::cDDSSizeofDDSurfaceDesc2);

        auto mipOffset = crnlib::cDDSSizeofDDSurfaceDesc2 + 4u;

        for (auto i = 0u; i < numMipMaps; ++i)
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
