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
#include "minko/file/VertexWelder.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/log/Logger.hpp"
#include "minko/math/SpatialIndex.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::scene;

const float VertexWelder::_defaultEpsilon = 1e-3f;

VertexWelder::VertexWelder() :
    AbstractWriterPreprocessor<Node::Ptr>(),
    _statusChanged(StatusChangedSignal::create()),
    _progressRate(0.f),
    _nodePredicateFunction([](Node::Ptr) -> bool { return true; })
{
}

void
VertexWelder::process(Node::Ptr& node, AssetLibrary::Ptr assetLibrary)
{
    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(shared_from_this(), "VertexWelder: start");

    auto surfaceNodes = NodeSet::create(node)
        ->descendants(true)
        ->where([this](Node::Ptr descendant) -> bool
            {
                return descendant->hasComponent<Surface>() &&
                    (!nodePredicateFunction() || nodePredicateFunction()(descendant)); 
            }
        );

    for (auto surfaceNode : surfaceNodes->nodes())
        for (auto surface : surfaceNode->components<Surface>())
            if (acceptsSurface(surface))
                weldSurfaceGeometry(surface);

    _progressRate = 1.f;

    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(shared_from_this(), "VertexWelder: stop");
}

bool
VertexWelder::acceptsSurface(Surface::Ptr surface)
{
    auto geometry = surface->geometry();

    if (_weldedGeometrySet.find(geometry) != _weldedGeometrySet.end())
        return false;

    auto data = geometry->data();

    if (!data->hasProperty("position"))
        return false;

    for (auto vertexBuffer : geometry->vertexBuffers())
    {
        for (const auto& vertexAttribute : vertexBuffer->attributes())
        {
            if (vertexAttribute.size == 0u ||
                vertexAttribute.size > 4u)
                return false;
        }
    }

    return true;
}

