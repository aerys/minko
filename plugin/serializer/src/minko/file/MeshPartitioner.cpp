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
#include "minko/data/HalfEdgeCollection.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/MeshPartitioner.hpp"
#include "minko/file/StreamingOptions.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/math/UnorderedSpatialIndex.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/log/Logger.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::material;
using namespace minko::render;
using namespace minko::scene;

MeshPartitioner::Options::Options() :
    maxNumTrianglesPerNode(60000),
    maxNumIndicesPerNode(65536),
    maxNumSurfacesPerSurfaceBucket(255),
    maxNumTrianglesPerSurfaceBucket(3000000),
    flags(Options::none),
    partitionMaxSizeFunction(defaultPartitionMaxSizeFunction),
    worldBoundsFunction(defaultWorldBoundsFunction),
    nodeFilterFunction(defaultNodeFilterFunction),
    surfaceIndexer(defaultSurfaceIndexer()),
    validSurfacePredicate(defaultValidSurfacePredicate),
    instanceSurfacePredicate(defaultInstanceSurfacePredicate)
{
}

MeshPartitioner::SurfaceIndexer
MeshPartitioner::defaultSurfaceIndexer()
{
    auto surfaceIndexer = SurfaceIndexer();

    surfaceIndexer.hash = [](Surface::Ptr surface) -> std::size_t
    {
        auto hashValue = std::size_t();

        auto material = surface->material();

        if (defaultInstanceSurfacePredicate(surface) ||
            (material->data()->hasProperty("zSorted") && material->data()->get<bool>("zSorted")))
            minko::hash_combine<std::string, std::hash<std::string>>(hashValue, surface->uuid());

        minko::hash_combine<Material::Ptr, std::hash<Material::Ptr>>(hashValue, surface->material());

        for (const auto& vertexBuffer : surface->geometry()->vertexBuffers())
            for (const auto& attribute : vertexBuffer->attributes())
                minko::hash_combine<std::string, std::hash<std::string>>(hashValue, *attribute.name);

        return hashValue;
    };

    surfaceIndexer.equal = [](Surface::Ptr left, Surface::Ptr right) -> bool
    {
        auto leftGeometry = left->geometry();
        auto rightGeometry = right->geometry();

        auto leftMaterial = left->material();
        auto rightMaterial = right->material();

        if (defaultInstanceSurfacePredicate(left) || defaultInstanceSurfacePredicate(right))
            return false;

        if ((leftMaterial->data()->hasProperty("zSorted") && leftMaterial->data()->get<bool>("zSorted")) ||
            (rightMaterial->data()->hasProperty("zSorted") && rightMaterial->data()->get<bool>("zSorted")))
            return false;

        auto leftAttributes = std::vector<VertexAttribute>();
        auto rightAttributes = std::vector<VertexAttribute>();

        for (auto vertexBuffer : leftGeometry->vertexBuffers())
        {
            leftAttributes.insert(
                leftAttributes.end(),
                vertexBuffer->attributes().begin(),
                vertexBuffer->attributes().end()
            );
        }

        for (auto vertexBuffer : rightGeometry->vertexBuffers())
        {
            rightAttributes.insert(
                rightAttributes.end(),
                vertexBuffer->attributes().begin(),
                vertexBuffer->attributes().end()
            );
        }

        if (leftAttributes.size() != rightAttributes.size())
            return false;

        for (auto i = 0u; i < leftAttributes.size(); ++i)
            if (leftAttributes.at(i).name != rightAttributes.at(i).name)
                return false;

        return left->material() == right->material();
    };

    return surfaceIndexer;
}

template <typename T>
static
bool
validateGeometry(Geometry::Ptr geometry)
{
    auto indices = geometry->indices()->dataPointer<T>();

    if (!indices)
    {
        LOG_WARNING("Invalid Geometry, MeshPartitioner only supports indexed meshes");

        return false;
    }

    const unsigned int numIndices = indices->size();

    if (numIndices == 0u || (numIndices % 3u) != 0u)
    {
        LOG_WARNING("Invalid Geometry, MeshPartitioner only supports triangle primitives");

        return false;
    }

    const auto numVertices = geometry->numVertices();

    for (auto index : *indices)
    {
        if (index >= numVertices)
        {
            LOG_WARNING("Invalid Geometry, index out of bound");

            return false;
        }
    }

    return true;
}

bool
MeshPartitioner::defaultValidSurfacePredicate(Surface::Ptr surface)
{
    auto geometry = surface->geometry();

    auto ushortDataPointer = geometry->indices()->dataPointer<unsigned short>();

    if (ushortDataPointer)
        return validateGeometry<unsigned short>(geometry);

    return validateGeometry<unsigned int>(geometry);
}

bool
MeshPartitioner::defaultInstanceSurfacePredicate(Surface::Ptr surface)
{
    return surface->geometry()->data()->hasProperty("surfaceRefCount") &&
           surface->geometry()->data()->get<unsigned int>("surfaceRefCount") > 1u;
}

bool
MeshPartitioner::defaultNodeFilterFunction(Node::Ptr node)
{
    return true;
}

MeshPartitioner::MeshPartitioner() :
    AbstractWriterPreprocessor<Node::Ptr>(),
    _progressRate(0.f),
    _statusChanged(StatusChangedSignal::create())
{
}

