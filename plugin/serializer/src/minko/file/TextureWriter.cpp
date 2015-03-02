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

std::unordered_map<TextureFormat, TextureWriter::FormatWriterFunction> TextureWriter::_formatWriterFunctions = 
{
    { TextureFormat::RGB, std::bind(writeRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA, std::bind(writeRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },

    { TextureFormat::RGB_DXT1, std::bind(writeCRNCompressedTexture, TextureFormat::RGB_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA_DXT1, std::bind(writeQCompressedTexture, TextureFormat::RGBA_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA_DXT3, std::bind(writeCRNCompressedTexture, TextureFormat::RGBA_DXT3, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA_DXT5, std::bind(writeCRNCompressedTexture, TextureFormat::RGBA_DXT5, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },

    { TextureFormat::RGB_ETC1, std::bind(writePvrCompressedTexture, TextureFormat::RGB_ETC1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },

    { TextureFormat::RGB_PVRTC1_2BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGB_PVRTC1_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGB_PVRTC1_4BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGB_PVRTC1_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA_PVRTC1_2BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC1_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA_PVRTC1_4BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC1_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },

    { TextureFormat::RGBA_PVRTC2_2BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC2_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA_PVRTC2_4BPP, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_PVRTC2_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },

    { TextureFormat::RGB_ATITC, std::bind(writeQCompressedTexture, TextureFormat::RGB_ATITC, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA_ATITC, std::bind(writeQCompressedTexture, TextureFormat::RGBA_ATITC, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) }
};

TextureWriter::TextureWriter() :
    AbstractWriter<AbstractTexture::Ptr>(),
    _headerSize(0)
{
    _magicNumber = 0x00000054 | MINKO_SCENE_MAGIC_NUMBER;
}

std::string
TextureWriter::embed(AssetLibraryPtr               assetLibrary,
                     OptionsPtr                    options,
                     DependencyPtr                 dependency,
                     WriterOptionsPtr              writerOptions,
                     std::vector<unsigned char>&   embeddedHeaderData)
{
    auto texture = _data;

    const auto generateMipmaps = writerOptions->generateMipmaps();

    if (generateMipmaps)
    {
        const auto width = texture->width();
        const auto height = texture->height();

        if (width != height)
        {
            auto dimensionSize = writerOptions->upscaleTextureWhenProcessedForMipmapping()
                ? std::max<uint>(width, height)
                : std::min<uint>(width, height);

            dimensionSize = std::min<uint>(
                dimensionSize,
                static_cast<uint>(writerOptions->textureMaxResolution().x)
            );

            dimensionSize = std::min<uint>(
                dimensionSize,
                static_cast<uint>(writerOptions->textureMaxResolution().y)
            );

            const auto newWidth = dimensionSize;
            const auto newHeight = dimensionSize;

            switch (texture->type())
            {
            case TextureType::Texture2D:
            {
                auto texture2d = std::static_pointer_cast<Texture>(texture);

                texture->resize(newWidth, newHeight, true);

                break;
            }
            case TextureType::CubeTexture:
            {
                // TODO

                break;
            }
            }
        }
    }
    else
    {
        if (texture->width() > writerOptions->textureMaxResolution().x ||
            texture->height() > writerOptions->textureMaxResolution().y)
        {
            texture->resize(
                std::min<uint>(texture->width(), writerOptions->textureMaxResolution().x),
                std::min<uint>(texture->height(), writerOptions->textureMaxResolution().y),
                true
            );
        }
    }

    const auto& textureFormats = writerOptions->textureFormats();

    std::stringstream headerStream;
    std::stringstream blobStream;

    auto headerData = msgpack::type::tuple<
        msgpack::type::tuple<int, int, unsigned char, unsigned char>,
        std::vector<msgpack::type::tuple<int, int, int>>>();

    auto formatHeaderData = std::vector<msgpack::type::tuple<int, int, int>>();

    for (auto textureFormat : textureFormats)
    {
        if (TextureFormatInfo::isCompressed(textureFormat) &&
            !writerOptions->compressTexture())
            continue;

        const auto offset = blobStream.str().size();

        if (!_formatWriterFunctions.at(textureFormat)(_data, writerOptions, blobStream))
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

    auto textureHeaderData = msgpack::type::make_tuple<int, int, unsigned char, unsigned char>(
        width,
        height,
        numFaces,
        numMipmaps
    );

    headerData.a0 = textureHeaderData;
    headerData.a1 = formatHeaderData;

    msgpack::pack(headerStream, headerData);

    const auto serializedHeaderData = headerStream.str();
    unsigned short headerSize = serializedHeaderData.size();

    _headerSize = headerSize;

    std::stringstream result;

    result << headerSize << headerStream.str() << blobStream.str();

    return result.str();
}

bool
TextureWriter::writeRGBATexture(AbstractTexture::Ptr abstractTexture,
                                WriterOptions::Ptr writerOptions,
                                std::stringstream& blob)
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
                                         WriterOptions::Ptr   writerOptions,
                                         std::stringstream&   blob)
{
    auto out = std::vector<unsigned char>();

    if (!PVRTranscoder::transcode(abstractTexture, writerOptions, textureFormat, out, { PVRTranscoder::Options::fastCompression }))
        return false;

    blob.write(reinterpret_cast<const char*>(out.data()), out.size());

    return true;
}

bool
TextureWriter::writeQCompressedTexture(TextureFormat        textureFormat,
                                       AbstractTexture::Ptr abstractTexture,
                                       WriterOptions::Ptr   writerOptions,
                                       std::stringstream&   blob)
{
    auto out = std::vector<unsigned char>();

    if (!QTranscoder::transcode(abstractTexture, writerOptions, textureFormat, out))
        return false;

    blob.write(reinterpret_cast<const char*>(out.data()), out.size());

    return true;
}

bool
TextureWriter::writeCRNCompressedTexture(TextureFormat        textureFormat,
                                         AbstractTexture::Ptr abstractTexture,
                                         WriterOptions::Ptr   writerOptions,
                                         std::stringstream&   blob)
{
    auto out = std::vector<unsigned char>();

    if (!CRNTranscoder::transcode(abstractTexture, writerOptions, textureFormat, out))
        return false;

    blob.write(reinterpret_cast<const char*>(out.data()), out.size());

    return true;
}