void
VertexWelder::weldSurfaceGeometry(Surface::Ptr surface)
{
    auto geometry = surface->geometry();

    auto spatialIndex = math::SpatialIndex<std::vector<unsigned int>>::create(_defaultEpsilon);

    buildSpatialIndex(geometry, spatialIndex);

    const auto expectedNumVertices = spatialIndex->size();

    if (expectedNumVertices == geometry->numVertices())
    {
        _weldedGeometrySet.insert(geometry);

        return;
    }

    auto weldedIndices = std::unordered_set<unsigned int>();

    auto indices = std::vector<unsigned int>();

    auto ushortIndexDataPointer = geometry->indices()->dataPointer<unsigned short>();
    std::vector<unsigned int>* indexDataPointer = nullptr;

    if (ushortIndexDataPointer)
    {
        indices.resize(ushortIndexDataPointer->size());

        for (auto i = 0u; i < ushortIndexDataPointer->size(); ++i)
            indices[i] = static_cast<unsigned int>(ushortIndexDataPointer->at(i));
    }
    else
    {
        indexDataPointer = geometry->indices()->dataPointer<unsigned int>();

        const auto& indexData = *indexDataPointer;

        indices.assign(indexData.begin(), indexData.end());
    }

    auto positionVertexBuffer = geometry->vertexBuffer("position");
    const auto& positionAttribute = positionVertexBuffer->attribute("position");
    const auto& positionData = positionVertexBuffer->data();

    auto vertexBufferToWeldedVertices = std::map<render::VertexBuffer::Ptr, std::vector<float>>();

    auto currentNewIndex = 0u;
    auto indexMap = std::vector<int>(geometry->numVertices(), -1);

    for (auto i = 0u; i < positionVertexBuffer->numVertices(); ++i)
    {
        const auto index = i;

        if (weldedIndices.find(index) != weldedIndices.end())
            continue;

        const auto position = math::make_vec3(&positionData.at(
            index * positionVertexBuffer->vertexSize() + positionAttribute.offset
        ));

        const auto& indicesToWeld = spatialIndex->at(position);

        const auto canWeldVertices = this->canWeldVertices(geometry, indicesToWeld);

        if (!canWeldVertices)
        {
            for (auto indexToWeld : indicesToWeld)
            {
                if (weldedIndices.find(indexToWeld) != weldedIndices.end())
                    continue;

                for (auto vertexBuffer : geometry->vertexBuffers())
                {
                    auto& weldedVertices = vertexBufferToWeldedVertices[vertexBuffer];

                    weldedVertices.resize(
                        weldedVertices.size() + vertexBuffer->vertexSize(),
                        0.f
                    );

                    const auto& vertexBufferData = vertexBuffer->data();

                    for (const auto& vertexAttribute : vertexBuffer->attributes())
                    {
                        if (vertexAttribute.size == 1u)
                        {
                            const auto result = vertexBufferData.at(
                                indexToWeld * vertexBuffer->vertexSize() + vertexAttribute.offset
                            );

                            weldedVertices[currentNewIndex * *vertexAttribute.vertexSize + vertexAttribute.offset] = result;
                        }
                        else if (vertexAttribute.size == 2u)
                        {
                            const auto result = math::make_vec2(&vertexBufferData.at(
                                indexToWeld * vertexBuffer->vertexSize() + vertexAttribute.offset
                            ));

                            const auto resultBegin = &result[0];
                            const auto resultEnd = resultBegin + vertexAttribute.size;

                            std::copy(
                                resultBegin,
                                resultEnd,
                                weldedVertices.begin() + currentNewIndex * *vertexAttribute.vertexSize + vertexAttribute.offset
                            );
                        }
                        else if (vertexAttribute.size == 3u)
                        {
                            const auto result = math::make_vec3(&vertexBufferData.at(
                                indexToWeld * vertexBuffer->vertexSize() + vertexAttribute.offset
                            ));

                            const auto resultBegin = &result[0];
                            const auto resultEnd = resultBegin + vertexAttribute.size;

                            std::copy(
                                resultBegin,
                                resultEnd,
                                weldedVertices.begin() + currentNewIndex * *vertexAttribute.vertexSize + vertexAttribute.offset
                            );
                        }
                        else if (vertexAttribute.size == 4u)
                        {
                            const auto result = math::make_vec4(&vertexBufferData.at(
                                indexToWeld * vertexBuffer->vertexSize() + vertexAttribute.offset
                            ));

                            const auto resultBegin = &result[0];
                            const auto resultEnd = resultBegin + vertexAttribute.size;

                            std::copy(
                                resultBegin,
                                resultEnd,
                                weldedVertices.begin() + currentNewIndex * *vertexAttribute.vertexSize + vertexAttribute.offset
                            );
                        }
                    }
                }

                indexMap[indexToWeld] = static_cast<int>(currentNewIndex);

                ++currentNewIndex;

                weldedIndices.insert(indexToWeld);
            }

            continue;
        }

        for (auto vertexBuffer : geometry->vertexBuffers())
        {
            auto& weldedVertices = vertexBufferToWeldedVertices[vertexBuffer];

            weldedVertices.resize(
                weldedVertices.size() + vertexBuffer->vertexSize(),
                0.f
            );
            
            const auto& vertexBufferData = vertexBuffer->data();

            for (const auto& vertexAttribute : vertexBuffer->attributes())
            {
                if (vertexAttribute.size == 1u)
                {
                    const auto result = weldAttribute<float>(
                        vertexAttribute,
                        vertexBufferData,
                        indicesToWeld,
                        [](const float* const data) -> float {  return *data; }
                    );

                    weldedVertices[currentNewIndex * *vertexAttribute.vertexSize + vertexAttribute.offset] = result;
                }
                else if (vertexAttribute.size == 2u)
                {
                    const auto result = weldAttribute<math::vec2>(
                        vertexAttribute,
                        vertexBufferData,
                        indicesToWeld,
                        math::make_vec2<float>
                    );

                    const auto resultBegin = &result[0];
                    const auto resultEnd = resultBegin + vertexAttribute.size;

                    std::copy(
                        resultBegin,
                        resultEnd,
                        weldedVertices.begin() + currentNewIndex * *vertexAttribute.vertexSize + vertexAttribute.offset
                    );
                }
                else if (vertexAttribute.size == 3u)
                {
                    auto result = !(vertexAttribute == positionAttribute)
                        ? weldAttribute<math::vec3>(
                            vertexAttribute,
                            vertexBufferData,
                            indicesToWeld,
                            math::make_vec3<float>
                        )
                        : position;

                    if (*vertexAttribute.name == "normal" ||
                        *vertexAttribute.name == "tangent")
                    {
                        result = math::normalize(result);
                    }

                    const auto resultBegin = &result[0];
                    const auto resultEnd = resultBegin + vertexAttribute.size;

                    std::copy(
                        resultBegin,
                        resultEnd,
                        weldedVertices.begin() + currentNewIndex * *vertexAttribute.vertexSize + vertexAttribute.offset
                    );
                }
                else if (vertexAttribute.size == 4u)
                {
                    const auto result = weldAttribute<math::vec4>(
                        vertexAttribute,
                        vertexBufferData,
                        indicesToWeld,
                        math::make_vec4<float>
                    );

                    const auto resultBegin = &result[0];
                    const auto resultEnd = resultBegin + vertexAttribute.size;

                    std::copy(
                        resultBegin,
                        resultEnd,
                        weldedVertices.begin() + currentNewIndex * *vertexAttribute.vertexSize + vertexAttribute.offset
                    );
                }
            }
        }

        for (auto weldedIndex : indicesToWeld)
        {
            weldedIndices.insert(weldedIndex);

            indexMap[weldedIndex] = static_cast<int>(currentNewIndex);
        }

        ++currentNewIndex;
    }

    for (auto i = 0u; i < indices.size(); ++i)
    {
        auto newIndex = indexMap.at(indices[i]);

        if (newIndex == -1)
            break;

        if (ushortIndexDataPointer)
            (*ushortIndexDataPointer)[i] = static_cast<unsigned short>(newIndex);
        else if (indexDataPointer)
            (*indexDataPointer)[i] = static_cast<unsigned int>(newIndex);
    }

    auto newVertexBuffers = std::vector<render::VertexBuffer::Ptr>();

    for (const auto& vertexBufferToWeldedVerticesPair : vertexBufferToWeldedVertices)
    {
        auto vertexBuffer = vertexBufferToWeldedVerticesPair.first;
        const auto& data = vertexBufferToWeldedVerticesPair.second;

        auto newVertexBuffer = render::VertexBuffer::create(
            vertexBuffer->context(),
            data
        );

        for (const auto& attribute : vertexBuffer->attributes())
            newVertexBuffer->addAttribute(*attribute.name, attribute.size, attribute.offset);

        geometry->removeVertexBuffer(vertexBuffer);

        newVertexBuffers.push_back(newVertexBuffer);
    }

    for (auto vertexBuffer : newVertexBuffers)
        geometry->addVertexBuffer(vertexBuffer);

    _weldedGeometrySet.insert(geometry);
}