static
void
bounds(NodeSet::Ptr nodes, math::vec3& minBound, math::vec3& maxBound)
{
    if (nodes->nodes().empty())
        return;

    auto root = nodes->nodes().front()->root();

    if (!root->hasComponent<Transform>())
        root->addComponent(Transform::create());

    for (auto node : nodes->nodes())
        if (!node->hasComponent<BoundingBox>())
            node->addComponent(BoundingBox::create());

    root->component<Transform>()->updateModelToWorldMatrix();

    auto worldBox = math::Box::Ptr();

    for (auto node : nodes->nodes())
    {
        auto box = node->component<BoundingBox>()->box();

        if (!worldBox)
            worldBox = math::Box::create(box->topRight(), box->bottomLeft());
        else
            worldBox = worldBox->merge(box);
    }

    minBound = worldBox->bottomLeft();
    maxBound = worldBox->topRight();
}

static
bool
contains(math::Box::Ptr left, math::Box::Ptr right)
{
    return right->bottomLeft().x >= left->bottomLeft().x &&
           right->bottomLeft().y >= left->bottomLeft().y &&
           right->bottomLeft().z >= left->bottomLeft().z &&
           right->topRight().x <= left->topRight().x &&
           right->topRight().y <= left->topRight().y &&
           right->topRight().z <= left->topRight().z;
}

void
MeshPartitioner::defaultWorldBoundsFunction(NodeSet::Ptr nodes, math::vec3& minBound, math::vec3& maxBound)
{
    bounds(nodes, minBound, maxBound);
}

math::vec3
MeshPartitioner::defaultPartitionMaxSizeFunction(Options& options, NodeSet::Ptr nodes)
{
    auto minBound = math::vec3();
    auto maxBound = math::vec3();

    options.worldBoundsFunction(nodes, minBound, maxBound);

    return (maxBound - minBound);
}

std::vector<std::vector<Surface::Ptr>>
MeshPartitioner::mergeSurfaces(const std::vector<Surface::Ptr>& surfaces)
{
    auto surfaceBuckets = std::unordered_map<
        Surface::Ptr,
        std::vector<Surface::Ptr>,
        std::function<std::size_t(std::shared_ptr<Surface>)>,
        std::function<bool(std::shared_ptr<Surface>, std::shared_ptr<Surface>)>
    >(0, _options.surfaceIndexer.hash, _options.surfaceIndexer.equal);

    for (auto surface : surfaces)
        surfaceBuckets[surface].push_back(surface);

    auto mergedSurfaces = std::vector<std::vector<Surface::Ptr>>();

    for (const auto& surfaceBucket : surfaceBuckets)
        mergedSurfaces.emplace_back(surfaceBucket.second.begin(), surfaceBucket.second.end());

    return mergedSurfaces;
}

void
MeshPartitioner::findInstances(const std::vector<Surface::Ptr>& surfaces)
{
    auto geometryToSurfaces = std::unordered_map<Geometry::Ptr, std::list<Surface::Ptr>>();

    for (auto surface : surfaces)
    {
        auto surfaceIt = geometryToSurfaces.insert(std::make_pair(
            surface->geometry(),
            std::list<Surface::Ptr>()
        ));

        surfaceIt.first->second.push_back(surface);
    }

    const auto numSurfaces = surfaces.size();
    const auto numGeometries = geometryToSurfaces.size();

    for (auto geometryToSurfacesPair : geometryToSurfaces)
    {
        if  (geometryToSurfacesPair.second.size() > 1u)
        {
            geometryToSurfacesPair.first->data()->set(
                "surfaceRefCount",
                static_cast<unsigned int>(geometryToSurfacesPair.second.size())
            );
        }
    }
}

bool
MeshPartitioner::surfaceBucketIsValid(const std::vector<SurfacePtr>& surfaceBucket) const
{
    if (surfaceBucket.size() == 1u)
        return true;

    if (surfaceBucket.size() > _options.maxNumSurfacesPerSurfaceBucket)
        return false;

    auto numTriangles = 0u;

    for (auto surface : surfaceBucket)
    {
        numTriangles += surface->geometry()->indices()->numIndices() / 3u;

        if (numTriangles > _options.maxNumTrianglesPerSurfaceBucket)
            return false;
    }

    return true;
}

void
MeshPartitioner::splitSurfaceBucket(const std::vector<Surface::Ptr>&           surfaceBucket,
                                    std::vector<std::vector<Surface::Ptr>>&    splitSurfaceBucket)
{
    if (surfaceBucket.size() == 1u)
    {
        splitSurfaceBucket.push_back(std::vector<Surface::Ptr>());

        splitSurface(surfaceBucket.front(), splitSurfaceBucket.back());

        return;
    }

    const auto begin = 0u;
    const auto middle = surfaceBucket.size() / 2u;
    const auto end = surfaceBucket.size();

    splitSurfaceBucket.emplace_back(surfaceBucket.begin() + begin, surfaceBucket.begin() + middle);
    splitSurfaceBucket.emplace_back(surfaceBucket.begin() + middle, surfaceBucket.begin() + end);
}

void
MeshPartitioner::splitSurface(Surface::Ptr                 surface,
                              std::vector<Surface::Ptr>&   splitSurface)
{
    splitSurface.push_back(surface);
}

