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

#include "minko/Types.hpp"
#include "minko/file/TextureWriter.hpp"
#include "minko/file/AbstractWriter.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/CRNTranscoder.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/PNGWriter.hpp"
#include "minko/file/PVRTranscoder.hpp"
#include "minko/file/QTranscoder.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::render;

std::unordered_map<TextureFormat, TextureWriter::FormatWriterFunction, Hash<TextureFormat>> TextureWriter::_formatWriterFunctions = 
{
    { TextureFormat::RGB, std::bind(writeRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    { TextureFormat::RGBA, std::bind(writeRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },

    { TextureFormat::RGB_DXT1, std::bind(writeCRNCompressedTexture, TextureFormat::RGB_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    { TextureFormat::RGBA_DXT1, std::bind(writeCRNCompressedTexture, TextureFormat::RGBA_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    { TextureFormat::RGBA_DXT3, std::bind(writeCRNCompressedTexture, TextureFormat::RGBA_DXT3, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    { TextureFormat::RGBA_DXT5, std::bind(writeCRNCompressedTexture, TextureFormat::RGBA_DXT5, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    
    { TextureFormat::RGB_ETC1, std::bind(writePvrCompressedTexture, TextureFormat::RGB_ETC1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },

    { TextureFormat::RGB_PVRTC1_2BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGB_PVRTC1_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    { TextureFormat::RGB_PVRTC1_4BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGB_PVRTC1_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    { TextureFormat::RGBA_PVRTC1_2BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC1_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    { TextureFormat::RGBA_PVRTC1_4BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC1_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },

    { TextureFormat::RGBA_PVRTC2_2BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC2_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    { TextureFormat::RGBA_PVRTC2_4BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC2_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },

    { TextureFormat::RGB_ATITC, std::bind(writeQCompressedTexture, TextureFormat::RGB_ATITC, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    { TextureFormat::RGBA_ATITC, std::bind(writeQCompressedTexture, TextureFormat::RGBA_ATITC, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) }
};

const float TextureWriter::_defaultGamma = 2.2f;

TextureWriter::TextureWriter() :
    AbstractWriter<AbstractTexture::Ptr>(),
    _textureType(),
    _headerSize(0)
{
    _magicNumber = 0x00000054 | MINKO_SCENE_MAGIC_NUMBER;
}

void
TextureWriter::gammaEncode(const std::vector<unsigned char>&    src,
                           std::vector<unsigned char>&          dst,
                           float                                gamma)
{
    dst.resize(src.size(), 0);

    for (auto i = 0u; i < src.size(); ++i)
    {
        dst[i] = static_cast<unsigned char>(math::pow(
            static_cast<float>(src[i]) / 255.f,
            1.f / gamma
        ) * 255.f);
    }
}

void
TextureWriter::gammaDecode(const std::vector<unsigned char>&    src,
                           std::vector<unsigned char>&          dst,
                           float                                gamma)
{
    dst.resize(src.size(), 0);

    for (auto i = 0u; i < src.size(); ++i)
    {
        dst[i] = static_cast<unsigned char>(math::pow(
            static_cast<float>(src[i]) / 255.f,
            gamma
        ) * 255.f);
    }
}

std::string
TextureWriter::embed(AssetLibraryPtr               assetLibrary,
                     OptionsPtr                    options,
                     DependencyPtr                 dependency,
                     WriterOptionsPtr              writerOptions,
                     std::vector<unsigned char>&   embeddedHeaderData)
{
    auto texture = _data;

    ensureTextureSizeIsValid(texture, writerOptions, _textureType);

    if (texture->type() == TextureType::Texture2D && !writerOptions->useTextureSRGBSpace(_textureType))
    {
        auto texture2D = std::static_pointer_cast<Texture>(texture);

        gammaDecode(texture2D->data(), texture2D->data(), defaultGamma());
    }

    const auto generateMipmaps = writerOptions->generateMipMaps(_textureType);

    const auto textureFormats = writerOptions->textureFormats(_textureType, assetLibrary->textureName(texture));

    std::stringstream headerStream;
    std::stringstream blobStream;

    auto headerData = msgpack::type::tuple<
        msgpack::type::tuple<int, int, unsigned char, unsigned char>,
        std::vector<msgpack::type::tuple<int, int, int>>>();

    auto formatHeaderData = std::vector<msgpack::type::tuple<int, int, int>>();

    for (auto textureFormat : textureFormats)
    {
        const auto offset = blobStream.str().size();

        if (!_formatWriterFunctions.at(textureFormat)(_data, _textureType, writerOptions, blobStream))
        {
            // TODO
            // handle error
        }
        else
        {
            auto length = blobStream.str().size() - offset;

            formatHeaderData.push_back(msgpack::type::make_tuple<int, int, int>(
                static_cast<int>(textureFormat),
                offset,
                length)
            );
        }
    }

    const auto width = texture->width();
    const auto height = texture->height();

    const auto numFaces = static_cast<unsigned char>((texture->type() == TextureType::Texture2D ? 1 : 6));
    const auto numMipmaps = static_cast<unsigned char>((generateMipmaps ? math::getp2(width) + 1 : 0));

    msgpack::type::tuple<int, int, unsigned char, unsigned char> textureHeaderData(
        width,
        height,
        numFaces,
        numMipmaps
    );

    headerData.get<0>() = textureHeaderData;
    headerData.get<1>() = formatHeaderData;

    msgpack::pack(headerStream, headerData);

    const auto serializedHeaderData = headerStream.str();
    unsigned short headerSize = serializedHeaderData.size();

    _headerSize = headerSize;

    std::stringstream result;

    result.write(reinterpret_cast<const char*>(&headerSize), 2u);

    result << headerStream.str() << blobStream.str();

    return result.str();
}

void
TextureWriter::ensureTextureSizeIsValid(AbstractTexture::Ptr    texture,
                                        WriterOptions::Ptr      writerOptions,
                                        const std::string&      textureType)
{
    const auto width = texture->width();
    const auto height = texture->height();

    auto newWidth = width;
    auto newHeight = height;

    if (writerOptions->generateMipMaps(_textureType) &&
        newWidth != newHeight)
    {
        newWidth = newHeight = writerOptions->upscaleTextureWhenProcessedForMipMapping(_textureType)
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
        texture->resize(
            newWidth,
            newHeight,
            writerOptions->textureFilter(textureType) == TextureFilter::LINEAR
        );
    }
}

bool
TextureWriter::writeRGBATexture(AbstractTexture::Ptr    abstractTexture,
                                const std::string&      textureType,
                                WriterOptions::Ptr      writerOptions,
                                std::stringstream&      blob)
{
    auto imageFormat = writerOptions->imageFormat();

    auto texture = std::static_pointer_cast<Texture>(abstractTexture);

    auto textureData = std::vector<unsigned char>();

    switch (imageFormat)
    {
    case serialize::ImageFormat::PNG:
    {
        auto writer = PNGWriter::create();

        writer->writeToStream(textureData, texture->data(), texture->width(), texture->height());

        break;
    }
    default:
        return false;
    }

    msgpack::type::tuple<int, std::string> serializedTexture(
        static_cast<int>(imageFormat),
        std::string(textureData.begin(), textureData.end())
    );

    msgpack::pack(blob, serializedTexture);

    return true;
}

bool
TextureWriter::writePvrCompressedTexture(TextureFormat        textureFormat,
                                         AbstractTexture::Ptr abstractTexture,
                                         const std::string&   textureType,
                                         WriterOptions::Ptr   writerOptions,
                                         std::stringstream&   blob)
{
    auto out = std::vector<unsigned char>();

    if (!PVRTranscoder::transcode(abstractTexture, textureType, writerOptions, textureFormat, out, { PVRTranscoder::Options::fastCompression }))
        return false;

    blob.write(reinterpret_cast<const char*>(out.data()), out.size());

    return true;
}

bool
TextureWriter::writeQCompressedTexture(TextureFormat        textureFormat,
                                       AbstractTexture::Ptr abstractTexture,
                                       const std::string&   textureType,
                                       WriterOptions::Ptr   writerOptions,
                                       std::stringstream&   blob)
{
    auto out = std::vector<unsigned char>();

    if (!QTranscoder::transcode(abstractTexture, textureType, writerOptions, textureFormat, out))
        return false;

    blob.write(reinterpret_cast<const char*>(out.data()), out.size());

    return true;
}

bool
TextureWriter::writeCRNCompressedTexture(TextureFormat        textureFormat,
                                         AbstractTexture::Ptr abstractTexture,
                                         const std::string&   textureType,
                                         WriterOptions::Ptr   writerOptions,
                                         std::stringstream&   blob)
{
    auto out = std::vector<unsigned char>();

    if (!CRNTranscoder::transcode(abstractTexture, textureType, writerOptions, textureFormat, out))
        return false;

    blob.write(reinterpret_cast<const char*>(out.data()), out.size());

    return true;
}
