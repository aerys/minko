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
#include "minko/file/DDSImage.hpp"
#include "minko/file/DDSParser.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/Texture.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::render;

DDSParser::DDSParser() :
    AbstractParser()
{
}

void
DDSParser::parse(const std::string&                filename,
                 const std::string&                resolvedFilename,
                 std::shared_ptr<Options>          options,
                 const std::vector<unsigned char>& data,
                 std::shared_ptr<AssetLibrary>     assetLibrary)
{
    auto targetTextureFormat = options->textureFormat();

    auto ddsImage = DDSImage();

    unsigned int width = 0;
    unsigned int height = 0;

    unsigned int size = 0;

    auto textureType = TextureType::Texture2D;
    auto textureFormat = TextureFormat::RGBA;

    auto textureData = std::vector<unsigned char>();

    if (!ddsImage.load(data, textureData, width, height, size, textureType, textureFormat))
    {
        LOG_ERROR("DDSParser: fail to load DDS file `" << resolvedFilename << "'");

        return;
    }

    if (targetTextureFormat != textureFormat)
    {
        LOG_ERROR("DDSParser: target format mismatched (" <<
                  static_cast<int>(targetTextureFormat) <<
                  ", " <<
                  static_cast<int>(textureFormat) <<
                  ")");

        return;
    }

    auto texture = render::Texture::create(options->context(), width, height, options->generateMipmaps());

    texture->data(textureData.data(), targetTextureFormat);
    texture->upload();

    assetLibrary->texture(filename, texture);

    complete()->execute(shared_from_this());
}