void
MeshPartitioner::process(Node::Ptr& node, AssetLibraryPtr assetLibrary)
{
    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(shared_from_this(), "MeshPartitioner: start");

    _assetLibrary = assetLibrary;

    const auto filterFunction = [this](Node::Ptr node) -> bool
        {
            return (_options.nodeFilterFunction
                ? _options.nodeFilterFunction(node)
                : defaultNodeFilterFunction(node)) &&
                node->hasComponent<Surface>();
        };

    _filteredNodes = NodeSet::create(node)
        ->descendants(true)
        ->where([&filterFunction](Node::Ptr descendant) -> bool
        {
            return filterFunction(descendant);
        }
    );

    auto excludedSurfaceNodes = NodeSet::create(node)
        ->descendants(true)
        ->where([&filterFunction](Node::Ptr descendant) -> bool
        {
            return !filterFunction(descendant);
        }
    );

    const auto& surfaceFowardingFunction = _streamingOptions->surfaceOperator().forwardingFunction;

    if (surfaceFowardingFunction)
    {
        for (auto excludedSurfaceNode : excludedSurfaceNodes->nodes())
            for (auto surface : excludedSurfaceNode->components<Surface>())
                surfaceFowardingFunction(surface);
    }

    if (_filteredNodes->nodes().empty())
        return;

    if (_options.worldBoundsFunction)
        _options.worldBoundsFunction(_filteredNodes, _worldMinBound, _worldMaxBound);
    else
        defaultWorldBoundsFunction(_filteredNodes, _worldMinBound, _worldMaxBound);

    auto mergedComponentRoot = Node::create()
        ->addComponent(Transform::create());

    if (node->hasComponent<Transform>())
    {
        mergedComponentRoot->component<Transform>()->matrix(math::inverse(
            node->component<Transform>()->modelToWorldMatrix()
        ));
    }

    node->component<Transform>()->updateModelToWorldMatrix();

    auto surfaces = std::vector<Surface::Ptr>();

    for (auto filteredNode : _filteredNodes->nodes())
        for (auto surface : filteredNode->components<Surface>())
            surfaces.push_back(surface);

    findInstances(surfaces);

    auto surfaceBuckets = std::vector<std::vector<Surface::Ptr>>();

    if (_options.flags & Options::mergeSurfaces)
    {
        surfaceBuckets = mergeSurfaces(surfaces);
    }
    else
    {
        for (auto surface : surfaces)
            surfaceBuckets.push_back({ surface });
    }

    auto validSurfaceBuckets = std::vector<std::vector<Surface::Ptr>>();
    auto pendingSurfaceBuckets = std::queue<std::vector<Surface::Ptr>>();

    for (const auto& surfaceBucket : surfaceBuckets)
        pendingSurfaceBuckets.push(surfaceBucket);

    while (!pendingSurfaceBuckets.empty())
    {
        const auto pendingSurfaceBucket = pendingSurfaceBuckets.front();
        pendingSurfaceBuckets.pop();

        if (surfaceBucketIsValid(pendingSurfaceBucket))
        {
            validSurfaceBuckets.push_back(pendingSurfaceBucket);

            continue;
        }

        auto splitSurfaceBucket = std::vector<std::vector<Surface::Ptr>>();

        this->splitSurfaceBucket(pendingSurfaceBucket, splitSurfaceBucket);

        for (const auto& surfaceBucket : splitSurfaceBucket)
            pendingSurfaceBuckets.push(surfaceBucket);
    }

    for (auto i = 0u; i < validSurfaceBuckets.size(); ++i)
    {
        const auto& surfaceBucket = validSurfaceBuckets.at(i);

        if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        {
            _progressRate = i / float(validSurfaceBuckets.size());

            statusChanged()->execute(
                shared_from_this(),
                "MeshPartitioner: processing new Surface batch with " + std::to_string(surfaceBucket.size()) + " Surface components"
            );
        }

        if (surfaceBucket.empty())
            continue;

        auto partitionInfo = PartitionInfo();

        partitionInfo.mergedIndices = math::UnorderedSpatialIndex<std::unordered_set<unsigned int>>::create();

        for (auto surface : surfaceBucket)
        {
            if (!_options.validSurfacePredicate(surface))
            {
                surface->target()->removeComponent(surface);

                continue;
            }

            partitionInfo.surfaces.push_back(surface);
        }

        if (partitionInfo.surfaces.empty())
            continue;

        auto targetNodeSet = std::unordered_set<Node::Ptr>();

        for (auto surface : partitionInfo.surfaces)
            targetNodeSet.insert(surface->target());

        const auto uniqueTarget = targetNodeSet.size() == 1u;

        auto targetNode = partitionInfo.surfaces.front()->target();

        auto processGeometries = true;

        auto geometries = std::vector<Geometry::Ptr>();

        partitionInfo.isInstance = partitionInfo.surfaces.size() == 1u &&
            _options.instanceSurfacePredicate(partitionInfo.surfaces.front());

        if (partitionInfo.isInstance)
        {
            auto processedInstanceIt = _processedInstances.find(partitionInfo.surfaces.front()->geometry());

            if (processedInstanceIt != _processedInstances.end())
            {
                processGeometries = false;

                geometries.assign(
                    processedInstanceIt->second.begin(),
                    processedInstanceIt->second.end()
                );
            }
        }
        else if (!uniqueTarget)
        {
            const auto newNodeName = targetNode->name();
            const auto newNodeLayout = targetNode->layout();

            targetNode = Node::create(newNodeName)
                ->layout(newNodeLayout)
                ->addComponent(Transform::create())
                ->addComponent(BoundingBox::create());

            mergedComponentRoot->addChild(targetNode);
        }

        if (processGeometries)
        {
            partitionInfo.useRootSpace = !uniqueTarget && !partitionInfo.isInstance;

            if (partitionInfo.surfaces.size() > 1u)
            {
                auto index = 0;

                for (auto surface : partitionInfo.surfaces)
                {
                    preprocessMergedSurface(partitionInfo, surface, index++);
                }
            }

            buildGlobalIndex(partitionInfo);

            buildHalfEdges(partitionInfo);

            buildPartitions(partitionInfo);

            buildGeometries(targetNode, partitionInfo, geometries);
        }

        patchNode(targetNode, partitionInfo, geometries);

        for (auto surface : partitionInfo.surfaces)
            surface->target()->removeComponent(surface);
    }

    node->addChild(mergedComponentRoot);

    _progressRate = 1.f;

    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(shared_from_this(), "MeshPartitioner: stop");
}

