/*
Copyright (c) 2013 Aerys

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

#include "minko/file/AbstractWriter.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/PNGWriter.hpp"
#include "minko/Types.hpp"

using namespace minko;
using namespace minko::file;

TextureWriter::TextureWriter() :
    _imageFormat()
{
}

void
TextureWriter::data(TexturePtr data)
{
    _data = data;
}

void
TextureWriter::imageFormat(serialize::ImageFormat imageFormat)
{
    _imageFormat = imageFormat;
}

void
TextureWriter::writeRawTexture(std::string&     filename,
                               AssetLibraryPtr  assetLibrary,
                               OptionsPtr       options,
                               WriterOptionsPtr writerOptions)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    if (file)
    {
        auto serializedData = embedTexture(assetLibrary, options, writerOptions);

        file.write(serializedData.c_str(), serializedData.size());
        file.close();
    }
    else
        std::cerr << "File " << filename << " can't be opened" << std::endl;
}

std::string
TextureWriter::embedTexture(AssetLibraryPtr      assetLibrary,
                            OptionsPtr           options,
                            WriterOptionsPtr     writerOptions)
{
    auto extension = serialize::extensionFromImageFormat(_imageFormat);

    auto texture = std::dynamic_pointer_cast<render::Texture>(_data);

    auto textureData = std::vector<unsigned char>();

    switch (_imageFormat)
    {
    case serialize::ImageFormat::PNG:
    {
        auto writer = PNGWriter::create();

        writer->writeToStream(textureData, texture->data(), texture->width(), texture->height());

        break;
    }
    default:
        throw WriterError("UnsupportedOutputImageFormat", "No writer found for extension: '" + extension + "'");
    }

    auto textureContent = std::string(textureData.begin(), textureData.end());

    return textureContent;
}
