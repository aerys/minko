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

#include "minko/StreamingOptions.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/LinkedAsset.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/POPGeometryWriter.hpp"
#include "minko/file/WriterOptions.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/scene/Node.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::serialize;

const int POPGeometryWriter::_fullPrecisionLevel = 32;

const bool POPGeometryWriter::_keepSplitVertexPattern = false;

const int POPGeometryWriter::_defaultMinPrecisionLevel = 0;
const int POPGeometryWriter::_defaultMaxPrecisionLevel = 12;

POPGeometryWriter::POPGeometryWriter() :
    AbstractWriter<Geometry::Ptr>(),
    _assetLibrary(),
    _geometry(),
    _rangeFunction(std::bind(
        &POPGeometryWriter::defaultRangeFunction,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4)
    )
{
    _magicNumber = 0x00000056 | MINKO_SCENE_MAGIC_NUMBER;
}

std::string
POPGeometryWriter::embed(AssetLibrary::Ptr              assetLibrary,
                         Options::Ptr                   options,
                         Dependency::Ptr                dependency,
                         WriterOptions::Ptr             writerOptions,
                         std::vector<unsigned char>&    embeddedHeaderData)
{
    auto geometry = data();

    _assetLibrary = assetLibrary;
    _options = options;
    _geometry = geometry;

    const auto& rangeFunction = _streamingOptions->popGeometryWriterLodRangeFunction()
        ? _streamingOptions->popGeometryWriterLodRangeFunction()
        : _rangeFunction;

    rangeFunction(geometry, nullptr, _minLevel, _maxLevel);

    auto lodData = std::map<int, LodData>();

    buildLodData(lodData, _minLevel, _maxLevel);

    updateBoundaryLevels(lodData);

    auto blobData = std::vector<unsigned char>();

    serializeGeometry(
        dependency,
        writerOptions,
        lodData,
        embeddedHeaderData,
        blobData
    );

    return std::string(blobData.begin(), blobData.end());
}

POPGeometryWriter::QuantizationIndex
POPGeometryWriter::quantify(const math::vec3&   position,
                            int                 level,
                            int                 maxLevel,
                            const math::vec3&   minBound,
                            const math::vec3&   maxBound,
                            const math::vec3&   boxSize,
                            bool                outputQuantizedPosition,
                            math::vec3&         quantizedPosition)
{
    auto segmentCount = std::pow(2, level);

    const auto offset = position - minBound;
    
    auto index = QuantizationIndex(static_cast<int>(offset[0] * segmentCount / boxSize.x),
                                   static_cast<int>(offset[1] * segmentCount / boxSize.y),
                                   static_cast<int>(offset[2] * segmentCount / boxSize.z));

    if (outputQuantizedPosition)
    {
        float quantizedPositionData[3] =
        {
            static_cast<float>((std::get<0>(index) + 0.5f) * boxSize.x / segmentCount + minBound.x),
            static_cast<float>((std::get<1>(index) + 0.5f) * boxSize.y / segmentCount + minBound.y),
            static_cast<float>((std::get<2>(index) + 0.5f) * boxSize.z / segmentCount + minBound.z)
        };

        quantizedPosition = math::vec3(quantizedPositionData[0],
                                 quantizedPositionData[1],
                                 quantizedPositionData[2]);
    }

    return index;
}

void
POPGeometryWriter::defaultRangeFunction(Geometry::Ptr       geometry,
                                        BoundingBox::Ptr    boundingBox,
                                        int&                minLevel,
                                        int&                maxLevel)
{
    minLevel = _defaultMinPrecisionLevel;
    maxLevel = _defaultMaxPrecisionLevel;
}

