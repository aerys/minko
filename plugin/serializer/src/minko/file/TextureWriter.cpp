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

#include "minko/file/TextureWriter.hpp"

#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/file/AbstractWriter.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/PNGWriter.hpp"
#include "minko/file/PVRTranscoder.hpp"
#include "minko/file/QTranscoder.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/log/Logger.hpp"
#include "minko/Types.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::render;

std::unordered_map<TextureFormat, TextureWriter::FormatWriterFunction> TextureWriter::_formatWriterFunctions = 
{
    { TextureFormat::RGB, std::bind(writeRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA, std::bind(writeRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },

    { TextureFormat::RGB_DXT1, std::bind(writePvrCompressedTexture, TextureFormat::RGB_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA_DXT1, std::bind(writeQCompressedTexture, TextureFormat::RGBA_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA_DXT3, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_DXT3, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA_DXT5, std::bind(writePvrCompressedTexture, TextureFormat::RGBA_DXT5, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },

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
TextureWriter::embed(AssetLibraryPtr     assetLibrary,
                     OptionsPtr          options,
                     Dependency::Ptr     dependency,
                     WriterOptionsPtr    writerOptions)
{
    auto texture = _data;

    const auto& textureFormats = writerOptions->textureFormats();

    std::stringstream headerStream;
    std::stringstream blobStream;

    auto headerData = msgpack::type::tuple<
        msgpack::type::tuple<int, int, unsigned char, unsigned char>,
        std::vector<msgpack::type::tuple<int, int, int>>>();

    auto formatHeaderData = std::vector<msgpack::type::tuple<int, int, int>>();

    for (auto textureFormat : textureFormats)
    {
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
    const auto numFaces = static_cast<unsigned char>(texture->type() == TextureType::Texture2D ? 1 : 6);
    const auto numMipmaps = static_cast<unsigned char>(writerOptions->generateMipmaps() && texture->width() == texture->height() ? math::getp2(texture->width()) + 1 : 0);

    auto textureHeaderData = msgpack::type::make_tuple<int, int, unsigned char, unsigned char>(
        width,
        height,
        numFaces,
        numMipmaps
    );

    headerData.a0 = textureHeaderData;
    headerData.a1 = formatHeaderData;

    msgpack::pack(headerStream, headerData);

    _headerSize = headerStream.str().size();

    std::stringstream result;

    result << headerStream.str() << blobStream.str();

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

    msgpack::type::tuple<int, std::vector<unsigned char>> serializedTexture(static_cast<int>(imageFormat), textureData);

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

    msgpack::pack(blob, out);

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

    msgpack::pack(blob, out);

    return true;
}
