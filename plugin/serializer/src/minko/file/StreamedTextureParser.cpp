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

#include "minko/data/Provider.hpp"
#include "minko/deserialize/Unpacker.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/PNGParser.hpp"
#include "minko/file/StreamedTextureParser.hpp"
#include "minko/file/StreamingOptions.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/OpenGLES2Context.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::deserialize;
using namespace minko::file;
using namespace minko::render;

StreamedTextureParser::StreamedTextureParser() :
    AbstractStreamedAssetParser(),
    _texture(),
    _textureType(TextureType::Texture2D),
    _textureFormat(TextureFormat::RGBA),
    _textureWidth(0),
    _textureHeight(0),
    _textureNumFaces(0),
    _textureNumMipmaps(0),
    _mipLevelsInfo()
{
    assetExtension(0x00000055);
}

bool
StreamedTextureParser::useDescriptor(const std::string&                 filename,
                                     Options::Ptr                       options,
                                     const std::vector<unsigned char>&  data,
                                     AssetLibrary::Ptr                  assetLibrary)
{
    return false;
}

void
StreamedTextureParser::parsed(const std::string&                 filename,
                              const std::string&                 resolvedFilename,
                              Options::Ptr                       options,
                              const std::vector<unsigned char>&  data,
                              AssetLibrary::Ptr                  assetLibrary)
{
    _texture = createTexture(assetLibrary, filename, _textureType);

    if (_texture == nullptr)
    {
        LOG_ERROR("failed to create texture from " << filename);

        _error->execute(
            shared_from_this(),
            Error("StreamedTextureParsingError", "streamed texture parsing error")
        );

        return;
    }

    if (deferParsing())
    {
        auto& textureReference = _dependency->getTextureReference(dependencyId());

        textureReference.texture = _texture;

        for (auto materialData : textureReference.dependentMaterialDataSet)
            materialData->set(textureReference.textureType, textureReference.texture->sampler());

        streamingOptions()->masterLodScheduler()->deferredTextureReady(
            this->data(),
            textureReference.dependentMaterialDataSet,
            textureReference.textureType,
            textureReference.texture
        );
    }
}

AbstractTexture::Ptr
StreamedTextureParser::createTexture(AssetLibrary::Ptr     assetLibrary,
                                     const std::string&    filename,
                                     render::TextureType   textureType)
{
    switch (textureType)
    {
    case TextureType::Texture2D:
    {
        const auto width = _textureWidth;
        const auto height = _textureHeight;

        auto texture2d = Texture::create(
            assetLibrary->context(),
            width,
            height,
            true,
            false,
            false,
            _textureFormat,
            filename
        );

        _texture = texture2d;

        texture2d->upload();

        if (streamingOptions()->streamedTextureFunction())
        {
            texture2d = std::static_pointer_cast<Texture>(streamingOptions()->streamedTextureFunction()(
                filename,
                texture2d
            ));
        }

        assetLibrary->texture(filename, texture2d);

        data()->set("size", math::vec2(width, height));
        data()->set("maxAvailableLod", 0);
        data()->set("maxLod", maxLod());

        break;
    }
    case TextureType::CubeTexture:

        return nullptr;
    }

    return _texture;
}

void
StreamedTextureParser::headerParsed(const std::vector<unsigned char>&    data,
                                    Options::Ptr                         options,
                                    unsigned int&                        linkedAssetId)
{
    auto header = msgpack::type::tuple<
        unsigned int,
        msgpack::type::tuple<int, int, unsigned char, unsigned char>,
        std::vector<msgpack::type::tuple<int, std::vector<msgpack::type::tuple<int, int>>>>>();

    unpack(header, data, data.size(), 0u);

    linkedAssetId = header.get<0>();

    _textureWidth = header.get<1>().get<0>();
    _textureHeight = header.get<1>().get<1>();
    _textureNumFaces = header.get<1>().get<2>();
    _textureNumMipmaps = header.get<1>().get<3>();
    _textureType = _textureNumFaces == 1 ? TextureType::Texture2D : TextureType::CubeTexture;

    auto availableTextureFormats = std::list<TextureFormat>();

    for (const auto& formatHeader : header.get<2>())
        availableTextureFormats.push_back(static_cast<TextureFormat>(formatHeader.get<0>()));

    _textureFormat = matchingTextureFormat(options, availableTextureFormats);

    const auto formatHeader = *std::find_if(
        header.get<2>().begin(),
        header.get<2>().end(),
        [this](const msgpack::type::tuple<int, std::vector<msgpack::type::tuple<int, int>>>& entry) -> bool
    {
        return entry.get<0>() == static_cast<int>(_textureFormat);
    });

    for (const auto& mipLevel : formatHeader.get<1>())
        _mipLevelsInfo.push_back(std::make_tuple(mipLevel.get<0>(), mipLevel.get<1>()));
}

