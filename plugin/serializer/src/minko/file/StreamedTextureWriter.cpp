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

#include "minko/file/AssetLibrary.hpp"
#include "minko/file/CRNTranscoder.hpp"
#include "minko/file/LinkedAsset.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/PNGWriter.hpp"
#include "minko/file/PVRTranscoder.hpp"
#include "minko/file/QTranscoder.hpp"
#include "minko/file/StreamedTextureWriter.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::render;

std::unordered_map<TextureFormat, StreamedTextureWriter::FormatWriterFunction, Hash<render::TextureFormat>> StreamedTextureWriter::_formatWriterFunctions =
{
    { TextureFormat::RGB, std::bind(writeRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },
    { TextureFormat::RGBA, std::bind(writeRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },

    { TextureFormat::RGB_DXT1, std::bind(writeCRNCompressedTexture, TextureFormat::RGB_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },
    { TextureFormat::RGBA_DXT1, std::bind(writeCRNCompressedTexture, TextureFormat::RGBA_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },
    { TextureFormat::RGBA_DXT3, std::bind(writeCRNCompressedTexture, TextureFormat::RGBA_DXT3, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },
    { TextureFormat::RGBA_DXT5, std::bind(writeCRNCompressedTexture, TextureFormat::RGBA_DXT5, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },

    { TextureFormat::RGB_ETC1, std::bind(writePvrCompressedTexture, TextureFormat::RGB_ETC1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },

    { TextureFormat::RGB_PVRTC1_2BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGB_PVRTC1_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },
    { TextureFormat::RGB_PVRTC1_4BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGB_PVRTC1_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },
    { TextureFormat::RGBA_PVRTC1_2BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC1_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },
    { TextureFormat::RGBA_PVRTC1_4BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC1_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },

    { TextureFormat::RGBA_PVRTC2_2BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC2_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },
    { TextureFormat::RGBA_PVRTC2_4BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC2_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },

    { TextureFormat::RGB_ATITC, std::bind(writeQCompressedTexture, TextureFormat::RGB_ATITC, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) },
    { TextureFormat::RGBA_ATITC, std::bind(writeQCompressedTexture, TextureFormat::RGBA_ATITC, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5) }
};

StreamedTextureWriter::StreamedTextureWriter() :
    AbstractWriter<AbstractTexture::Ptr>(),
    _textureType()
{
    _magicNumber = 0x00000055 | MINKO_SCENE_MAGIC_NUMBER;
}

std::string
StreamedTextureWriter::embed(AssetLibrary::Ptr              assetLibrary,
                             Options::Ptr                   options,
                             Dependency::Ptr                dependency,
                             WriterOptions::Ptr             writerOptions,
                             std::vector<unsigned char>&    embeddedHeaderData)
{
    if (!writerOptions->generateMipmaps(_textureType))
        writerOptions->generateMipmaps(_textureType, true);

    auto texture = _data;

    ensureTextureSizeIsValid(texture, writerOptions);

    const auto& textureFormats = writerOptions->textureFormats();

    auto linkedAsset = _linkedAsset;
    const auto linkedAssetId = _linkedAssetId;

    msgpack::sbuffer headerStream;
    msgpack::sbuffer blobStream;

    auto headerData = msgpack::type::tuple<
        unsigned int,
        msgpack::type::tuple<int, int, unsigned char, unsigned char>,
        std::vector<msgpack::type::tuple<int, std::vector<msgpack::type::tuple<int, int>>>>>();

    auto formatHeaders = std::vector<msgpack::type::tuple<int, std::vector<msgpack::type::tuple<int, int>>>>();

    for (auto textureFormat : textureFormats)
    {
        if (TextureFormatInfo::isCompressed(textureFormat) &&
            !writerOptions->compressTexture(_textureType))
            continue;

        auto formatHeader = msgpack::type::tuple<int, std::vector<msgpack::type::tuple<int, int>>>();

        formatHeader.get<0>() = static_cast<int>(textureFormat);

        if (!_formatWriterFunctions.at(textureFormat)(_data, _textureType, writerOptions, blobStream, formatHeader.get<1>()))
        {
            // TODO
            // handle error
        }
        else
        {
            formatHeaders.push_back(formatHeader);
        }
    }

    const auto width = texture->width();
    const auto height = texture->height();
    const auto numFaces = static_cast<unsigned char>(texture->type() == TextureType::Texture2D ? 1 : 6);
    const auto numMipmaps = static_cast<unsigned char>(writerOptions->generateMipmaps(_textureType) && texture->width() == texture->height() ? math::getp2(texture->width()) + 1 : 0);

    auto textureHeaderData = msgpack::type::tuple<int, int, unsigned char, unsigned char>(
        width,
        height,
        numFaces,
        numMipmaps
    );

    headerData.get<0>() = linkedAssetId;
    headerData.get<1>() = textureHeaderData;
    headerData.get<2>() = formatHeaders;

    msgpack::pack(headerStream, headerData);

    const auto headerSize = static_cast<int>(headerStream.size());

    std::stringstream result;

    embeddedHeaderData.push_back((headerSize & 0xff00) >> 8);
    embeddedHeaderData.push_back((headerSize & 0x00ff));

    embeddedHeaderData.insert(
        embeddedHeaderData.end(),
        headerStream.data(),
        headerStream.data() + headerSize
    );

    if (linkedAsset != nullptr &&
        linkedAsset->linkType() == LinkedAsset::LinkType::Internal)
    {
        linkedAsset
            ->length(blobStream.size())
            ->data(std::vector<unsigned char>(blobStream.data(), blobStream.data() + blobStream.size()));
    }
    else
    {
        result.write(
            blobStream.data(),
            blobStream.size()
        );
    }

    return result.str();
}

void
StreamedTextureWriter::ensureTextureSizeIsValid(AbstractTexture::Ptr    texture,
                                                WriterOptions::Ptr      writerOptions)
{
    const auto width = texture->width();
    const auto height = texture->height();

    auto newWidth = width;
    auto newHeight = height;

    if (newWidth != newHeight)
    {
        newWidth = newHeight = writerOptions->upscaleTextureWhenProcessedForMipmapping(_textureType)
            ? std::max<uint>(newWidth, newHeight)
            : std::min<uint>(newWidth, newHeight);
    }

    newWidth = static_cast<uint>(newWidth * writerOptions->textureScale(_textureType).x);
    newHeight = static_cast<uint>(newHeight * writerOptions->textureScale(_textureType).y);

    newWidth = std::min<uint>(newWidth, writerOptions->textureMaxSize(_textureType).x);
    newHeight = std::min<uint>(newHeight, writerOptions->textureMaxSize(_textureType).y);

    if (width != newWidth ||
        height != newHeight)
    {
        texture->resize(newWidth, newHeight, true);
    }
}

bool
StreamedTextureWriter::writeMipLevels(TextureFormat                                  textureFormat,
                                      int                                            textureWidth,
                                      int                                            textureHeight,
                                      const std::vector<unsigned char>&              data,
                                      std::vector<msgpack::type::tuple<int, int>>&   mipLevels,
                                      msgpack::sbuffer&                              blob)
{
    const auto numMipmaps = math::getp2(std::max<int>(textureWidth, textureHeight)) + 1;

    mipLevels.resize(numMipmaps);

    auto dataOffset = 0;
    auto serializedDataOffset = blob.size();

    for (auto i = 0u; i < numMipmaps; ++i)
    {
        const auto previousBlobSize = blob.size();

        const auto mipLevelWidth = std::max(textureWidth >> i, 1);
        const auto mipLevelHeight = std::max(textureHeight >> i, 1);

        const auto mipLevelDataSize = TextureFormatInfo::textureSize(textureFormat, mipLevelWidth, mipLevelHeight);

        const auto mipLevelData = std::vector<unsigned char>(
            data.begin() + dataOffset,
            data.begin() + dataOffset + mipLevelDataSize
        );

        blob.write(reinterpret_cast<const char*>(mipLevelData.data()), mipLevelData.size());

        const auto mipLevelSerializedDataSize = blob.size() - previousBlobSize;

        mipLevels[i].get<0>() = serializedDataOffset;
        mipLevels[i].get<1>() = mipLevelSerializedDataSize;

        dataOffset += mipLevelDataSize;
        serializedDataOffset += mipLevelSerializedDataSize;
    }

    return true;
}

bool
StreamedTextureWriter::writeRGBATexture(AbstractTexture::Ptr                            abstractTexture,
                                        const std::string&                              textureType,
                                        WriterOptions::Ptr                              writerOptions,
                                        msgpack::sbuffer&                               blob,
                                        std::vector<msgpack::type::tuple<int, int>>&    mipLevels)
{
    const auto textureFormat = TextureFormat::RGBA;

    auto imageFormat = writerOptions->imageFormat();

    auto texture = std::static_pointer_cast<Texture>(abstractTexture);

    const auto baseWidth = texture->width();
    const auto baseHeight = texture->height();

    auto mipLevelTemplate = Texture::create(
        texture->context(),
        baseWidth,
        baseHeight,
        false,
        false,
        true
    );

    mipLevelTemplate->data(texture->data().data());

    const auto numMipLevels = math::getp2(baseWidth) + 1;

    mipLevels.resize(numMipLevels);

    auto serializedDataOffset = blob.size();

    for (auto i = 0u; i < numMipLevels; ++i)
    {
        const auto previousBlobSize = blob.size();

        const auto mipLevelWidth = std::max(baseWidth >> i, 1u);
        const auto mipLevelHeight = std::max(baseHeight >> i, 1u);

        mipLevelTemplate->resize(mipLevelWidth, mipLevelHeight, true);

        auto mipLevelData = std::vector<unsigned char>();

        auto writer = PNGWriter::create();

        writer->writeToStream(mipLevelData, mipLevelTemplate->data(), mipLevelWidth, mipLevelHeight);

        blob.write(reinterpret_cast<const char*>(mipLevelData.data()), mipLevelData.size());

        const auto mipLevelSerializedDataSize = blob.size() - previousBlobSize;

        mipLevels[i].get<0>() = serializedDataOffset;
        mipLevels[i].get<1>() = mipLevelSerializedDataSize;

        serializedDataOffset += mipLevelSerializedDataSize;
    }

    return true;
}

bool
StreamedTextureWriter::writePvrCompressedTexture(TextureFormat                                  textureFormat,
                                                 AbstractTexture::Ptr                           abstractTexture,
                                                 const std::string&                             textureType,
                                                 WriterOptions::Ptr                             writerOptions,
                                                 msgpack::sbuffer&                              blob,
                                                 std::vector<msgpack::type::tuple<int, int>>&   mipLevels)
{
    auto out = std::vector<unsigned char>();

    if (!PVRTranscoder::transcode(
        abstractTexture,
        textureType,
        writerOptions,
        textureFormat,
        out,
        { PVRTranscoder::Options::fastCompression }))
        return false;

    if (!writeMipLevels(textureFormat, abstractTexture->width(), abstractTexture->height(), out, mipLevels, blob))
        return false;

    return true;
}

bool
StreamedTextureWriter::writeQCompressedTexture(TextureFormat                                textureFormat,
                                               AbstractTexture::Ptr                         abstractTexture,
                                               const std::string&                           textureType,
                                               WriterOptions::Ptr                           writerOptions,
                                               msgpack::sbuffer&                            blob,
                                               std::vector<msgpack::type::tuple<int, int>>& mipLevels)
{
    auto out = std::vector<unsigned char>();

    if (!QTranscoder::transcode(abstractTexture, textureType, writerOptions, textureFormat, out))
        return false;

    if (!writeMipLevels(textureFormat, abstractTexture->width(), abstractTexture->height(), out, mipLevels, blob))
        return false;

    return true;
}

bool
StreamedTextureWriter::writeCRNCompressedTexture(TextureFormat                                  textureFormat,
                                                 AbstractTexture::Ptr                           abstractTexture,
                                                 const std::string&                             textureType,
                                                 WriterOptions::Ptr                             writerOptions,
                                                 msgpack::sbuffer&                              blob,
                                                 std::vector<msgpack::type::tuple<int, int>>&   mipLevels)
{
    auto out = std::vector<unsigned char>();

    if (!CRNTranscoder::transcode(abstractTexture, textureType, writerOptions, textureFormat, out))
        return false;

    if (!writeMipLevels(textureFormat, abstractTexture->width(), abstractTexture->height(), out, mipLevels, blob))
        return false;

    return true;
}
