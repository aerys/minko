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
#include "minko/file/PVRParser.hpp"
#include "minko/file/TextureParser.hpp"
#include "minko/file/TextureWriter.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/OpenGLES2Context.hpp"
#include "minko/render/Texture.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::render;
using namespace minko::serialize;

std::unordered_map<render::TextureFormat, TextureParser::FormatParserFunction> TextureParser::_formatParserFunctions =
{
    { TextureFormat::RGB, std::bind(parseRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    { TextureFormat::RGBA, std::bind(parseRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) },
    { TextureFormat::RGB_DXT1, std::bind(parsePVRTexture, TextureFormat::RGB_DXT1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4) }
};

TextureParser::TextureParser() :
    AbstractSerializerParser()
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

    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, reinterpret_cast<const char*>(data.data() + textureHeaderOffset), _textureHeaderSize);

    auto formats = unpacked.get().as<std::vector<msgpack::type::tuple<int, int, int>>>();

    auto contextAvailableTextureFormats = std::unordered_map<TextureFormat, unsigned int>();
    OpenGLES2Context::availableTextureFormats(contextAvailableTextureFormats);

    auto availableTextureFormats = std::unordered_multiset<TextureFormat>(contextAvailableTextureFormats.size());

    for (const auto& entry : contextAvailableTextureFormats)
    {
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

        if (!_formatParserFunctions.at(desiredFormat)(resolvedFilename, textureLoaderOptions, textureData, assetLibrary))
        {
            // TODO
            // handle parsing error
        }
    });

    textureLoader
        ->queue(resolvedFilename)
        ->load();

    complete()->execute(shared_from_this());
}

bool
TextureParser::parseRGBATexture(const std::string& fileName,
                                Options::Ptr options,
                                const std::vector<unsigned char>& data,
                                AssetLibrary::Ptr assetLibrary)
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
TextureParser::parsePVRTexture(TextureFormat                        format,
                               const std::string&                   fileName,
                               Options::Ptr                         options,
                               const std::vector<unsigned char>&    data,
                               AssetLibrary::Ptr                    assetLibrary)
{
    msgpack::unpacked unpacked;
    msgpack::unpack(&unpacked, reinterpret_cast<const char*>(data.data()), data.size());

    auto deserializedTexture = unpacked.get().as<std::vector<unsigned char>>();

    auto parser = PVRParser::create();

    parser->targetTextureFormat(format);

    parser->parse(fileName, fileName, options, deserializedTexture, assetLibrary);

    return true;
}
