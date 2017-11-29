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
#include "minko/file/UnusedVertexCleaner.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::render;
using namespace minko::scene;

UnusedVertexCleaner::UnusedVertexCleaner() :
    AbstractWriterPreprocessor<Node::Ptr>(),
    _statusChanged(StatusChangedSignal::create()),
    _progressRate(0.f)
{
}

void
UnusedVertexCleaner::process(Node::Ptr& node, AssetLibrary::Ptr assetLibrary)
{
    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(shared_from_this(), "UnusedVertexCleaner: start");

    auto geometrySet = std::unordered_set<Geometry::Ptr>();

    auto surfaceNodeSet = NodeSet::create(node)
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<Surface>(); });

    for (auto surfaceNode : surfaceNodeSet->nodes())
        for (auto surface : surfaceNode->components<Surface>())
            geometrySet.insert(surface->geometry());

    if (!geometrySet.empty())
    {
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
        statusChanged()->execute(shared_from_this(), "UnusedVertexCleaner: stop");
}

void
UnusedVertexCleaner::processGeometry(GeometryPtr geometry, AssetLibraryPtr assetLibrary)
{
    const auto numIndices = geometry->indices()->numIndices();
    const auto numVertices = geometry->numVertices();

    if (numVertices == 0u)
        return;

    auto indices = std::vector<unsigned int>();

    auto ushortIndexDataPointer = geometry->indices()->dataPointer<unsigned short>();

    if (ushortIndexDataPointer)
    {
        indices.resize(ushortIndexDataPointer->size());

        for (auto i = 0u; i < ushortIndexDataPointer->size(); ++i)
            indices[i] = static_cast<unsigned int>(ushortIndexDataPointer->at(i));
    }
    else
    {
        indices = *geometry->indices()->dataPointer<unsigned int>();
    }

    auto vertexUseCount = std::vector<unsigned int>(numVertices, 0u);

    for (auto i = 0u; i < numIndices; ++i)
    {
        const auto index = indices.at(i);

        if (index < numVertices)
            ++vertexUseCount[index];
    }

    auto indexMap = std::vector<unsigned int>(numVertices);
    auto currentNewIndex = 0u;

    for (auto i = 0u; i < numVertices; ++i)
    {
        const auto vertexUsed = vertexUseCount.at(i) > 0u;

        if (vertexUsed)
            indexMap[i] = currentNewIndex++;
        else
            indexMap[i] = currentNewIndex;
    }

    const auto newNumVertices = currentNewIndex;

    render::IndexBuffer::Ptr newIndexBuffer = nullptr;

    if (newNumVertices > std::numeric_limits<unsigned short>::max())
        newIndexBuffer = createIndexBuffer<unsigned int>(indices, indexMap, assetLibrary);
    else
        newIndexBuffer = createIndexBuffer<unsigned short>(indices, indexMap, assetLibrary);

    geometry->indices(newIndexBuffer);

    const auto vertexBuffers = geometry->vertexBuffers();
    auto newVertexBuffers = std::vector<render::VertexBuffer::Ptr>();

    for (auto vertexBuffer : vertexBuffers)
    {
        const auto& vertexBufferData = vertexBuffer->data();
        auto newVertexBufferData = std::vector<float>(newNumVertices * vertexBuffer->vertexSize(), 0.f);

        for (const auto& vertexAttribute : vertexBuffer->attributes())
        {
            auto currentNewIndex = 0u;
            for (auto i = 0u; i < numVertices; ++i)
            {
                const auto vertexUsed = vertexUseCount.at(i) > 0u;

                if (!vertexUsed)
                    continue;

                std::copy(
                    vertexBufferData.begin() + i * *vertexAttribute.vertexSize + vertexAttribute.offset,
                    vertexBufferData.begin() + i * *vertexAttribute.vertexSize + vertexAttribute.offset + vertexAttribute.size,
                    newVertexBufferData.begin() + currentNewIndex * *vertexAttribute.vertexSize + vertexAttribute.offset
                );

                ++currentNewIndex;
            }
        }

        auto newVertexBuffer = render::VertexBuffer::create(
            vertexBuffer->context(),
            newVertexBufferData
        );

        for (const auto& attribute : vertexBuffer->attributes())
            newVertexBuffer->addAttribute(*attribute.name, attribute.size, attribute.offset);

        newVertexBuffers.push_back(newVertexBuffer);
    }

    for (auto vertexBuffer : vertexBuffers)
        geometry->removeVertexBuffer(vertexBuffer);

    for (auto vertexBuffer : newVertexBuffers)
        geometry->addVertexBuffer(vertexBuffer);
}

template <typename T>
render::IndexBuffer::Ptr
UnusedVertexCleaner::createIndexBuffer(const std::vector<unsigned int>&    indices,
                                       const std::vector<unsigned int>&    indexMap,
                                       AssetLibrary::Ptr                   assetLibrary)
{
    const auto numIndices = indices.size();

    auto newIndices = std::vector<T>(numIndices, 0);
    for (auto i = 0u; i < numIndices; ++i)
        newIndices[i] = indexMap.at(indices[i]);

    return render::IndexBuffer::create(assetLibrary->context(), newIndices);
}
