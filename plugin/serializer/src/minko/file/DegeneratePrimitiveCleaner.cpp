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

#include "minko/component/BoundingBox.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/DegeneratePrimitiveCleaner.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/math/SpatialIndex.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::render;
using namespace minko::scene;

DegeneratePrimitiveCleaner::DegeneratePrimitiveCleaner() :
    AbstractWriterPreprocessor<Node::Ptr>(),
    _statusChanged(StatusChangedSignal::create()),
    _progressRate(0.f),
    _options(defaultOptions())
{
}

DegeneratePrimitiveCleaner::Options
DegeneratePrimitiveCleaner::defaultOptions()
{
    auto options = Options();

    options.useMinPrecision = true;
    options.vertexMinPrecision = 1e-3f;

    return options;
}

void
DegeneratePrimitiveCleaner::process(Node::Ptr& node, AssetLibrary::Ptr assetLibrary)
{
    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(shared_from_this(), "DegeneratePrimitiveCleaner: start");

    auto geometrySet = std::unordered_set<Geometry::Ptr>();

    auto surfaceNodeSet = NodeSet::create(node)
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<Surface>(); });

    for (auto surfaceNode : surfaceNodeSet->nodes())
        for (auto surface : surfaceNode->components<Surface>())
            geometrySet.insert(surface->geometry());

    if (!geometrySet.empty())
    {
        _spatialIndex = math::SpatialIndex<unsigned int>::create(_options.vertexMinPrecision);

        auto geometryIndex = 0;

        for (auto geometry : geometrySet)
        {
            _progressRate = geometryIndex / float(geometrySet.size());

            processGeometry(geometry, assetLibrary);

            ++geometryIndex;
        }
    }

    _progressRate = 1.f;

    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(shared_from_this(), "DegeneratePrimitiveCleaner: stop");
}

void
DegeneratePrimitiveCleaner::processGeometry(GeometryPtr geometry, AssetLibraryPtr assetLibrary)
{
    const auto primitiveSize = 3u;

    const auto numVertices = geometry->numVertices();

    auto indices = geometry->indices();
    const auto numIndices = indices->numIndices();

    if (numIndices == 0u || (numIndices % primitiveSize) != 0u)
        return;

    const auto numPrimitives = numIndices / primitiveSize;

    const auto* u16Indices = indices->dataPointer<unsigned short>();
    const auto* u32Indices = indices->dataPointer<unsigned int>();

    auto degeneratePrimitives = std::unordered_set<unsigned int>();

    for (auto i = 0u; i < numIndices / primitiveSize; ++i)
    {
        auto primitive = std::vector<unsigned int>(primitiveSize, 0u);

        for (auto j = 0u; j < primitiveSize; ++j)
        {
            const auto index = u16Indices
                ? static_cast<unsigned int>(u16Indices->at(i * primitiveSize + j))
                : u32Indices->at(i * primitiveSize + j);

            primitive[j] = index;
        }

        auto degeneratePrimitive = false;

        for (auto j = 0u; j < primitiveSize; ++j)
        {
            if (primitive[j] >= numVertices)
            {
                degeneratePrimitive = true;
                degeneratePrimitives.insert(i);
                break;
            }
        }

        if (degeneratePrimitive)
            continue;

        // find degenerate primitive by index

        for (auto j = 0u; j < primitiveSize - 1; ++j)
        {
            for (auto k = 0u; k < primitiveSize; ++k)
            {
                if (j == k)
                    continue;

                if (primitive[j] != primitive[k])
                    continue;

                degeneratePrimitive = true;

                break;
            }

            if (degeneratePrimitive)
                break;
        }

        if (degeneratePrimitive)
        {
            degeneratePrimitives.insert(i);
            continue;
        }

        if (!_options.useMinPrecision)
            continue;

        // find degenerate primitive by precision

        _spatialIndex->clear();

        const auto positionVertexBuffer = geometry->vertexBuffer("position");
        const auto& positionVertexBufferData = positionVertexBuffer->data();
        const auto& positionVertexAttribute = positionVertexBuffer->attribute("position");

        for (auto j = 0u; j < primitiveSize; ++j)
        {
            const auto position = math::make_vec3(
                &positionVertexBufferData.at(primitive[j] * *positionVertexAttribute.vertexSize + positionVertexAttribute.offset)
            );

            ++(*_spatialIndex)[position];
        }

        if (_spatialIndex->size() < 3u)
        {
            degeneratePrimitive = true;
            degeneratePrimitives.insert(i);
        }
    }

    const auto numDegeneratePrimitives = degeneratePrimitives.size();

    if (numDegeneratePrimitives == 0u)
        return;

    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(
            shared_from_this(),
            "DegeneratePrimitiveCleaner: removing " + std::to_string(numDegeneratePrimitives) + " degenerate primitives"
        );

    auto newIndexBuffer = IndexBuffer::Ptr();

    if (numVertices <= static_cast<unsigned int>(std::numeric_limits<unsigned short>::max()))
    {
        newIndexBuffer = createIndexBuffer<unsigned short>(
            indices,
            primitiveSize,
            numPrimitives,
            degeneratePrimitives,
            assetLibrary
        );
    }
    else
    {
        newIndexBuffer = createIndexBuffer<unsigned int>(
            indices,
            primitiveSize,
            numPrimitives,
            degeneratePrimitives,
            assetLibrary
        );
    }

    geometry->indices(newIndexBuffer);
}

template <typename T>
IndexBuffer::Ptr
DegeneratePrimitiveCleaner::createIndexBuffer(IndexBuffer::Ptr                          indexBuffer,
                                              unsigned int                              primitiveSize,
                                              unsigned int                              numPrimitives,
                                              const std::unordered_set<unsigned int>&   degeneratePrimitives,
                                              AssetLibrary::Ptr                         assetLibrary)
{
    const auto numIndices = indexBuffer->numIndices();
    const auto numDegeneratePrimitives = degeneratePrimitives.size();

    const auto* u16Indices = indexBuffer->dataPointer<unsigned short>();
    const auto* u32Indices = indexBuffer->dataPointer<unsigned int>();

    const auto newNumIndices = numIndices - numDegeneratePrimitives * primitiveSize;

    auto newIndices = std::vector<T>(newNumIndices, 0u);

    auto primitiveOffset = 0u;
    for (auto i = 0u; i < numPrimitives; ++i)
    {
        if (degeneratePrimitives.find(i) != degeneratePrimitives.end())
            continue;

        for (auto j = 0u; j < primitiveSize; ++j)
        {
            newIndices[primitiveOffset * primitiveSize + j] = u16Indices
                ? static_cast<T>(u16Indices->at(i * primitiveSize + j))
                : static_cast<T>(u32Indices->at(i * primitiveSize + j));
        }

        ++primitiveOffset;
    }

    return IndexBuffer::create(assetLibrary->context(), newIndices);
}
