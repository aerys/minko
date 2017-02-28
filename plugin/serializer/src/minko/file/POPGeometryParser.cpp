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

#include "minko/data/Provider.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/AssetLocation.hpp"
#include "minko/file/LinkedAsset.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/POPGeometryAssetDescriptor.hpp"
#include "minko/file/POPGeometryParser.hpp"
#include "minko/file/StreamingOptions.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/log/Logger.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/VertexBuffer.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::deserialize;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::render;

POPGeometryParser::POPGeometryParser() :
    AbstractStreamedAssetParser(),
    _lodCount(0),
    _minLod(0),
    _maxLod(0),
    _minBound(),
    _maxBound(),
    _vertexSize(0),
    _vertexAttributes(),
    _lods(),
    _geometryIndexOffset(0),
    _geometryVertexOffset(0),
    _lodRequestNumPrimitivesLoaded(0),
    _lodRequestNumVerticesLoaded(0)
{
    assetExtension(0x00000056);
}

std::size_t
POPGeometryParser::lodRequestNumPrimitivesLoaded()
{
    return _lodRequestNumPrimitivesLoaded;
}

std::size_t
POPGeometryParser::lodRequestNumVerticesLoaded()
{
    return _lodRequestNumVerticesLoaded;
}

bool
POPGeometryParser::useDescriptor(const std::string&                 filename,
                                 Options::Ptr                       options,
                                 const std::vector<unsigned char>&  data,
                                 AssetLibrary::Ptr                  assetLibrary)
{
    auto descriptor = assetLibrary->assetDescriptor<POPGeometryAssetDescriptor>(filename);

    if (!descriptor)
        return false;

    _geometry = assetLibrary->geometry(filename);

    auto lowerLod = StreamingOptions::MAX_LOD;
    auto upperLod = 0;

    for (auto i = 0u; i < descriptor->numLodDescriptors(); ++i)
    {
        const auto& lodDescriptor = descriptor->lodDescriptor(i);

        lowerLod = math::min(lodDescriptor.level, lowerLod);
        upperLod = math::max(lodDescriptor.level, upperLod);

        auto lodInfo = LodInfo(
            lodDescriptor.level,
            lodDescriptor.level,
            lodDescriptor.numIndices,
            lodDescriptor.numVertices,
            lodDescriptor.dataOffset,
            lodDescriptor.dataLength
        );

        _lods.emplace(lodInfo.level, lodInfo);
    }

    lodParsed(
        lowerLod - 1,
        upperLod,
        data,
        options,
        options->disposeIndexBufferAfterLoading(),
        options->disposeVertexBufferAfterLoading()
    );

    return true;
}

void
POPGeometryParser::parsed(const std::string&                 filename,
                          const std::string&                 resolvedFilename,
                          Options::Ptr                       options,
                          const std::vector<unsigned char>&  data,
                          AssetLibrary::Ptr                  assetLibrary)
{
    _geometry = createPOPGeometry(assetLibrary, options, filename);

    if (_geometry == nullptr)
    {
        LOG_ERROR("failed to create geometry from " << filename);

        _error->execute(
            shared_from_this(),
            Error("POPGeometryParsingError", "geometry parsing error")
        );
    }

    if (!options->trackAssetDescriptor())
        return;

    auto assetDescriptor = assetLibrary->assetDescriptor<POPGeometryAssetDescriptor>(filename);

    if (!assetDescriptor)
    {
        assetDescriptor = POPGeometryAssetDescriptor::create()->location(AssetLocation(
            linkedAsset()->filename(),
            linkedAsset()->offset(),
            linkedAsset()->length()
        ));

        for (const auto& lod : _lods)
        {
            assetDescriptor->addLodDescriptor(POPGeometryAssetDescriptor::LodDescriptor {
                lod.first,
                lod.second.indexCount,
                lod.second.vertexCount,
                lod.second.blobOffset,
                lod.second.blobSize
            });
        }

        assetLibrary->assetDescriptor(filename, assetDescriptor);
    }
}

