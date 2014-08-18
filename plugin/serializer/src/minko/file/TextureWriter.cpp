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
#include "minko/file/DDSImage.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/PNGWriter.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/Types.hpp"

#include "squish.h"

using namespace minko;
using namespace minko::file;
using namespace minko::render;

std::unordered_map<TextureFormat, TextureWriter::FormatWriterFunction> TextureWriter::_formatWriterFunctions = 
{
    { TextureFormat::RGB, std::bind(writeRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGBA, std::bind(writeRGBATexture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) },
    { TextureFormat::RGB_DXT1, std::bind(writeRGBDXT1Texture, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) }
};

TextureWriter::TextureWriter() :
    AbstractWriter<AbstractTexture::Ptr>(),
    _headerSize(0)
{
    _magicNumber = 0x0000004E | MINKO_SCENE_MAGIC_NUMBER;
}

std::string
TextureWriter::embed(AssetLibraryPtr     assetLibrary,
                     OptionsPtr          options,
                     Dependency::Ptr     dependency,
                     WriterOptionsPtr    writerOptions)
{
    const auto& textureFormats = writerOptions->textureFormats();

    std::stringstream headerStream;
    std::stringstream blobStream;

    auto headerData = std::vector<msgpack::type::tuple<int, int, int>>();

    for (auto textureFormat : textureFormats)
    {
        auto offset = blobStream.str().size();

        if (!_formatWriterFunctions.at(textureFormat)(_data, writerOptions, blobStream))
        {
            // TODO
            // handle error
        }

        auto length = blobStream.str().size() - offset;

        headerData.push_back(msgpack::type::make_tuple<int, int, int>(
            static_cast<int>(textureFormat),
            offset,
            length));
    }

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
TextureWriter::writeRGBDXT1Texture(AbstractTexture::Ptr abstractTexture,
                                   WriterOptions::Ptr writerOptions,
                                   std::stringstream& blob)
{
    auto texture = std::static_pointer_cast<Texture>(abstractTexture);

    auto width = texture->width();
    auto height = texture->height();

    auto flags = squish::kDxt1;

    auto compressedSize = squish::GetStorageRequirements(width, height, flags);
    auto textureData = std::vector<unsigned char>(compressedSize);

    squish::CompressImage(texture->data().data(), texture->width(), texture->height(), textureData.data(), flags);

    auto ddsImage = DDSImage();

    auto ddsOutputStream = std::stringstream();

    ddsImage.save(ddsOutputStream, texture, TextureFormat::RGB_DXT1, textureData);

    auto& ddsData = ddsOutputStream.str();
    
    msgpack::pack(blob, std::vector<unsigned char>(ddsData.begin(), ddsData.end()));

    return true;
}