MeshPartitioner::OctreeNodePtr
MeshPartitioner::ensurePartitionSizeIsValid(OctreeNodePtr       node,
                                            const math::vec3&   maxSize,
                                            PartitionInfo&      partitionInfo)
{
    auto minBound = node->minBound;
    auto maxBound = node->maxBound;

    if (!partitionInfo.useRootSpace)
    {
        auto transform = partitionInfo.surfaces.front()->target()->component<Transform>();

        minBound = (transform->modelToWorldMatrix() * math::vec4(minBound, 1.f)).xyz();
        maxBound = (transform->modelToWorldMatrix() * math::vec4(maxBound, 1.f)).xyz();
    }

    const auto nodeSize = maxBound - minBound;
    const auto nodeSizeGreaterThanMaxSize = math::greaterThan(nodeSize, maxSize);

    if (nodeSizeGreaterThanMaxSize.x ||
        nodeSizeGreaterThanMaxSize.y ||
        nodeSizeGreaterThanMaxSize.z)
    {
        splitNode(node, partitionInfo);

        for (auto child : node->children)
        {
            ensurePartitionSizeIsValid(child, maxSize, partitionInfo);
        }
    }

    return node;
}

Geometry::Ptr
MeshPartitioner::createGeometry(Geometry::Ptr                       referenceGeometry,
                                const std::vector<unsigned int>&    triangleIndices,
                                PartitionInfo&                      partitionInfo)
{
    if (triangleIndices.empty())
        return nullptr;

    const auto& indices = partitionInfo.indices;
    const auto& vertices = partitionInfo.vertices;

    auto geometry = Geometry::create();

    auto vertexCount = 0;
    auto indexCount = triangleIndices.size() * 3;

    const auto vertexSize = referenceGeometry->vertexSize();

    auto globalIndexToLocalIndexMap = std::unordered_map<unsigned int, unsigned short>(indexCount);
    auto localIndexToGlobalIndexMap = std::unordered_map<unsigned short, unsigned int>(indexCount);

    auto localIndices = std::vector<unsigned short>(indexCount);

    auto currentIndex = 0;

    for (auto i = 0; i < triangleIndices.size(); ++i)
    {
        for (auto j = 0; j < 3; ++j)
        {
            const auto globalIndex = indices.at(triangleIndices.at(i) * 3 + j);

            auto result = globalIndexToLocalIndexMap.insert(std::make_pair(
                globalIndex,
                currentIndex
            ));

            if (result.second)
            {
                localIndices[i * 3 + j] = currentIndex;

                localIndexToGlobalIndexMap.insert(std::make_pair(currentIndex, globalIndex));

                ++currentIndex;

                ++vertexCount;
            }
            else
            {
                localIndices[i * 3 + j] = result.first->second;
            }
        }
    }

    geometry->indices(IndexBuffer::create(referenceGeometry->indices()->context(), localIndices));

    auto globalAttributeOffset = 0;

    for (auto vertexBuffer : referenceGeometry->vertexBuffers())
    {
        auto localVertexSize = vertexBuffer->vertexSize();

        auto vertexBufferData = std::vector<float>(vertexCount * localVertexSize);

        for (auto i = 0; i < triangleIndices.size(); ++i)
        {
            auto triangleIndex = triangleIndices.at(i);

            for (auto j = 0; j < 3; ++j)
            {
                auto globalIndex = indices.at(triangleIndex * 3 + j);
                auto localIndex = globalIndexToLocalIndexMap.at(globalIndex);

                std::copy(
                    vertices.begin() + globalIndex * vertexSize + globalAttributeOffset,
                    vertices.begin() + globalIndex * vertexSize + localVertexSize + globalAttributeOffset,
                    vertexBufferData.begin() + localIndex * localVertexSize
                );
            }
        }

        auto newVertexBuffer = VertexBuffer::create(vertexBuffer->context(), vertexBufferData);

        for (auto attribute : vertexBuffer->attributes())
            newVertexBuffer->addAttribute(attribute.name, attribute.size, attribute.offset);

        geometry->addVertexBuffer(newVertexBuffer);

        globalAttributeOffset += localVertexSize;
    }

    if (_options.flags & Options::applyCrackFreePolicy)
    {
        markProtectedVertices(geometry, localIndexToGlobalIndexMap, partitionInfo);
    }

    return geometry;
}

void
MeshPartitioner::markProtectedVertices(Geometry::Ptr                                            geometry,
                                       const std::unordered_map<unsigned short, unsigned int>&  indices,
                                       PartitionInfo&                                           partitionInfo)
{
    const auto numVertices = geometry->numVertices();

    const auto protectedFlagVertexAttributeSize = 1u;
    const auto protectedFlagVertexAttributeOffset = 0u;

    auto protectedFlagVertexBufferData = std::vector<float>(numVertices * protectedFlagVertexAttributeSize);

    for (auto index : geometry->indices()->data())
    {
        auto globalIndex = indices.at(index);

        const auto protectedFlagVertexBufferDataOffset =
            index * protectedFlagVertexAttributeSize + protectedFlagVertexAttributeOffset;

        if (partitionInfo.protectedIndices.find(globalIndex) != partitionInfo.protectedIndices.end())
        {
            protectedFlagVertexBufferData[protectedFlagVertexBufferDataOffset] = 1.f;
        }
        else
        {
            protectedFlagVertexBufferData[protectedFlagVertexBufferDataOffset] = 0.f;
        }
    }

    auto protectedFlagVertexBuffer = VertexBuffer::create(_assetLibrary->context(), protectedFlagVertexBufferData);

    protectedFlagVertexBuffer->addAttribute(
        "popProtected",
        protectedFlagVertexAttributeSize,
        protectedFlagVertexAttributeOffset
    );

    geometry->addVertexBuffer(protectedFlagVertexBuffer);
}