Geometry::Ptr
POPGeometryParser::createPOPGeometry(AssetLibrary::Ptr      assetLibrary,
                                     Options::Ptr           options,
                                     const std::string&     fileName)
{
    auto existingGeometry = assetLibrary->geometry(fileName);

    if (existingGeometry != nullptr)
        return existingGeometry;

    if (_lodCount <= 0)
        return Geometry::create(fileName);

    auto popGeometry = Geometry::create(fileName);

    popGeometry->data()->set<std::string>("type", "pop");

    auto indexCount = 0;
    auto vertexCount = 0;

    for (const auto& lod : _lods)
    {
        indexCount += lod.second.indexCount;
        vertexCount += lod.second.vertexCount;
    }

    auto indexBuffer = IndexBuffer::create(options->context());

    if (options->disposeIndexBufferAfterLoading())
    {
        indexBuffer->upload(0u, indexCount, std::vector<unsigned short>(indexCount, 0u));

        indexBuffer->disposeData();
    }
    else
    {
        indexBuffer->data().resize(indexCount, 0u);

        indexBuffer->upload();
    }

    popGeometry->indices(indexBuffer);

    auto vertexBuffers = std::vector<VertexBuffer::Ptr>(_numVertexBuffers);

    for (auto i = 0u; i < _numVertexBuffers; ++i)
    {
        vertexBuffers[i] = VertexBuffer::create(options->context());
    }

    for (const auto& attribute : _vertexAttributes)
    {
        const auto vertexBufferIndex = attribute.get<0>();

        const auto attributeName = attribute.get<1>();
        const auto attributeSize = attribute.get<2>();
        const auto attributeOffset = attribute.get<3>();

        vertexBuffers.at(vertexBufferIndex)->addAttribute(
            attributeName,
            attributeSize,
            attributeOffset
        );
    }

    for (auto vertexBuffer : vertexBuffers)
    {
        if (options->disposeVertexBufferAfterLoading())
        {
            vertexBuffer->upload(0u, vertexCount, std::vector<float>(vertexCount * vertexBuffer->vertexSize(), 0.f));

            vertexBuffer->disposeData();
        }
        else
        {
            vertexBuffer->data().resize(vertexCount * vertexBuffer->vertexSize(), 0.f);

            vertexBuffer->upload();
        }

        popGeometry->addVertexBuffer(vertexBuffer);
    }

    popGeometry->data()->set("popMinBound", _minBound);
    popGeometry->data()->set("popMaxBound", _maxBound);

    if (_isSharedPartition)
    {
        popGeometry->data()->set("isSharedPartition", true);

        popGeometry->data()->set("borderMinPrecision", _minBorderPrecision);
        popGeometry->data()->set("borderMaxDeltaPrecision", _maxDeltaBorderPrecision);
    }

    auto availableLods = std::map<int, ProgressiveOrderedMeshLodInfo>();

    for (const auto& levelToLodPair : _lods)
    {
        const auto& lod = levelToLodPair.second;

        availableLods.insert(std::make_pair(lod.level, ProgressiveOrderedMeshLodInfo(
            lod.level,
            lod.precisionLevel
        )));
    }

    if (availableLods.find(_fullPrecisionLod) == availableLods.end())
    {
        availableLods[_fullPrecisionLod] = ProgressiveOrderedMeshLodInfo(
            _fullPrecisionLod,
            _fullPrecisionLod
        );
    }

    if (data())
    {
        data()->set("availableLods", availableLods);
        data()->set("maxAvailableLod", 0);
    }

    popGeometry->data()->set("popFullPrecisionLod", float(_fullPrecisionLod));

    if (streamingOptions()->popGeometryFunction())
    {
        popGeometry = streamingOptions()->popGeometryFunction()(
            fileName,
            popGeometry
        );
    }

    assetLibrary->geometry(fileName, popGeometry);

    return popGeometry;
}