void
VertexWelder::buildSpatialIndex(Geometry::Ptr geometry, math::SpatialIndex<std::vector<unsigned int>>::Ptr index)
{
    auto positionVertexBuffer = geometry->vertexBuffer("position");
    const auto& positionAttribute = positionVertexBuffer->attribute("position");

    const auto& data = positionVertexBuffer->data();

    for (auto i = 0u; i < positionVertexBuffer->numVertices(); ++i)
    {
        const auto position = math::make_vec3(
            &data.at(i * positionVertexBuffer->vertexSize() + positionAttribute.offset)
        );

        index->at(position).push_back(i);
    }
}

bool
VertexWelder::canWeldVertices(Geometry::Ptr                     geometry,
                              const std::vector<unsigned int>&  indices)
{
    if (indices.size() <= 1u)
        return false;

    auto permutations = std::vector<bool>(indices.size());
    std::fill(permutations.begin() + indices.size() - 2u, permutations.end(), true);

    auto pairs = std::vector<std::pair<unsigned int, unsigned int>>();

    auto currentPairSize = 0u;
    do
    {
        for (auto i = 0u; i < indices.size(); ++i)
        {
            if (permutations.at(i))
            {
                if (currentPairSize == 0u)
                {
                    pairs.emplace_back(0u, 0u);
                    pairs.back().first = indices.at(i);
                    ++currentPairSize;
                }
                else
                {
                    pairs.back().second = indices.at(i);
                    ++currentPairSize;
                    break;
                }
            }
        }

        if (currentPairSize >= 2u)
        {
            currentPairSize = 0u;

            continue;
        }

    } while (std::next_permutation(permutations.begin(), permutations.end()));

    for (auto vertexBuffer : geometry->vertexBuffers())
    {
        for (const auto& vertexAttribute : vertexBuffer->attributes())
        {
            if (*vertexAttribute.name == "position")
                continue;

            for (const auto& pair : pairs)
            {
                if (vertexAttribute.size == 1u)
                {
                    if (!canWeldAttribute<float>(
                        vertexAttribute,
                        vertexBuffer->data(),
                        pair,
                        [](const float* const data) -> float { return *data; },
                        _scalarAttributeWeldablePredicateFunction))
                        return false;
                }
                else if (vertexAttribute.size == 2u)
                {
                    if (!canWeldAttribute<math::vec2>(
                        vertexAttribute,
                        vertexBuffer->data(),
                        pair,
                        math::make_vec2<float>,
                        _vec2AttributeWeldablePredicateFunction))
                        return false;
                }
                else if (vertexAttribute.size == 3u)
                {
                    if (!canWeldAttribute<math::vec3>(
                        vertexAttribute,
                        vertexBuffer->data(),
                        pair,
                        math::make_vec3<float>,
                        _vec3AttributeWeldablePredicateFunction))
                        return false;
                }
                else if (vertexAttribute.size == 4u)
                {
                    if (!canWeldAttribute<math::vec4>(
                        vertexAttribute,
                        vertexBuffer->data(),
                        pair,
                        math::make_vec4<float>,
                        _vec4AttributeWeldablePredicateFunction))
                        return false;
                }
            }
        }
    }

    return true;
}