void
MeshPartitioner::registerSharedTriangle(OctreeNodePtr    partitionNode,
                                        unsigned int     triangleIndex,
                                        PartitionInfo&   partitionInfo)
{
    auto& protectedIndices = partitionInfo.protectedIndices;

    auto sharedIndices = std::vector<unsigned int>(3);

    for (auto i = 0u; i < 3u; ++i)
        sharedIndices[i] = partitionInfo.indices.at(triangleIndex * 3u + i);

    auto candidateHalfEdges = std::unordered_set<HalfEdge::Ptr>();

    for (auto sharedIndex : sharedIndices)
    {
        auto halfEdge = partitionInfo.halfEdges.at(sharedIndex);

        candidateHalfEdges.insert(halfEdge);

        auto discontinousHalfEdges = std::queue<HalfEdge::Ptr>();

        discontinousHalfEdges.push(halfEdge);

        while (!discontinousHalfEdges.empty())
        {
            auto discontinousHalfEdge = discontinousHalfEdges.front();
            discontinousHalfEdges.pop();

            const auto discontinousHalfEdgeIndex = discontinousHalfEdge->startNodeId();

            if (partitionInfo.markedDiscontinousIndices.find(discontinousHalfEdgeIndex) !=
                partitionInfo.markedDiscontinousIndices.end())
                continue;

            partitionInfo.markedDiscontinousIndices.insert(discontinousHalfEdgeIndex);

            candidateHalfEdges.insert(discontinousHalfEdge);

            const auto discontinousHalfEdgeVertexPosition = positionAt(
                discontinousHalfEdgeIndex,
                partitionInfo
            );

            const auto& secondaryDiscontinousHalfEdgeIndices = partitionInfo.mergedIndices->at(
                discontinousHalfEdgeVertexPosition
            );

            for (auto secondaryDiscontinousHalfEdgeIndex : secondaryDiscontinousHalfEdgeIndices)
            {
                if (secondaryDiscontinousHalfEdgeIndex == discontinousHalfEdgeIndex)
                    continue;

                auto secondaryDiscontinousHalfEdge = partitionInfo.halfEdges.at(secondaryDiscontinousHalfEdgeIndex);

                discontinousHalfEdges.push(secondaryDiscontinousHalfEdge);
                discontinousHalfEdges.push(secondaryDiscontinousHalfEdge->next());
                discontinousHalfEdges.push(secondaryDiscontinousHalfEdge->prec());
            }
        }
    }

    for (auto halfEdge : candidateHalfEdges)
    {
        partitionInfo.protectedIndices.insert(halfEdge->startNodeId());
    }
}

void
MeshPartitioner::insertTriangle(OctreeNodePtr       partitionNode,
                                unsigned int        triangleIndex,
                                PartitionInfo&      partitionInfo)
{
    const auto& indices = partitionInfo.indices;
    const auto& vertices = partitionInfo.vertices;

    const auto vertexSize = partitionInfo.vertexSize;
    const auto positionAttributeOffset = partitionInfo.positionAttributeOffset;

    if (partitionNode->children.empty())
    {
        if (countTriangles(partitionNode) < _options.maxNumTrianglesPerNode)
        {
            const auto triangleIndices = std::vector<unsigned int>(
                indices.begin() + triangleIndex * 3,
                indices.begin() + (triangleIndex + 1) * 3
            );

            const auto expectedNumIndices = partitionNode->indices.back().size() + triangleIndices.size();

            if (_options.flags & Options::applyCrackFreePolicy)
            {
                if (expectedNumIndices >= _options.maxNumIndicesPerNode)
                {
                    splitNode(partitionNode, partitionInfo);
                }
                else
                {
                    partitionNode->triangles.back().push_back(triangleIndex);
                    partitionNode->indices.back().insert(triangleIndices.begin(), triangleIndices.end());

                    return;
                }
            }
            else
            {
                if (expectedNumIndices >= _options.maxNumIndicesPerNode)
                {
                    partitionNode->triangles.push_back(std::vector<unsigned int>());
                    partitionNode->indices.push_back(std::set<unsigned int>());
                }

                partitionNode->triangles.back().push_back(triangleIndex);
                partitionNode->indices.back().insert(triangleIndices.begin(), triangleIndices.end());

                return;
            }
        }
        else
        {
            splitNode(partitionNode, partitionInfo);
        }
    }

    auto positions = std::vector<math::vec3>
    {
        math::make_vec3(
            &vertices.at(indices.at(triangleIndex * 3 + 0) * vertexSize + positionAttributeOffset)),
        math::make_vec3(
            &vertices.at(indices.at(triangleIndex * 3 + 1) * vertexSize + positionAttributeOffset)),
        math::make_vec3(
            &vertices.at(indices.at(triangleIndex * 3 + 2) * vertexSize + positionAttributeOffset))
    };

    auto localIndices = std::vector<int>();

    for (const auto& position : positions)
    {
        auto nodeCenter = (partitionNode->maxBound + partitionNode->minBound) * 0.5f;

        auto x = position.x < nodeCenter.x ? 0 : 1;
        auto y = position.y < nodeCenter.y ? 0 : 1;
        auto z = position.z < nodeCenter.z ? 0 : 1;

        localIndices.push_back(indexAt(x, y, z));
    }

    if (localIndices.at(0) == localIndices.at(1) &&
        localIndices.at(1) == localIndices.at(2))
    {
        insertTriangle(partitionNode->children.at(localIndices.at(0)), triangleIndex, partitionInfo);
    }
    else
    {
        if (_options.flags & Options::applyCrackFreePolicy)
        {
            registerSharedTriangle(partitionNode, triangleIndex, partitionInfo);

            insertTriangle(partitionNode->children.at(localIndices.at(0)), triangleIndex, partitionInfo);
        }
        else
        {
            auto& sharedTriangles = partitionNode->sharedTriangles;
            auto& sharedIndices = partitionNode->sharedIndices;

            const auto triangleIndices = std::vector<unsigned int>(
                indices.begin() + triangleIndex * 3,
                indices.begin() + (triangleIndex + 1) * 3
            );

            const auto expectedNumIndices = sharedIndices.back().size() + triangleIndices.size();

            if (expectedNumIndices >= _options.maxNumIndicesPerNode)
            {
                sharedTriangles.push_back(std::vector<unsigned int>());
                sharedIndices.push_back(std::set<unsigned int>());
            }

            sharedTriangles.back().push_back(triangleIndex);
            sharedIndices.back().insert(triangleIndices.begin(), triangleIndices.end());
        }
    }
}

