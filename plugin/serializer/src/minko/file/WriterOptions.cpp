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

#include "minko/file/WriterOptions.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/MipFilter.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::material;
using namespace minko::math;
using namespace minko::render;
using namespace minko::serialize;

const unsigned int WriterOptions::EmbedMode::None       = 0u;
const unsigned int WriterOptions::EmbedMode::Geometry   = 1u << 0;
const unsigned int WriterOptions::EmbedMode::Material   = 1u << 1;
const unsigned int WriterOptions::EmbedMode::Texture    = 1u << 2;
const unsigned int WriterOptions::EmbedMode::All        = Geometry | Material | Texture;

WriterOptions::WriterOptions() :
    _addBoundingBoxes(false),
    _embedMode(EmbedMode::All),
    _geometryNameFunction([](const std::string& str) -> std::string { return str; }),
    _materialNameFunction([](const std::string& str) -> std::string { return str; }),
    _textureNameFunction([](const std::string& str) -> std::string { return str; }),
    _geometryUriFunction([](const std::string& str) -> std::string { return str; }),
    _materialUriFunction([](const std::string& str) -> std::string { return str; }),
    _textureUriFunction([](const std::string& str) -> std::string { return str; }),
    _geometryFunction([](const std::string& filename, Geometry::Ptr geometry) -> Geometry::Ptr { return geometry; }),
    _materialFunction([](const std::string& filename, Material::Ptr material) -> Material::Ptr { return material; }),
    _textureFunction([](const std::string& filename, AbstractTexture::Ptr texture) -> AbstractTexture::Ptr { return texture; }),
    _textureFormats(),
    _compressedTextureExceptions(),
    _textureOptions{
        { "",               { ImageFormat::JPEG, 0.9f, true, 0.f, false, true,  true, true, math::vec2(1.f), math::ivec2(2048), TextureFilter::LINEAR,  MipFilter::LINEAR } },
        { "lightMap",       { ImageFormat::JPEG, 0.9f, true, 0.f, false, true,  true, true, math::vec2(1.f), math::ivec2(2048), TextureFilter::LINEAR,  MipFilter::LINEAR } },
        { "radianceMap",    { ImageFormat::JPEG, 0.9f, true, 0.f, true,  true,  true, true, math::vec2(1.f), math::ivec2(2048), TextureFilter::NEAREST, MipFilter::NONE } },
        { "irradianceMap",  { ImageFormat::JPEG, 0.9f, true, 0.f, false, false, true, true, math::vec2(1.f), math::ivec2(2048), TextureFilter::NEAREST, MipFilter::NONE } }
    },
    _writeAnimations(false),
    _nullAssetUuids()
{
}