void
StreamedTextureParser::lodParsed(int                                 previousLod,
                                 int                                 currentLod,
                                 const std::vector<unsigned char>&   data,
                                 Options::Ptr                        options)
{
    auto dataOffset = 0u;

    for (auto lod = previousLod + 1; lod <= currentLod; ++lod)
    {
        const auto mipLevel = lodToMipLevel(lod);

        const auto& mipLevelInfo = _mipLevelsInfo.at(mipLevel);

        auto mipLevelDataSize = std::get<1>(mipLevelInfo);
        dataOffset += mipLevelDataSize;

        auto mipLevelData = data.data() + data.size() - dataOffset;

        auto extractedLodData = std::vector<unsigned char>();

        if (extractLodData(
            _textureFormat,
            "",
            options,
            options->assetLibrary(),
            DataChunk(mipLevelData, 0u, mipLevelDataSize),
            extractedLodData
        ))
        {
            mipLevelData = extractedLodData.data();
            mipLevelDataSize = extractedLodData.size();
        }

        switch (_textureType)
        {
        case TextureType::Texture2D:
        {
            auto texture2d = std::static_pointer_cast<Texture>(_texture);

            texture2d->uploadMipLevel(
                mipLevel,
                const_cast<unsigned char*>(mipLevelData)
            );

            if (mipLevel == 0)
            {
                const auto storeTextureData = !options->disposeTextureAfterLoading();

                if (storeTextureData)
                {
                    texture2d->data(const_cast<unsigned char*>(mipLevelData));
                }
            }

            break;
        }
        case TextureType::CubeTexture:

            break;
        }
    }

    this->data()->set("maxAvailableLod", currentLod);
}

TextureFormat
StreamedTextureParser::matchingTextureFormat(std::shared_ptr<Options>                  options,
                                             const std::list<render::TextureFormat>&   availableTextureFormats)
{
    const auto& contextAvailableTextureFormats = OpenGLES2Context::availableTextureFormats();

    auto availableTextureFormatMatches = std::unordered_multiset<TextureFormat, Hash<TextureFormat>>(contextAvailableTextureFormats.size());

    for (const auto& textureFormatToContextFormat : contextAvailableTextureFormats)
    {
        availableTextureFormatMatches.insert(textureFormatToContextFormat.first);
    }

    for (const auto& textureFormat : availableTextureFormats)
    {
        availableTextureFormatMatches.insert(textureFormat);
    }

    auto filteredAvailableTextureFormats = std::unordered_set<TextureFormat, Hash<TextureFormat>>(availableTextureFormatMatches.size());

    for (auto textureFormat : availableTextureFormatMatches)
    {
        if (availableTextureFormatMatches.count(textureFormat) == 2)
            filteredAvailableTextureFormats.insert(textureFormat);
    }

    return options->textureFormatFunction()(filteredAvailableTextureFormats);
}

bool
StreamedTextureParser::complete(int currentLod)
{
    return lodToMipLevel(currentLod) == 0;
}

void
StreamedTextureParser::completed()
{
}

int
StreamedTextureParser::lodToMipLevel(int lod) const
{
    return (_textureNumMipmaps - 1) - lod;
}

bool
StreamedTextureParser::extractLodData(TextureFormat                        format,
                                      const std::string&                   filename,
                                      std::shared_ptr<Options>             options,
                                      std::shared_ptr<AssetLibrary>        assetLibrary,
                                      const DataChunk&                     lodData,
                                      std::vector<unsigned char>&          extractedLodData)
{
    if (TextureFormatInfo::isCompressed(format))
        return false;

    switch (format)
    {
    case TextureFormat::RGB:
    case TextureFormat::RGBA:
    {
        auto localAssetLibrary = AssetLibrary::create(assetLibrary->context());
        auto parser = PNGParser::create();

        auto parserOptions = options->clone()
            ->disposeTextureAfterLoading(false);

        parser->parse(
            filename,
            filename,
            parserOptions,
            std::vector<unsigned char>(lodData.data + lodData.offset, lodData.data + lodData.size),
            localAssetLibrary
        );

        auto mipLevelTexture = localAssetLibrary->texture(filename);

        extractedLodData = mipLevelTexture->data();

        mipLevelTexture->dispose();

        break;
    }
    default:
        return false;
    }

    return true;
}

void
StreamedTextureParser::lodRangeFetchingBound(int  currentLod,
                                             int  requiredLod,
                                             int& lodRangeMinSize,
                                             int& lodRangeMaxSize,
                                             int& lodRangeRequestMinSize,
                                             int& lodRangeRequestMaxSize)

{
    if (streamingOptions()->streamedTextureLodRangeFetchingBoundFunction())
    {
        streamingOptions()->streamedTextureLodRangeFetchingBoundFunction()(
            currentLod,
            requiredLod,
            lodRangeMinSize,
            lodRangeMaxSize,
            lodRangeRequestMinSize,
            lodRangeRequestMaxSize
        );
    }
    else
    {
        lodRangeMinSize = StreamingOptions::MAX_LOD_RANGE;
    }
}

void
StreamedTextureParser::lodRangeRequestByteRange(int lowerLod, int upperLod, int& offset, int& size) const
{
    const auto& nextLodLowerBoundInfo = _mipLevelsInfo.at(lodToMipLevel(lowerLod));
    const auto& nextLodUpperBoundInfo = _mipLevelsInfo.at(lodToMipLevel(upperLod));

    offset = std::get<0>(nextLodUpperBoundInfo);
    size = std::get<0>(nextLodLowerBoundInfo) + std::get<1>(nextLodLowerBoundInfo) - offset;
}

int
StreamedTextureParser::lodLowerBound(int lod) const
{
    return lod;
}

int
StreamedTextureParser::maxLod() const
{
    return _textureNumMipmaps - 1;
}
