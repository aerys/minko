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

#include "minko/component/Surface.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/VertexColorSampler.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::material;
using namespace minko::render;
using namespace minko::scene;

VertexColorSampler::VertexColorSampler() :
    AbstractWriterPreprocessor<Node::Ptr>(),
    _statusChanged(StatusChangedSignal::create())
{
}

void
VertexColorSampler::process(Node::Ptr& node, AssetLibrary::Ptr assetLibrary)
{
    computeVertexColorAttributes(node, assetLibrary);
}

void
VertexColorSampler::computeVertexColorAttributes(Node::Ptr           node,
                                                 AssetLibrary::Ptr   assetLibrary)
{
    auto surfaceNodes = NodeSet::create(node)
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<Surface>(); });

    for (auto surfaceNode : surfaceNodes->nodes())
    {
        for (auto surface : surfaceNode->components<Surface>())
        {
            auto geometry = surface->geometry();
            auto material = surface->material();

            if (geometry->hasVertexAttribute("uv") &&
                material->data()->hasProperty("diffuseMap"))
            {
                computeVertexColorAttributes(geometry, material, assetLibrary);
            }
        }
    }
}

static
float
packColor(const math::vec3& color)
{
    return math::dot(color, math::vec3(1.f, 1.f / 255.f, 1.f / 65025.f));
}

void
VertexColorSampler::computeVertexColorAttributes(Geometry::Ptr       geometry,
                                                 Material::Ptr       material,
                                                 AssetLibrary::Ptr   assetLibrary)
{
    auto diffuseMap = std::static_pointer_cast<Texture>(assetLibrary->getTextureByUuid(
        material->data()->get<TextureSampler>("diffuseMap").uuid,
        true
    ));

    if (diffuseMap == nullptr)
        return;

    const auto format = diffuseMap->format();

    auto numComponents = 0u;

    if (format == TextureFormat::RGB)
        numComponents  = 3u;
    else if (format == TextureFormat::RGBA)
        numComponents = 4u;
    else
        return;

    const auto& textureData = diffuseMap->data();

    auto uvVertexBuffer = geometry->vertexBuffer("uv");
    const auto& uvVertexAttribute = uvVertexBuffer->attribute("uv");

    const auto numVertices = uvVertexBuffer->numVertices();

    auto colorVertexBuffer = VertexBuffer::Ptr();

    if (geometry->hasVertexAttribute("color"))
    {
        colorVertexBuffer = geometry->vertexBuffer("color");
    }

    auto colorVertexAttributeOffset = 0u;
    auto colorVertexAttributeSize = 1u;
    auto colorVertexBufferVertexSize = colorVertexAttributeSize;

    auto defaultColorVertexBufferData = std::vector<float>();

    float* colorVertexBufferData = nullptr;

    if (colorVertexBuffer != nullptr)
    {
        const auto& colorVertexAttribute = colorVertexBuffer->attribute("color");

        colorVertexAttributeOffset = colorVertexAttribute.offset;
        colorVertexAttributeSize = colorVertexAttribute.size;
        colorVertexBufferVertexSize = *colorVertexAttribute.vertexSize;

        colorVertexBufferData = colorVertexBuffer->data().data();
    }
    else
    {
        defaultColorVertexBufferData.resize(numVertices * colorVertexBufferVertexSize);

        colorVertexBufferData = defaultColorVertexBufferData.data();
    }

    for (auto i = 0u; i < numVertices; ++i)
    {
        const auto uvIndex = i * *uvVertexAttribute.vertexSize + uvVertexAttribute.offset;
        const auto colorIndex = i * colorVertexBufferVertexSize + colorVertexAttributeOffset;

        const auto uv = math::make_vec2(&uvVertexBuffer->data().at(uvIndex));

        auto color = math::vec4(0.f, 0.f, 0.f, 1.f);

        sampleColor(diffuseMap->width(), diffuseMap->height(), numComponents, textureData, uv, color);

        auto packedColor = math::fvec1(packColor(math::vec3(color)));

        for (auto j = 0u; j < colorVertexAttributeSize; ++j)
        {
            colorVertexBufferData[colorIndex + j] = packedColor[j];
        }
    }

    if (colorVertexBuffer == nullptr)
    {
        auto colorVertexBuffer = VertexBuffer::create(
            assetLibrary->context(),
            colorVertexBufferData,
            colorVertexBufferData + numVertices * colorVertexBufferVertexSize
        );

        colorVertexBuffer->addAttribute("color", colorVertexAttributeSize, colorVertexAttributeOffset);

        geometry->addVertexBuffer(colorVertexBuffer);
    }
}

void
VertexColorSampler::sampleColor(unsigned int                       width,
                                unsigned int                       height,
                                unsigned int                       numComponents,
                                const std::vector<unsigned char>&  textureData,
                                const math::vec2&                  uv,
                                math::vec4&                        color)
{
    auto normalizedU = math::fract(uv.s);
    auto normalizedV = math::fract(uv.t);

    normalizedU = normalizedU < 0.f ? 1.f - normalizedU : normalizedU;
    normalizedV = normalizedV < 0.f ? 1.f - normalizedV : normalizedV;

    const auto x = static_cast<unsigned int>(std::floor(normalizedU * width));
    const auto y = static_cast<unsigned int>(std::floor(normalizedV * height));

    const auto index = (x + y * width) * numComponents;

    for (auto i = 0u; i < numComponents; ++i)
    {
        color[i] = textureData.at(index + i) / 255.f;
    }
}