void
MeshPartitioner::splitNode(OctreeNodePtr    partitionNode,
                           PartitionInfo&   partitionInfo)
{
    partitionNode->children.resize(8);

    auto nodeCenter = (partitionNode->maxBound + partitionNode->minBound) * 0.5f;
    auto nodeHalfSize = (partitionNode->maxBound - partitionNode->minBound) * 0.5f;

    for (auto x = 0; x < 2; ++x)
    for (auto y = 0; y < 2; ++y)
    for (auto z = 0; z < 2; ++z)
    {
        partitionNode->children[indexAt(x, y, z)] = OctreeNodePtr(new OctreeNode(
            partitionNode->depth + 1,
            partitionNode->minBound + math::vec3(
                x * nodeHalfSize.x,
                y * nodeHalfSize.y,
                z * nodeHalfSize.z),
            partitionNode->minBound + math::vec3(
                (x + 1) * nodeHalfSize.x,
                (y + 1) * nodeHalfSize.y,
                (z + 1) * nodeHalfSize.z),
            partitionNode));
    }

    for (const auto& triangles : partitionNode->triangles)
    for (auto triangle : triangles)
    {
        insertTriangle(partitionNode, triangle, partitionInfo);
    }

    partitionNode->triangles = std::vector<std::vector<unsigned int>>(1, std::vector<unsigned int>());
    partitionNode->indices.clear();
}

int
MeshPartitioner::countTriangles(OctreeNodePtr partitionNode)
{
    auto numTriangles = 0;

    for (const auto& triangles : partitionNode->triangles)
        numTriangles += triangles.size();

    return numTriangles;
}

int
MeshPartitioner::indexAt(int x, int y, int z)
{
    return x + (y << 1) + (z << 2);
}

int
MeshPartitioner::computeDepth(OctreeNodePtr partitionNode)
{
    auto depth = 0;

    auto pendingNodes = std::queue<OctreeNodePtr>();

    pendingNodes.push(partitionNode);

    while (!pendingNodes.empty())
    {
        auto pendingNode = pendingNodes.front();

        pendingNodes.pop();

        for (auto childPartitionNode : pendingNode->children)
        {
            pendingNodes.push(childPartitionNode);
        }

        depth = std::max(depth, pendingNode->depth);
    }

    return depth;
}

bool
MeshPartitioner::preprocessMergedSurface(PartitionInfo& partitionInfo,
                                         Surface::Ptr   surface,
                                         int            index)
{
    if (surface->data()->hasProperty("mergingMask") ||
        surface->geometry()->hasVertexAttribute("mergingMask"))
        return true;

    const auto mergingMask = index;

    surface->data()->set("mergingMask", mergingMask);

    auto geometry = surface->geometry();

    auto positionVertexBuffer = geometry->vertexBuffer("position");

    auto mergingMaskData = std::vector<float>(geometry->numVertices(), mergingMask);

    auto mergingMaskVertexBuffer = VertexBuffer::create(positionVertexBuffer->context(), mergingMaskData);

    mergingMaskVertexBuffer->addAttribute("mergingMask", 1u, 0u);

    geometry->addVertexBuffer(mergingMaskVertexBuffer);

    return true;
}

