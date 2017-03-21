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
#include "minko/deserialize/Unpacker.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/OpenGLES2Context.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::render;
using namespace minko::serialize;
using namespace minko::deserialize;

const int TextureParser::TEXTURE_HEADER_SIZE_BYTE_SIZE = 2;

std::unordered_map<render::TextureFormat, TextureParser::FormatParserFunction, Hash<TextureFormat>> TextureParser::_formatParserFunctions =
{
    { TextureFormat::RGB, std::bind(parseRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA, std::bind(parseRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },

    { TextureFormat::RGB_DXT1, std::bind(parseCompressedTexture, TextureFormat::RGB_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA_DXT1, std::bind(parseCompressedTexture, TextureFormat::RGBA_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA_DXT3, std::bind(parseCompressedTexture, TextureFormat::RGBA_DXT3, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA_DXT5, std::bind(parseCompressedTexture, TextureFormat::RGBA_DXT5, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },

    { TextureFormat::RGB_ETC1, std::bind(parseCompressedTexture, TextureFormat::RGB_ETC1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },

    { TextureFormat::RGB_PVRTC1_2BPP, std::bind(parseCompressedTexture, TextureFormat::RGB_PVRTC1_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGB_PVRTC1_4BPP, std::bind(parseCompressedTexture, TextureFormat::RGB_PVRTC1_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA_PVRTC1_2BPP, std::bind(parseCompressedTexture, TextureFormat::RGBA_PVRTC1_2BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA_PVRTC1_4BPP, std::bind(parseCompressedTexture, TextureFormat::RGBA_PVRTC1_4BPP, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },

    { TextureFormat::RGB_ATITC, std::bind(parseCompressedTexture, TextureFormat::RGB_ATITC, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) },
    { TextureFormat::RGBA_ATITC, std::bind(parseCompressedTexture, TextureFormat::RGBA_ATITC, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8) }
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
    readHeader(filename, data, 0x00000054);

    if (_textureHeaderSize == 0)
    {
        const auto& versionInfo = SceneVersionInfo::getInfoByVersion(_version);
        const auto assetHeaderSize = MINKO_SCENE_HEADER_SIZE + versionInfo.numDependenciesBytes() + TEXTURE_HEADER_SIZE_BYTE_SIZE;
        const auto textureHeaderSizeOffset = assetHeaderSize - TEXTURE_HEADER_SIZE_BYTE_SIZE;

        std::stringstream headerDataStream(std::string(
            data.begin() + textureHeaderSizeOffset,
            data.begin() + textureHeaderSizeOffset + TEXTURE_HEADER_SIZE_BYTE_SIZE
        ));

        headerDataStream.read(reinterpret_cast<char*>(&_textureHeaderSize), TEXTURE_HEADER_SIZE_BYTE_SIZE);
    }

    auto textureHeaderOffset = _headerSize + _dependencySize + TEXTURE_HEADER_SIZE_BYTE_SIZE;
    auto textureBlobOffset = textureHeaderOffset + _textureHeaderSize;

    typedef msgpack::type::tuple<
        msgpack::type::tuple<int, int, unsigned char, unsigned char>,
        std::vector<msgpack::type::tuple<int, int, int>>>
    HeaderType;

    HeaderType header;
    unpack(header, data, _textureHeaderSize, textureHeaderOffset);

    const auto& textureHeader = header.get<0>();

    const auto textureWidth = textureHeader.get<0>();
    const auto textureHeight = textureHeader.get<1>();
    const auto textureType = textureHeader.get<2>() == 1 ? TextureType::Texture2D : TextureType::CubeTexture;
    const auto textureNumMipmaps = textureHeader.get<3>();

    const auto& formats = header.get<1>();

    const auto& contextAvailableTextureFormats = OpenGLES2Context::availableTextureFormats();

    auto availableTextureFormats = std::unordered_multiset<TextureFormat, Hash<TextureFormat>>(
        contextAvailableTextureFormats.size()
    );

    for (const auto& entry : contextAvailableTextureFormats)
    {
        LOG_DEBUG("platform-supported texture format: " << TextureFormatInfo::name(entry.first));
        availableTextureFormats.insert(entry.first);
    }

    for (const auto& entry : formats)
    {
        LOG_DEBUG("embedded texture format: " << TextureFormatInfo::name(static_cast<TextureFormat>(entry.get<0>())));
        availableTextureFormats.insert(static_cast<TextureFormat>(entry.get<0>()));
    }

    auto filteredAvailableTextureFormats = std::unordered_set<TextureFormat, Hash<TextureFormat>>(availableTextureFormats.size());

    for (auto textureFormat : availableTextureFormats)
    {
        if (availableTextureFormats.count(textureFormat) == 2)
            filteredAvailableTextureFormats.insert(textureFormat);
    }

    auto desiredFormat = options->textureFormatFunction()(filteredAvailableTextureFormats);

    auto desiredFormatInfo = *std::find_if(formats.begin(), formats.end(),
                                           [&](const msgpack::type::tuple<int, int, int>& entry) -> bool
    {
        return static_cast<TextureFormat>(entry.get<0>()) == desiredFormat;
    });

    auto offset = textureBlobOffset + desiredFormatInfo.get<1>();
    auto length = desiredFormatInfo.get<2>();

    if (!_dataEmbed)
    {
        auto textureFileOptions = options->clone()
            ->seekingOffset(offset)
            ->seekedLength(length)
            ->loadAsynchronously(false)
            ->storeDataIfNotParsed(false)
            ->parserFunction([](const std::string& extension) -> AbstractParser::Ptr { return nullptr; });

        auto loader = Loader::create();

        loader->options(textureFileOptions);

        auto errorSlot = loader->error()->connect([&](Loader::Ptr, const Error& error)
        {
            _error->execute(
                shared_from_this(),
                Error("TextureLoadingError", std::string("Failed to load texture ") + filename)
            );
        });

        auto completeSlot = loader->complete()->connect([&](Loader::Ptr loaderThis)
        {
            const auto& textureData = loaderThis->files().at(filename)->data();

            if (!_formatParserFunctions.at(desiredFormat)(filename, textureFileOptions, textureData, assetLibrary, textureWidth, textureHeight, textureType, textureNumMipmaps))
            {
                _error->execute(
                    shared_from_this(),
                    Error("TextureParsingError", std::string("Failed to parse texture ") + filename)
                );
            }
        });

        loader
            ->queue(filename)
            ->load();
    }
    else
    {
        const auto textureDataBegin = data.begin() + offset;
        const auto textureDataEnd = textureDataBegin + length;
        const auto textureData = std::vector<unsigned char>(textureDataBegin, textureDataEnd);

        if (!_formatParserFunctions.at(desiredFormat)(filename, options, textureData, assetLibrary, textureWidth, textureHeight, textureType, textureNumMipmaps))
        {
            _error->execute(
                shared_from_this(),
                Error("TextureParsingError", std::string("Failed to parse texture ") + filename)
            );
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
    msgpack::type::tuple<int, std::string> deserializedTexture;
    unpack(deserializedTexture, data, data.size());

    auto parsingOptions = options->parserFunction()
        ? options->clone()->parserFunction(nullptr)
        : options;

    auto imageFormat = static_cast<ImageFormat>(deserializedTexture.get<0>());

    auto parser = AbstractParser::Ptr();

    switch (imageFormat)
    {
    case ImageFormat::PNG:
        parser = parsingOptions->getParser("png");
        break;

    case ImageFormat::JPEG:
        parser = parsingOptions->getParser("jpg");
        break;

    default:
        return false;
    }

    parser->parse(
        fileName,
        fileName,
        parsingOptions,
        std::vector<unsigned char>(deserializedTexture.get<1>().begin(), deserializedTexture.get<1>().end()),
        assetLibrary
    );

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
    const auto& textureData = data;

    const auto hasMipmaps = options->generateMipmaps() && numMipmaps > 0;

    switch (type)
    {
    case TextureType::Texture2D:
    {
        auto texture = render::Texture::create(
            options->context(),
            width,
            height,
            hasMipmaps,
            false,
            false,
            format,
            fileName
        );

        const auto storeTextureData = !options->disposeTextureAfterLoading();

        if (storeTextureData)
            texture->data(const_cast<unsigned char*>(textureData.data()));

        texture->upload();

        if (!storeTextureData)
            texture->uploadMipLevel(0, const_cast<unsigned char*>(textureData.data()));

        if (hasMipmaps)
        {
            auto mipLevelOffset = TextureFormatInfo::textureSize(format, width, height);

            for (auto i = 1; i < numMipmaps; ++i)
            {
                const auto mipLevelWidth = width >> i;
                const auto mipLevelHeight = height >> i;

                const auto mipLevelData = textureData.data() + mipLevelOffset;

                texture->uploadMipLevel(i, const_cast<unsigned char*>(mipLevelData));

                mipLevelOffset += TextureFormatInfo::textureSize(format, mipLevelWidth, mipLevelHeight);
            }
        }

        assetLibrary->texture(fileName, texture);

        if (options->disposeTextureAfterLoading())
            texture->disposeData();

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