void
POPGeometryWriter::buildLodData(std::map<int, LodData>& lodData,
                                int                     minLevel,
                                int                     maxLevel)
{
    auto assetLibrary = _assetLibrary;

    auto geometry = _geometry;

    auto positionVertexBuffer = geometry->vertexBuffer("position");

    const auto& positionAttribute = positionVertexBuffer->attribute("position");
    auto positionAttributeSize = positionAttribute.size;
    auto positionAttributeOffset = positionAttribute.offset;

    auto vertexSize = positionVertexBuffer->vertexSize();

    auto vertices = positionVertexBuffer->data();
    auto indices = geometry->indices()->data();

    auto minBound = math::vec3();
    auto maxBound = math::vec3();

    auto precisionLevelBias = 0;

    auto partitioningIsActive = geometry->data()->hasProperty("partitioningMaxDepth");
    auto isSharedPartition = geometry->data()->hasProperty("isSharedPartition") &&
                             geometry->data()->get<bool>("isSharedPartition");

    auto partitioningDepth = 0;
    auto partitioningMaxDepth = 0;

    if (partitioningIsActive)
    {
        partitioningMaxDepth = geometry->data()->get<int>("partitioningMaxDepth");
        partitioningDepth = geometry->data()->get<int>("partitioningDepth");

        minBound = geometry->data()->get<math::vec3>("partitioningMinBound");
        maxBound = geometry->data()->get<math::vec3>("partitioningMaxBound");
    }
    else
    {
        auto node = scene::Node::create()
            ->addComponent(Transform::create())
            ->addComponent(Surface::create(geometry, _options->material(), _options->effect()))
            ->addComponent(BoundingBox::create());

        auto box = node->component<BoundingBox>()->box();

        minBound = box->bottomLeft();
        maxBound = box->topRight();
    }

    auto orderedBufferMap = std::map<int, std::vector<unsigned short>>();

    auto levelToPrecisionLevelMap = std::unordered_map<int, int>();

    levelToPrecisionLevelMap.insert(std::make_pair(_fullPrecisionLevel, _fullPrecisionLevel));

    _minBound = minBound;
    _maxBound = maxBound;

    auto boxSize = maxBound - minBound;

    static const auto minBoxSize = 1.0E-7f;
    boxSize = glm::max(math::vec3(minBoxSize), boxSize);

    for (auto level = maxLevel; level >= minLevel - 1; --level)
    {
        auto& currentOrderedBuffer = orderedBufferMap[level == maxLevel ? _fullPrecisionLevel : level + 1];

        auto remainingIndices = std::vector<unsigned short>();

        auto precisionLevel = std::max(level - partitioningDepth + partitioningMaxDepth + precisionLevelBias,
                                       minLevel);

        levelToPrecisionLevelMap.insert(std::make_pair(level, precisionLevel));

        for (auto i = 0; i < indices.size(); i += 3)
        {
            unsigned short triangle[3] = { indices.at(i), indices.at(i + 1), indices.at(i + 2) };
            QuantizationIndex quantizedTriangle[3];

            for (int j = 0; j < 3; ++j)
            {
                auto vertexIndex = triangle[j];

                auto vertexPositionOffset = vertexIndex * vertexSize + positionAttributeOffset;

                auto vertexPosition = math::vec3(
                    vertices[vertexPositionOffset + 0],
                    vertices[vertexPositionOffset + 1],
                    vertices[vertexPositionOffset + 2]
                );

                auto actualLevel = precisionLevel;

                math::vec3 quantizedPosition;
                auto quantizationIndex = quantify(vertexPosition, actualLevel, maxLevel, minBound, maxBound, boxSize, false, quantizedPosition);

                quantizedTriangle[j] = quantizationIndex;
            }

            auto triangleIsDegenerate =
                quantizedTriangle[0] == quantizedTriangle[1] ||
                quantizedTriangle[0] == quantizedTriangle[2] ||
                quantizedTriangle[1] == quantizedTriangle[2];

            auto& targetTriangleContainer = triangleIsDegenerate ? currentOrderedBuffer : remainingIndices;

            for (auto j = 0; j < 3; ++j)
            {
                auto index = triangle[j];

                targetTriangleContainer.push_back(index);
            }
        }

        indices = remainingIndices;
    }

    if (!indices.empty())
    {
        auto level = minLevel;

        auto& orderedBuffer = orderedBufferMap[level];

        orderedBuffer.insert(orderedBuffer.end(), indices.begin(), indices.end());
    }

    unsigned short currentOrderedIndex = 0;

    auto indexToOrderedIndexMap = std::unordered_map<unsigned short, unsigned short>();

    for (const auto& orderedBufferEntry : orderedBufferMap)
    {
        const auto level = orderedBufferEntry.first;
        const auto& orderedBuffer = orderedBufferEntry.second;

        if (orderedBuffer.empty())
            continue;

        auto lodDataIt = lodData.insert(std::make_pair(level, LodData(levelToPrecisionLevelMap.at(level))));

        auto& orderedIndexBuffer = lodDataIt.first->second.indices;
        auto& orderedVertexBuffers = lodDataIt.first->second.vertices;

        orderedVertexBuffers.resize(geometry->vertexBuffers().size());
        lodDataIt.first->second.vertexSizes.resize(geometry->vertexBuffers().size());

        auto vertexBufferIndex = 0;
        for (auto vertexBuffer : geometry->vertexBuffers())
        {
            lodDataIt.first->second.vertexSizes[vertexBufferIndex] = vertexBuffer->vertexSize();

            ++vertexBufferIndex;
        }

        for (auto index : orderedBuffer)
        {
            auto indexToOrderedIndexIt = indexToOrderedIndexMap.find(index);

            if (indexToOrderedIndexIt == indexToOrderedIndexMap.end())
            {
                auto orderedIndex = currentOrderedIndex++;

                indexToOrderedIndexMap.insert(std::make_pair(index, orderedIndex));

                orderedIndexBuffer.push_back(orderedIndex);

                auto vertexBufferIndex = 0;
                for (auto vertexBuffer : geometry->vertexBuffers())
                {
                    const auto localVertexSize = vertexBuffer->vertexSize();
                    const auto& localVertices = vertexBuffer->data();

                    auto vertexBegin = localVertices.begin() + (index * localVertexSize);
                    auto vertexEnd = vertexBegin + localVertexSize;

                    auto& orderedVertexBuffer = orderedVertexBuffers.at(vertexBufferIndex);

                    orderedVertexBuffer.insert(orderedVertexBuffer.end(), vertexBegin, vertexEnd);

                    ++vertexBufferIndex;
                }
            }
            else
            {
                auto orderedIndex = indexToOrderedIndexIt->second;

                orderedIndexBuffer.push_back(orderedIndex);
            }
        }
    }
}