bool
MeshPartitioner::buildGlobalIndex(PartitionInfo& partitionInfo)
{
    const auto& surfaces = partitionInfo.surfaces;

    if (surfaces.empty())
        return false;

    auto indexCount = 0;
    auto vertexCount = 0;

    for (auto surface : surfaces)
    {
        auto geometry = surface->geometry();

        indexCount += geometry->indices()->numIndices();
        vertexCount += geometry->numVertices();
    }

    auto referenceGeometry = surfaces.front()->geometry();

    const auto vertexSize = referenceGeometry->vertexSize();

    partitionInfo.vertexSize = vertexSize;

    auto globalPositionVertexAttributeOffset = 0u;

    for (auto vertexBuffer : referenceGeometry->vertexBuffers())
    {
        if (vertexBuffer->hasAttribute("position"))
        {
            partitionInfo.positionAttributeOffset = globalPositionVertexAttributeOffset + vertexBuffer->attribute("position").offset;

            break;
        }

        globalPositionVertexAttributeOffset += vertexBuffer->vertexSize();
    }

    partitionInfo.positionAttributeOffset = globalPositionVertexAttributeOffset;

    auto& indices = partitionInfo.indices;
    auto& vertices = partitionInfo.vertices;

    indices.resize(indexCount, 0);
    vertices.resize(vertexCount * vertexSize, 0.0f);

    auto indexOffset = 0u;
    auto vertexOffset = 0u;

    auto& minBound = partitionInfo.minBound;
    auto& maxBound = partitionInfo.maxBound;

    minBound = math::vec3(
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max()
    );

    maxBound = math::vec3(
        -std::numeric_limits<float>::max(),
        -std::numeric_limits<float>::max(),
        -std::numeric_limits<float>::max()
    );

    for (auto surface : surfaces)
    {
        auto target = surface->target();
        auto transformMatrix = math::mat4();

        if (partitionInfo.useRootSpace && target->hasComponent<Transform>())
        {
            transformMatrix = target->component<Transform>()->modelToWorldMatrix(true);
        }

        auto geometry = surface->geometry();

        auto positionVertexBuffer = geometry->vertexBuffer("position");

        auto normalVertexBuffer = geometry->vertexBuffer("normal");
        auto tangentVertexBuffer = geometry->vertexBuffer("tangent");

        const auto localIndexCount = geometry->indices()->numIndices();
        const auto localVertexCount = geometry->numVertices();

        auto indexData = std::vector<unsigned int>();

        auto ushortIndexDataPointer = geometry->indices()->dataPointer<unsigned short>();

        if (ushortIndexDataPointer)
        {
            indexData.resize(ushortIndexDataPointer->size());

            for (auto i = 0u; i < ushortIndexDataPointer->size(); ++i)
                indexData[i] = static_cast<unsigned int>(ushortIndexDataPointer->at(i));
        }

        const auto& localIndices = ushortIndexDataPointer
            ? indexData
            : *geometry->indices()->dataPointer<unsigned int>();

        auto globalVertexAttributeOffset = 0;

        for (auto vertexBuffer : geometry->vertexBuffers())
        {
            const auto& localVertices = vertexBuffer->data();
            auto transformedLocalVertices = localVertices;

            const auto localVertexSize = vertexBuffer->vertexSize();

            for (auto i = 0; i < localIndices.size(); ++i)
            {
                auto localIndex = static_cast<int>(localIndices.at(i));

                auto globalIndex = localIndex + vertexOffset;

                indices[indexOffset + i] = globalIndex;

                if (vertexBuffer == positionVertexBuffer)
                {
                    const auto positionAttributeOffset = vertexBuffer->attribute("position").offset;

                    auto position = math::make_vec3(
                        &localVertices.at(localIndex * localVertexSize + positionAttributeOffset
                    ));

                    if (partitionInfo.useRootSpace)
                    {
                        position = math::vec3(transformMatrix * math::vec4(position, 1.0f));

                        std::copy(
                            &position.x,
                            &position.x + 3,
                            transformedLocalVertices.begin() + localIndex * localVertexSize + positionAttributeOffset
                        );
                    }

                    minBound = math::min(minBound, position);
                    maxBound = math::max(maxBound, position);
                }

                if (partitionInfo.useRootSpace)
                {
                    if (vertexBuffer == normalVertexBuffer)
                    {
                        const auto normalAttributeOffset = vertexBuffer->attribute("normal").offset;

                        auto normal = math::make_vec3(
                            &localVertices.at(localIndex * localVertexSize + normalAttributeOffset
                        ));

                        normal = math::mat3(transformMatrix) * normal;

                        std::copy(
                            &normal.x,
                            &normal.x + 3,
                            transformedLocalVertices.begin() + localIndex * localVertexSize + normalAttributeOffset
                        );
                    }

                    if (vertexBuffer == tangentVertexBuffer)
                    {
                        const auto tangentAttributeOffset = vertexBuffer->attribute("tangent").offset;

                        auto tangent = math::make_vec3(
                            &localVertices.at(localIndex * localVertexSize + tangentAttributeOffset
                        ));

                        tangent = math::mat3(transformMatrix) * tangent;

                        std::copy(
                            &tangent.x,
                            &tangent.x + 3,
                            transformedLocalVertices.begin() + localIndex * localVertexSize + tangentAttributeOffset
                        );
                    }
                }

                std::copy(
                    transformedLocalVertices.begin() + localIndex * localVertexSize,
                    transformedLocalVertices.begin() + (localIndex + 1) * localVertexSize,
                    vertices.begin() + globalIndex * vertexSize + globalVertexAttributeOffset
                );
            }

            globalVertexAttributeOffset += localVertexSize;
        }

        indexOffset += localIndexCount;
        vertexOffset += localVertexCount;
    }

    for (auto surface : surfaces)
    {
        if (_options.instanceSurfacePredicate(surface))
            continue;

        surface->geometry()->disposeIndexBufferData();
        surface->geometry()->disposeVertexBufferData();
    }

    return true;
}

bool
MeshPartitioner::buildHalfEdges(PartitionInfo& partitionInfo)
{
    auto halfEdgeCollection = HalfEdgeCollection::create(partitionInfo.indices);

    const auto& halfEdges = halfEdgeCollection->halfEdges();

    const auto numVertices = partitionInfo.vertices.size() / partitionInfo.vertexSize;

    partitionInfo.halfEdges.resize(numVertices);

    partitionInfo.halfEdgeReferences.resize(halfEdges.size());
    std::copy(
        halfEdges.begin(),
        halfEdges.end(),
        partitionInfo.halfEdgeReferences.begin()
    );

    for (auto halfEdge : halfEdges)
    {
        const auto index = halfEdge->startNodeId();

        partitionInfo.halfEdges[index] = halfEdge;

        const auto position = math::make_vec3(&partitionInfo.vertices.at(
            index * partitionInfo.vertexSize + partitionInfo.positionAttributeOffset
        ));

        auto& mergedIndices = partitionInfo.mergedIndices->at(position);

        mergedIndices.insert(index);
    }

    return true;
}

