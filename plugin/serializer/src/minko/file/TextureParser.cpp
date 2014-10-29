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
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/PNGParser.hpp"
#include "minko/file/TextureParser.hpp"
#include "minko/file/TextureWriter.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/OpenGLES2Context.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::render;
using namespace minko::serialize;

std::unordered_map<render::TextureFormat, TextureParser::FormatParserFunction> TextureParser::_formatParserFunctions =
{
    { TextureFormat::RGB, std::bind(parseRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA, std::bind(parseRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },

    { TextureFormat::RGB_DXT1, std::bind(parseCompressedTexture, TextureFormat::RGB_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA_DXT3, std::bind(parseCompressedTexture, TextureFormat::RGBA_DXT3, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA_DXT5, std::bind(parseCompressedTexture, TextureFormat::RGBA_DXT5, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },

    { TextureFormat::RGB_ETC1, std::bind(parseCompressedTexture, TextureFormat::RGB_ETC1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },

    { TextureFormat::RGB_PVRTC1_2BPP, std::bind(parseCompressedTexture, TextureFormat::RGB_PVRTC1_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGB_PVRTC1_4BPP, std::bind(parseCompressedTexture, TextureFormat::RGB_PVRTC1_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA_PVRTC1_2BPP, std::bind(parseCompressedTexture, TextureFormat::RGBA_PVRTC1_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA_PVRTC1_4BPP, std::bind(parseCompressedTexture, TextureFormat::RGBA_PVRTC1_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) }
};

TextureParser::TextureParser() :
    AbstractSerializerParser(),
    _textureHeaderSize(0),
    _dataEmbed(false)
{
}

void
TextureParser::parse(const std::string&                filename,
                     const std::string&                resolvedFilename,
                     std::shared_ptr<Options>          options,
                     const std::vector<unsigned char>& data,
                     std::shared_ptr<AssetLibrary>     assetLibrary)
{
    readHeader(filename, data);

    auto textureHeaderOffset = _headerSize + _dependenciesSize;
    auto textureBlobOffset = textureHeaderOffset + _textureHeaderSize;

    auto rawTextureHeaderData = std::vector<unsigned char>(data.begin() + textureHeaderOffset,
                                                           data.begin() + textureHeaderOffset + _textureHeaderSize);

    auto headerData = msgpack::type::tuple<
        msgpack::type::tuple<int, int, unsigned char, unsigned char>,
        std::vector<msgpack::type::tuple<int, int, int>>>();

    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, reinterpret_cast<const char*>(data.data() + textureHeaderOffset), _textureHeaderSize);

    auto header = unpacked.get().as<msgpack::type::tuple<
        msgpack::type::tuple<int, int, unsigned char, unsigned char>,
        std::vector<msgpack::type::tuple<int, int, int>>>>();

    const auto& textureHeader = header.a0;

    const auto textureWidth = textureHeader.a0;
    const auto textureHeight = textureHeader.a1;
    const auto textureType = textureHeader.a2 == 1 ? TextureType::Texture2D : TextureType::CubeTexture;
    const auto textureNumMipmaps = textureHeader.a3;

    const auto& formats = header.a1;

    auto contextAvailableTextureFormats = std::unordered_map<TextureFormat, unsigned int>();
    OpenGLES2Context::availableTextureFormats(contextAvailableTextureFormats);

    auto availableTextureFormats = std::unordered_multiset<TextureFormat>(contextAvailableTextureFormats.size());

    for (const auto& entry : contextAvailableTextureFormats)
    {
        LOG_DEBUG("available texture format: " << TextureFormatInfo::name(entry.first));

        availableTextureFormats.insert(entry.first);
    }

    for (const auto& entry : formats)
    {
        availableTextureFormats.insert(static_cast<TextureFormat>(entry.a0));
    }

    auto filteredAvailableTextureFormats = std::unordered_set<TextureFormat>(availableTextureFormats.size());

    for (auto textureFormat : availableTextureFormats)
    {
        if (availableTextureFormats.count(textureFormat) == 2)
            filteredAvailableTextureFormats.insert(textureFormat);
    }

    auto desiredFormat = options->textureFormatFunction()(filteredAvailableTextureFormats);

    auto desiredFormatInfo = *std::find_if(formats.begin(), formats.end(),
                                           [&](const msgpack::type::tuple<int, int, int>& entry) -> bool
    {
        return static_cast<TextureFormat>(entry.a0) == desiredFormat;
    });

    auto offset = textureBlobOffset + desiredFormatInfo.a1;
    auto length = desiredFormatInfo.a2;

    if (!_dataEmbed)
    {
        auto textureLoaderOptions = Options::create(options)
            ->seekingOffset(offset)
            ->seekedLength(length)
            ->parserFunction([&](const std::string& extension) -> AbstractParser::Ptr
        {
            return nullptr;
        });

        auto textureLoader = Loader::create(textureLoaderOptions);

        auto textureLoaderCompleteSlot = textureLoader->complete()->connect([&](Loader::Ptr loader) -> void
        {
            auto textureData = assetLibrary->blob(resolvedFilename);

            if (!_formatParserFunctions.at(desiredFormat)(filename, textureLoaderOptions, textureData, assetLibrary, textureWidth, textureHeight, textureType, textureNumMipmaps))
            {
                // TODO
                // handle parsing error
            }
        });

        textureLoader
            ->queue(resolvedFilename)
            ->load();
    }
    else
    {
        const auto textureDataBegin = data.begin() + offset;
        const auto textureDataEnd = textureDataBegin + length;
        const auto textureData = std::vector<unsigned char>(textureDataBegin, textureDataEnd);

        if (!_formatParserFunctions.at(desiredFormat)(filename, options, textureData, assetLibrary, textureWidth, textureHeight, textureType, textureNumMipmaps))
        {
            // TODO
            // handle parsing error
        }
    }

    complete()->execute(shared_from_this());
}

bool
TextureParser::parseRGBATexture(const std::string&                  fileName,
                                Options::Ptr                        options,
                                const std::vector<unsigned char>&   data,
                                AssetLibrary::Ptr                   assetLibrary,
                                int                                 width,
                                int                                 height,
                                render::TextureType                 type,
                                int                                 numMipmaps)
{
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, reinterpret_cast<const char*>(data.data()), data.size());

    auto deserializedTexture = unpacked.get().as<msgpack::type::tuple<int, std::vector<unsigned char>>>();

    auto imageFormat = static_cast<ImageFormat>(deserializedTexture.a0);

    auto parser = AbstractParser::Ptr();

    switch (imageFormat)
    {
    case ImageFormat::PNG:
        parser = PNGParser::create();
        break;

    default:
        return false;
    }

    parser->parse(fileName, fileName, options, deserializedTexture.a1, assetLibrary);

    return true;
}

bool
TextureParser::parseCompressedTexture(TextureFormat                        format,
                                      const std::string&                   fileName,
                                      Options::Ptr                         options,
                                      const std::vector<unsigned char>&    data,
                                      AssetLibrary::Ptr                    assetLibrary,
                                      int                                  width,
                                      int                                  height,
                                      render::TextureType                  type,
                                      int                                  numMipmaps)
{
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, reinterpret_cast<const char*>(data.data()), data.size());

    auto textureData = unpacked.get().as<std::vector<unsigned char>>();

    const auto hasMipmaps = options->generateMipmaps() && numMipmaps > 0;

    switch (type)
    {
    case TextureType::Texture2D:
    {
        auto texture = render::Texture::create(
            options->context(),
            width,
            height,
            hasMipmaps
        );

        texture->data(textureData.data(), format);
        texture->upload();

        if (hasMipmaps)
        {
            auto mipLevelOffset = std::max(
                TextureFormatInfo::mipLevelMinSize(format),
                static_cast<int>(
                    TextureFormatInfo::numBitsPerPixel(format) / 8.0f * width * height
                )
            );

            for (auto i = 1; i <= numMipmaps; ++i)
            {
                const auto mipLevelDimensionSize = width >> i;

                const auto mipLevelData = textureData.data() + mipLevelOffset;

                texture->uploadMipLevel(i, const_cast<unsigned char*>(mipLevelData));

                mipLevelOffset += std::max(
                    TextureFormatInfo::mipLevelMinSize(format),
                    static_cast<int>(
                        TextureFormatInfo::numBitsPerPixel(format) / 8.0f * mipLevelDimensionSize * mipLevelDimensionSize
                    )
                );
            }
        }

        assetLibrary->texture(fileName, texture);

        break;
    }
    case TextureType::CubeTexture:

        // TODO fixme

        return false;

    default:
        break;
    }

    return true;
}