void
POPGeometryWriter::serializeGeometry(Dependency::Ptr                dependency,
                                     WriterOptions::Ptr             writerOptions,
                                     const std::map<int, LodData>&  lodData,
                                     std::vector<unsigned char>&    headerData,
                                     std::vector<unsigned char>&    blobData)
{
    msgpack::sbuffer headerBuffer;
    msgpack::sbuffer blobBuffer;

    auto linkedAsset = _linkedAsset;
    
    serializeHeader(headerBuffer, blobBuffer, lodData);

    const auto headerSize = headerBuffer.size();

    headerData.push_back((headerSize & 0xff00) >> 8);
    headerData.push_back((headerSize & 0x00ff));

    headerData.insert(headerData.end(), headerBuffer.data(), headerBuffer.data() + headerSize);

    if (linkedAsset != nullptr &&
        linkedAsset->linkType() == LinkedAsset::LinkType::Internal)
    {
        linkedAsset
            ->length(blobBuffer.size())
            ->data(std::vector<unsigned char>(blobBuffer.data(), blobBuffer.data() + blobBuffer.size()));
    }
    else
    {
        blobData.insert(blobData.end(), blobBuffer.data(), blobBuffer.data() + blobBuffer.size());
    }
}

void
POPGeometryWriter::serializeHeader(msgpack::sbuffer&                    headerBuffer,
                                   msgpack::sbuffer&                    blobBuffer,
                                   const std::map<int, LodData>&        lodData)
{
    auto geometry = _geometry;

    auto vertexSize = geometry->vertexSize();

    auto minLevel = _minLevel;
    auto maxLevel = _maxLevel;

    auto fullPrecisionLevel = _fullPrecisionLevel;

    auto minBound = _minBound;
    auto maxBound = _maxBound;

    auto levelCount = lodData.size();

    const auto numVertexBuffers = geometry->vertexBuffers().size();

    auto vertexAttributes = std::vector<msgpack::type::tuple<unsigned int, std::string, unsigned int, unsigned int>>();

    auto vertexBufferIndex = 0u;
    for (auto vertexBuffer : geometry->vertexBuffers())
    {
        for (const auto& attribute : vertexBuffer->attributes())
        {
            vertexAttributes.push_back(
                msgpack::type::tuple<unsigned int, std::string, unsigned int, unsigned int>(
                    vertexBufferIndex,
                    *attribute.name,
                    attribute.size,
                    attribute.offset
                )
            );
        }

        ++vertexBufferIndex;
    }

    auto bounds = std::vector<float>
    {
        minBound.x,
        minBound.y,
        minBound.z,
        maxBound.x,
        maxBound.y,
        maxBound.z
    };

    auto isSharedPartition = false;
    auto borderMinPrecision = 0;
    auto borderMaxDeltaPrecision = 0;

    if (geometry->data()->hasProperty("isSharedPartition") &&
        geometry->data()->get<bool>("isSharedPartition"))
    {
        isSharedPartition = true;

        borderMinPrecision = geometry->data()->get<int>("borderMinPrecision");
        borderMaxDeltaPrecision = geometry->data()->get<int>("borderMaxDeltaPrecision");
    }

    std::vector<msgpack::type::tuple<int, int, int, int, int, int>> lodInfo;

    msgpack::type::tuple<
        unsigned int,
        int, int, int, int, std::string, int, int,
        std::vector<msgpack::type::tuple<
            unsigned int, std::string, unsigned int, unsigned int>
        >,
        bool, int, int,
        std::vector<msgpack::type::tuple<int, int, int, int, int, int>>
    > headerData(
        _linkedAssetId,
        levelCount,
        minLevel,
        maxLevel,
        fullPrecisionLevel,
        TypeSerializer::serializeVector(bounds),
        vertexSize,
        numVertexBuffers,
        vertexAttributes,
        isSharedPartition,
        borderMinPrecision,
        borderMaxDeltaPrecision,
        lodInfo
    );

    auto levels = std::list<int>();

    for (const auto& lod : lodData)
        levels.push_back(lod.first);

    levels.sort();

    for (auto level : levels)
        serializeLod(headerData.get<12>(), blobBuffer, level, lodData.at(level));

    msgpack::pack(headerBuffer, headerData);
}