bool
MeshPartitioner::buildPartitions(PartitionInfo& partitionInfo)
{
    const auto rootPartitionMinBound = partitionInfo.minBound;
    const auto rootPartitionMaxBound = partitionInfo.maxBound;

    auto& octreeRoot = partitionInfo.rootPartitionNode;

    octreeRoot = OctreeNodePtr(new OctreeNode(
        0,
        rootPartitionMinBound,
        rootPartitionMaxBound,
        nullptr
    ));

    partitionInfo.baseDepth = 0;

    octreeRoot = ensurePartitionSizeIsValid(
        octreeRoot,
        _options.partitionMaxSizeFunction
            ? _options.partitionMaxSizeFunction(_options, _filteredNodes)
            : defaultPartitionMaxSizeFunction(_options, _filteredNodes),
        partitionInfo
    );

    for (auto i = 0; i < partitionInfo.indices.size(); i += 3)
    {
        insertTriangle(octreeRoot, i / 3, partitionInfo);
    }

    return true;
}

bool
MeshPartitioner::buildGeometries(Node::Ptr                      node,
                                 PartitionInfo&                 partitionInfo,
                                 std::vector<Geometry::Ptr>&    geometries)
{
    auto partitionNode = partitionInfo.rootPartitionNode;

    const auto modelToWorldMatrix = node->component<Transform>()->modelToWorldMatrix();
    const auto worldToModelMatrix = math::inverse(modelToWorldMatrix);

    auto referenceSurface = partitionInfo.surfaces.front();
    auto referenceGeometry = referenceSurface->geometry();
    auto referenceMaterial = referenceSurface->material();
    auto referenceEffect = referenceSurface->effect();

    static auto currentGeometryId = 0;

    auto maxDepth = computeDepth(partitionNode);

    auto pendingNodes = std::queue<OctreeNodePtr>();

    pendingNodes.push(partitionNode);

    while (!pendingNodes.empty())
    {
        auto pendingNode = pendingNodes.front();

        pendingNodes.pop();

        for (auto childPartitionNode : pendingNode->children)
        {
            pendingNodes.push(childPartitionNode);
        }

        auto newGeometries = std::list<Geometry::Ptr>();

        if (!pendingNode->children.empty())
        {
            for (const auto& triangles : pendingNode->sharedTriangles)
            {
                auto newGeometry = createGeometry(referenceGeometry, triangles, partitionInfo);

                if (newGeometry == nullptr)
                    continue;

                newGeometry->data()->set("isSharedPartition", true);

                newGeometries.push_back(newGeometry);
            }
        }
        else
        {
            for (const auto& triangles : pendingNode->triangles)
            {
                auto newGeometry = createGeometry(referenceGeometry, triangles, partitionInfo);

                if (newGeometry == nullptr)
                    continue;

                newGeometries.push_back(newGeometry);
            }
        }

        for (auto newGeometry : newGeometries)
        {
            if (newGeometry == nullptr)
                continue;

            auto minBound = pendingNode->minBound;
            auto maxBound = pendingNode->maxBound;

            if (referenceGeometry->data()->hasProperty("type"))
                newGeometry->data()->set("type", referenceGeometry->data()->get<std::string>("type"));

            const auto baseDepth = partitionInfo.baseDepth;

            newGeometry->data()->set("partitioningMaxDepth", maxDepth - baseDepth);
            newGeometry->data()->set("partitioningDepth", pendingNode->depth - baseDepth);
            newGeometry->data()->set("partitioningMinBound", minBound);
            newGeometry->data()->set("partitioningMaxBound", maxBound);

            auto geometryName = "geometry_" + std::to_string(currentGeometryId++);
            auto geometryNameLastSeparatorPos = geometryName.find_last_of("/\\");

            if (geometryNameLastSeparatorPos != std::string::npos)
                geometryName = geometryName.substr(geometryNameLastSeparatorPos + 1);

            _assetLibrary->geometry(
                geometryName,
                newGeometry
            );

            geometries.push_back(newGeometry);
        }
    }

    if (partitionInfo.isInstance)
    {
        _processedInstances.insert(std::make_pair(referenceGeometry, geometries));
    }

    return true;
}

bool
MeshPartitioner::patchNode(Node::Ptr                            node,
                           PartitionInfo&                       partitionInfo,
                           const std::vector<Geometry::Ptr>&    geometries)
{
    auto referenceSurface = partitionInfo.surfaces.front();
    auto referenceMaterial = referenceSurface->material();
    auto referenceEffect = referenceSurface->effect();

    auto newSurfaces = std::list<Surface::Ptr>();

    for (auto geometry : geometries)
    {
        auto newSurface = Surface::create(
            geometry,
            referenceMaterial,
            referenceEffect
        );

        newSurfaces.push_back(newSurface);

        if (_options.flags & Options::createOneNodePerSurface)
        {
            auto newNode = Node::create(node->name())
                ->layout(node->layout())
                ->addComponent(Transform::create())
                ->addComponent(BoundingBox::create())
                ->addComponent(newSurface);

            node->addChild(newNode);
        }
        else
        {
            node->addComponent(newSurface);
        }
    }

    if (_streamingOptions->surfaceOperator().substitutionFunction)
        _streamingOptions->surfaceOperator().substitutionFunction(
            std::list<Surface::Ptr>(partitionInfo.surfaces.begin(), partitionInfo.surfaces.end()),
            newSurfaces
        );

    return true;
}

math::vec3
MeshPartitioner::positionAt(unsigned int         index,
                            const PartitionInfo& partitionInfo)
{
    return math::make_vec3(&partitionInfo.vertices.at(
        index * partitionInfo.vertexSize + partitionInfo.positionAttributeOffset
    ));
}