void
POPGeometryParser::headerParsed(const std::vector<unsigned char>&    data,
                                Options::Ptr                         options,
                                unsigned int&                        linkedAssetId)
{
    msgpack::type::tuple<
        unsigned int,
        int, int, int, int, std::string, int, int,
        std::vector<msgpack::type::tuple<
            unsigned int, std::string, unsigned int, unsigned int>
        >,
        bool, int, int,
        std::vector<msgpack::type::tuple<int, int, int, int, int, int>>
    > headerData;

    unpack(headerData, data, data.size(), 0u);

    auto size = data.size();

    linkedAssetId = headerData.get<0>();

    _lodCount = headerData.get<1>();
    _minLod = headerData.get<2>();
    _maxLod = headerData.get<3>();
    _fullPrecisionLod = headerData.get<4>();

    const auto& rawBounds = headerData.get<5>();
    auto bounds = TypeDeserializer::deserializeVector<float>(rawBounds);

    _minBound = glm::make_vec3(bounds.data());
    _maxBound = glm::make_vec3(bounds.data() + 3);

    _vertexSize = headerData.get<6>();

    _numVertexBuffers = headerData.get<7>();

    _vertexAttributes = headerData.get<8>();

    _isSharedPartition = headerData.get<9>();

    if (_isSharedPartition)
    {
        _minBorderPrecision = headerData.get<10>();
        _maxDeltaBorderPrecision = headerData.get<11>();
    }

    for (auto i = 0; i < _lodCount; ++i)
    {
        const auto& lodData = headerData.get<12>().at(i);

        auto level = lodData.get<0>();
        auto precisionLevel = lodData.get<1>();
        auto indexCount = lodData.get<2>();
        auto vertexCount = lodData.get<3>();
        auto blobOffset = lodData.get<4>();
        auto blobSize = lodData.get<5>();

        _lods.insert(std::make_pair(level, LodInfo(
            level,
            precisionLevel,
            indexCount,
            vertexCount,
            blobOffset,
            blobSize
        )));
    }
}

void
POPGeometryParser::lodParsed(int                                 previousLod,
                             int                                 currentLod,
                             const std::vector<unsigned char>&   data,
                             Options::Ptr                        options)
{
    lodParsed(
        previousLod,
        currentLod,
        data,
        options,
        options->disposeIndexBufferAfterLoading(),
        options->disposeVertexBufferAfterLoading()
    );
}

bool
POPGeometryParser::complete(int currentLod)
{
    return currentLod == _maxLod;
}

void
POPGeometryParser::completed()
{
    if (!data())
        return;

    auto availableLods = data()->get<std::map<int, ProgressiveOrderedMeshLodInfo>>("availableLods");

    auto& fullPrecisionLodInfo = availableLods.find(_fullPrecisionLod)->second;

    if (!fullPrecisionLodInfo.isValid())
    {
        const auto& maxLodInfo = availableLods.at(_maxLod);

        fullPrecisionLodInfo = ProgressiveOrderedMeshLodInfo(
            fullPrecisionLodInfo._level,
            fullPrecisionLodInfo._precisionLevel,
            maxLodInfo._indexOffset + maxLodInfo._indexCount,
            0
        );

        data()->set("availableLods", availableLods);
        data()->set("maxAvailableLod", fullPrecisionLodInfo._level);
    }
}

