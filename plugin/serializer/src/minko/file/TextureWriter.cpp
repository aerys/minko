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

#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/Types.hpp"

using namespace minko;
using namespace minko::file;

TextureWriter::TextureWriter() :
    _extension()
{
}

void
TextureWriter::extension(const std::string& extension)
{
    _extension = extension;
}

void
TextureWriter::writeRawTexture(std::string&                     filename,
                               std::shared_ptr<AssetLibrary>    assetLibrary,
                               std::shared_ptr<Options>         options,
                               std::shared_ptr<WriterOptions>   writerOptions)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    if (file)
    {
        auto dependencies               = Dependency::create();
        auto serializedData             = embed(assetLibrary, options, dependencies, writerOptions);
        auto serializedDependencies     = dependencies->serialize(assetLibrary, options, writerOptions);

        auto res                        = serializedData;

        file.write(res.c_str(), res.size());
        file.close();
    }
    else
        std::cerr << "File " << filename << " can't be opened" << std::endl;

    complete()->execute(this->shared_from_this());
}

std::string
TextureWriter::embed(AssetLibraryPtr                    assetLibrary,
                     OptionsPtr                         options,
                     DependencyPtr                      dependency,
                     std::shared_ptr<WriterOptions>     writerOptions)
{
    auto texture = std::dynamic_pointer_cast<render::Texture>(data());

    auto textureFormat = /* texture->format(); */ render::TextureFormat::RGBA;

    uint componentCount = { };

    switch (textureFormat)
    {
        case render::TextureFormat::RGB:
            componentCount = 3;
            break;

        case render::TextureFormat::RGBA:
            componentCount = 4;
            break;

        default:
            break;
    }
	
	return "";
	
    /*auto textureData = std::vector<unsigned char> { };

    {
        auto devilWriter = file::DevILWriter::create();

        devilWriter->writeToStream(textureData,
                                   _extension,
                                   texture->data(),
                                   texture->width(),
                                   texture->height(),
                                   texture->width(),
                                   texture->height(),
                                   componentCount);
    }

    auto textureContent = std::string(textureData.begin(), textureData.end());

    return textureContent;*/
}