void
POPGeometryWriter::serializeLod(std::vector<msgpack::type::tuple<int, int, int, int, int, int>>&    lodHeaders,
                                msgpack::sbuffer&                                                   blobBuffer,
                                int                                                                 level,
                                const LodData&                                                      lod)
{
    const auto& lodIndices = lod.indices;
    const auto& lodVertices = lod.vertices;

    const auto previousBlobBufferSize = blobBuffer.size();

    msgpack::type::tuple<std::string, std::vector<std::string>> lodData;

    lodData.get<0>() = TypeSerializer::serializeVector(lodIndices);

    for (const auto& lodVertexBuffer : lodVertices)
        lodData.get<1>().push_back(TypeSerializer::serializeVector(lodVertexBuffer));

    msgpack::pack(blobBuffer, lodData);

    const auto levelBlobOffset = previousBlobBufferSize;
    const auto levelBlobSize = blobBuffer.size() - previousBlobBufferSize;

    const auto indexCount = lodIndices.size();
    const auto vertexCount = lodVertices.front().size() / lod.vertexSizes.front();

    lodHeaders.emplace_back(
        level, lod.precisionLevel, indexCount, vertexCount, levelBlobOffset, levelBlobSize
    );
}

void
POPGeometryWriter::updateBoundaryLevels(const std::map<int, LodData>& lodData)
{
    auto minLevel = INT_MAX;
    auto maxLevel = 0;

    for (const auto& levelToLodData : lodData)
    {
        if (levelToLodData.first < minLevel)
            minLevel = levelToLodData.first;

        if (levelToLodData.first > maxLevel)
            maxLevel = levelToLodData.first;
    }

    _minLevel = minLevel;
    _maxLevel = maxLevel;
}
