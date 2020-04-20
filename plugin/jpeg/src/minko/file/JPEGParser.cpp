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

#include "JPEGParser.hpp"

#include "minko/file/Options.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/MipMapChainParser.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"

#include "jpgd.h"

using namespace minko;
using namespace minko::file;

void
JPEGParser::parse(const std::string&                filename,
                  const std::string&                resolvedFilename,
                  std::shared_ptr<Options>          options,
                  const std::vector<unsigned char>& data,
                  std::shared_ptr<AssetLibrary>     assetLibrary)
{
    int width;
    int height;
    int comps;

    // Loads a JPEG image from a memory buffer.
    // req_comps can be 1 (grayscale), 3 (RGB), or 4 (RGBA).
    // On return, width/height will be set to the image's dimensions, and actual_comps will be set
    // to either 1 (grayscale) or 3 (RGB).
    auto bmpData = jpgd::decompress_jpeg_image_from_memory(
        (const unsigned char*)&data[0], static_cast<int>(data.size()), &width, &height, &comps, 3
    );

    if (bmpData == nullptr)
    {
        error()->execute(shared_from_this(), Error("ParserError", "failed to decode JPEG file " + filename));

        return;
    }

    auto format = render::TextureFormat::RGBA;
    if (comps == 3 || comps == 1)
        format = render::TextureFormat::RGB;

    render::AbstractTexture::Ptr texture = nullptr;

    if (options->isCubeTexture())
    {
        MipMapChainParser parser;

        auto cubeTexture = parser.parseCubeTexture(
            options->context(),
            width,
            height,
            bmpData,
            options->parseMipMaps(),
            options->parseMipMaps() || options->generateMipmaps(),
            options->resizeSmoothly(),
            format,
            filename
        );

        cubeTexture = std::static_pointer_cast<render::CubeTexture>(options->textureFunction()(filename, cubeTexture));

        assetLibrary->cubeTexture(filename, cubeTexture);
        texture = cubeTexture;
    }
    else if (options->isRectangleTexture())
    {
        throw; // FIXME: handle rectangle textures
    }
    else
    {
        MipMapChainParser parser;

        auto texture2d = parser.parseTexture(
            options->context(),
            width,
            height,
            bmpData,
            options->parseMipMaps(),
            options->parseMipMaps() || options->generateMipmaps(),
            options->resizeSmoothly(),
            format,
            filename
        );

        texture2d = std::static_pointer_cast<render::Texture>(options->textureFunction()(filename, texture2d));

        texture = texture2d;
        assetLibrary->texture(filename, texture2d);
    }

    if (options->disposeTextureAfterLoading())
        texture->disposeData();

    free(bmpData);

    if (options->disposeTextureAfterLoading())
        texture->disposeData();

    complete()->execute(shared_from_this());
}
