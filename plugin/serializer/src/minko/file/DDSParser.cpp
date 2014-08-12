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
#include "minko/file/DDSParser.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/Texture.hpp"

#include "ddsloader.h"

using namespace minko;
using namespace minko::file;
using namespace minko::render;

DDSParser::DDSParser() :
    AbstractParser(),
    _textureFormat(TextureFormat::RGBA)
{
}

void
DDSParser::parse(const std::string&                filename,
                 const std::string&                resolvedFilename,
                 std::shared_ptr<Options>          options,
                 const std::vector<unsigned char>& data,
                 std::shared_ptr<AssetLibrary>     assetLibrary)
{
    static const auto ddsFormatToTextureFormat = std::unordered_map<unsigned int, TextureFormat>
    {
        { DDS_LOADER_FORMAT_DXT1, TextureFormat::RGB_DXT1 },
        { DDS_LOADER_FORMAT_DXT3, TextureFormat::RGBA_DXT3 },
        { DDS_LOADER_FORMAT_DXT5, TextureFormat::RGBA_DXT5 }
    };

    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int format = 0;

    auto textureData = std::vector<unsigned char>();

    if (!loadDDS(data, textureData, format, width, height))
    {
        LOG_ERROR("DDSParser: fail to load DDS file `" << resolvedFilename << "'");

        return;
    }

    auto loadedTextureFormat = ddsFormatToTextureFormat.at(format);

    if (_textureFormat != loadedTextureFormat)
    {
        LOG_ERROR("DDSParser: target format mismatched (" <<
                  static_cast<int>(_textureFormat) <<
                  ", " <<
                  static_cast<int>(loadedTextureFormat) <<
                  ")");

        return;
    }

    auto texture = render::Texture::create(options->context(), width, height, options->generateMipmaps());

    texture->data(textureData.data(), _textureFormat);
    texture->upload();

    assetLibrary->texture(filename, texture);

    complete()->execute(shared_from_this());
}