void
POPGeometryParser::lodParsed(int                                 previousLod,
                             int                                 currentLod,
                             const std::vector<unsigned char>&   data,
                             Options::Ptr                        options,
                             bool                                disposeIndexBuffer,
                             bool                                disposeVertexBuffer)
{
    const auto lodInfoRangeBeginIt = _lods.lower_bound(previousLod + 1);
    const auto lodInfoRangeEndIt = _lods.lower_bound(currentLod);
    const auto lodInfoRangeUpperBoundIt = _lods.upper_bound(currentLod);

    _lodRequestNumPrimitivesLoaded = 0;
    _lodRequestNumVerticesLoaded = 0;

    auto availableLods = this->data()
        ? this->data()->get<std::map<int, ProgressiveOrderedMeshLodInfo>>("availableLods")
        : std::map<int, ProgressiveOrderedMeshLodInfo>();

    auto dataOffset = 0u;

    for (auto lodInfoIt = lodInfoRangeBeginIt; lodInfoIt != lodInfoRangeUpperBoundIt; ++lodInfoIt)
    {
        const auto& lodInfo = lodInfoIt->second;

        const auto dataSize = lodInfo.blobSize;

        msgpack::type::tuple<std::string, std::vector<std::string>> lodData;

        unpack(lodData, data, dataSize, dataOffset);

        dataOffset += dataSize;

        auto indices = TypeDeserializer::deserializeVector<unsigned short>(lodData.get<0>());

        auto indexBuffer = _geometry->indices();

        const auto geometryIndexOffset = _geometryIndexOffset;
        _geometryIndexOffset += lodInfo.indexCount;

        if (disposeIndexBuffer)
        {
            indexBuffer->upload(geometryIndexOffset, lodInfo.indexCount, indices);
        }
        else
        {
            auto& indexData = indexBuffer->data();

            if (indexData.size() < geometryIndexOffset + indices.size())
                indexData.resize(geometryIndexOffset + indices.size());

            std::copy(indices.begin(), indices.end(), indexData.begin() + geometryIndexOffset);

            indexBuffer->upload(geometryIndexOffset, lodInfo.indexCount);
        }

        auto geometryVertexOffset = _geometryVertexOffset;

        _geometryVertexOffset += lodInfo.vertexCount;

        auto vertexBufferIndex = 0u;
        for (auto vertexBuffer : _geometry->vertexBuffers())
        {
            auto vertices = TypeDeserializer::deserializeVector<float>(lodData.get<1>().at(vertexBufferIndex));

            if (lodInfo.vertexCount > 0)
            {
                const auto localVertexOffset = geometryVertexOffset * vertexBuffer->vertexSize();

                if (disposeVertexBuffer)
                {
                    vertexBuffer->upload(geometryVertexOffset, lodInfo.vertexCount, vertices);
                }
                else
                {
                    auto& vertexData = vertexBuffer->data();

                    if (vertexData.size() < localVertexOffset + vertices.size())
                        vertexData.resize(localVertexOffset + vertices.size());

                    std::copy(
                        vertices.begin(),
                        vertices.end(),
                        vertexData.begin() + localVertexOffset
                    );

                    vertexBuffer->upload(geometryVertexOffset, lodInfo.vertexCount);
                }
            }

            ++vertexBufferIndex;
        }

        availableLods[lodInfo.level] = ProgressiveOrderedMeshLodInfo(
            lodInfo.level,
            lodInfo.precisionLevel,
            geometryIndexOffset,
            lodInfo.indexCount
        );

        _lodRequestNumPrimitivesLoaded += lodInfo.indexCount / 3;
        _lodRequestNumVerticesLoaded += lodInfo.vertexCount;
    }

    if (this->data())
    {
        this->data()->set("availableLods", availableLods);
        this->data()->set("maxAvailableLod", lodInfoRangeEndIt->second.level);
    }
}

void
POPGeometryParser::lodRangeFetchingBound(int  currentLod,
                                         int  requiredLod,
                                         int& lodRangeMinSize,
                                         int& lodRangeMaxSize,
                                         int& lodRangeRequestMinSize,
                                         int& lodRangeRequestMaxSize)

{
    if (streamingOptions()->popGeometryLodRangeFetchingBoundFunction())
    {
        streamingOptions()->popGeometryLodRangeFetchingBoundFunction()(
            currentLod,
            requiredLod,
            lodRangeMinSize,
            lodRangeMaxSize,
            lodRangeRequestMinSize,
            lodRangeRequestMaxSize
        );
    }
    else
    {
        lodRangeMinSize = StreamingOptions::MAX_LOD_RANGE;
    }
}

void
POPGeometryParser::lodRangeRequestByteRange(int lowerLod, int upperLod, int& offset, int& size) const
{
    auto lowerLodIt = _lods.lower_bound(lowerLod);
    auto upperLodIt = _lods.lower_bound(upperLod);

    const auto& lowerLodInfo = lowerLodIt->second;
    const LodInfo* upperLodInfo = nullptr;

    if (upperLodIt != _lods.end())
        upperLodInfo = &upperLodIt->second;
    else
        upperLodInfo = &_lods.rbegin()->second;

    offset = lowerLodInfo.blobOffset;
    size = (upperLodInfo->blobOffset + upperLodInfo->blobSize) - offset;
}

int
POPGeometryParser::lodLowerBound(int lod) const
{
    auto lodLowerIt = _lods.lower_bound(lod);

    const LodInfo* lodInfo = nullptr;

    if (lodLowerIt != _lods.end())
        lodInfo = &lodLowerIt->second;
    else
        lodInfo = &_lods.rbegin()->second;

    return lodInfo->level;
}

int
POPGeometryParser::maxLod() const
{
    return _maxLod;
}

