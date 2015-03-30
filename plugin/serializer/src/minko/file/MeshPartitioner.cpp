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
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

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
    flags(Options::none),
    partitionMaxSizeFunction(defaultPartitionMaxSizeFunction),
    worldBoundsFunction(defaultWorldBoundsFunction),
    nodeFilterFunction(defaultNodeFilterFunction),
    surfaceIndexer(defaultSurfaceIndexer())
{
}

MeshPartitioner::SurfaceIndexer
MeshPartitioner::defaultSurfaceIndexer()
{
    auto surfaceIndexer = SurfaceIndexer();

    surfaceIndexer.hash = [](Surface::Ptr surface) -> std::size_t
    {
        auto hashValue = std::size_t();

        minko::hash_combine<Material::Ptr, std::hash<Material::Ptr>>(hashValue, surface->material());

        for (const auto& vertexBuffer : surface->geometry()->vertexBuffers())
            for (const auto& attribute : vertexBuffer->attributes())
                minko::hash_combine<std::string, std::hash<std::string>>(hashValue, *attribute.name);

        return hashValue;
    };

    surfaceIndexer.equal = [](Surface::Ptr left, Surface::Ptr right) -> bool
    {
        auto leftMaterial = left->material();
        auto rightMaterial = right->material();

        if ((leftMaterial->data()->hasProperty("zSorted") && leftMaterial->data()->get<bool>("zSorted")) ||
            (rightMaterial->data()->hasProperty("zSorted") && rightMaterial->data()->get<bool>("zSorted")))
            return false;

        auto leftAttributes = std::vector<VertexAttribute>();
        auto rightAttributes = std::vector<VertexAttribute>();

        for (auto vertexBuffer : left->geometry()->vertexBuffers())
        {
            leftAttributes.insert(
                leftAttributes.end(),
                vertexBuffer->attributes().begin(),
                vertexBuffer->attributes().end()
            );
        }

        for (auto vertexBuffer : right->geometry()->vertexBuffers())
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

bool
MeshPartitioner::defaultNodeFilterFunction(Node::Ptr node)
{
    return true;
}

MeshPartitioner::MeshPartitioner() :
    AbstractWriterPreprocessor<Node::Ptr>()
{
}

static
void
bounds(Node::Ptr root, math::vec3& minBound, math::vec3& maxBound)
{
    if (!root->hasComponent<Transform>())
        root->addComponent(Transform::create());

    auto meshNodes = NodeSet::create(root)
        ->descendants(true)
        ->where([](Node::Ptr descendant){ return descendant->hasComponent<Surface>(); });

    for (auto meshNode : meshNodes->nodes())
        if (!meshNode->hasComponent<BoundingBox>())
            meshNode->addComponent(BoundingBox::create());

    root->component<Transform>()->updateModelToWorldMatrix();

    auto worldBox = math::Box::create();

    for (auto meshNode : meshNodes->nodes())
        worldBox = worldBox->merge(meshNode->component<BoundingBox>()->box());

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
MeshPartitioner::defaultWorldBoundsFunction(Node::Ptr root, math::vec3& minBound, math::vec3& maxBound)
{
    bounds(root, minBound, maxBound);
}

math::vec3
MeshPartitioner::defaultPartitionMaxSizeFunction(Node::Ptr root)
{
    auto minBound = math::vec3();
    auto maxBound = math::vec3();

    bounds(root, minBound, maxBound);

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
MeshPartitioner::process(Node::Ptr& node, AssetLibraryPtr assetLibrary)
{
    _assetLibrary = assetLibrary;

    auto meshNodes = NodeSet::create(node)
        ->descendants(true)
        ->where([this](Node::Ptr descendant) -> bool
        {
            return (_options.nodeFilterFunction
                ? _options.nodeFilterFunction(descendant)
                : defaultNodeFilterFunction(descendant)) &&
                descendant->hasComponent<Surface>();
        }
    );

    if (meshNodes->nodes().empty())
        return;

    if (_options.worldBoundsFunction)
        _options.worldBoundsFunction(node, _worldMinBound, _worldMaxBound);
    else
        defaultWorldBoundsFunction(node, _worldMinBound, _worldMaxBound);

    if (_options.flags & Options::mergeSurfaces)
    {
        node->component<Transform>()->updateModelToWorldMatrix();

        auto surfaces = std::vector<Surface::Ptr>();

        for (auto meshNode : meshNodes->nodes())
            for (auto surface : meshNode->components<Surface>())
                surfaces.push_back(surface);

        auto surfaceBuckets = mergeSurfaces(surfaces);

        for (const auto& surfaceBucket : surfaceBuckets)
        {
            auto partitionInfo = PartitionInfo();

            for (auto surface : surfaceBucket)
                partitionInfo.surfaces.push_back(surface);

            partitionInfo.useRootSpace = true;

            buildGlobalIndex(partitionInfo);

            buildHalfEdges(partitionInfo);

            buildPartitions(partitionInfo);

            auto newNodeName = std::string();

            if (!surfaceBucket.empty())
                newNodeName = surfaceBucket.front()->target()->name();

            for (auto surface : surfaceBucket)
                surface->target()->removeComponent(surface);

            auto newNode = Node::create(newNodeName)
                ->addComponent(Transform::create())
                ->addComponent(BoundingBox::create());

            patchNode(newNode, partitionInfo);

            node->addChild(newNode);
        }

        node->component<Transform>()->matrix(math::mat4());
        node->component<Transform>()->updateModelToWorldMatrix();
    }
}

MeshPartitioner::OctreeNodePtr
MeshPartitioner::pickBestPartitions(OctreeNodePtr       root,
                                    const math::vec3&   modelMinBound,
                                    const math::vec3&   modelMaxBound,
                                    PartitionInfo&      partitionInfo)
{
    auto currentMinBound = root->minBound;
    auto currentMaxBound = root->maxBound;

    auto validNode = OctreeNodePtr();

    if (!contains(math::Box::create(root->maxBound, root->minBound), math::Box::create(modelMaxBound, modelMinBound)))
        return root;

    splitNode(root, partitionInfo);

    for (auto child : root->children)
    {
        if (contains(math::Box::create(child->maxBound, child->minBound), math::Box::create(modelMaxBound, modelMinBound)))
        {
            validNode = pickBestPartitions(child, modelMinBound, modelMaxBound, partitionInfo);

            break;
        }
    }

    if (validNode == nullptr)
    {
        root->children.clear();

        return root;
    }

    return validNode;
}

MeshPartitioner::OctreeNodePtr
MeshPartitioner::ensurePartitionSizeIsValid(OctreeNodePtr       node,
                                            const math::vec3&   maxSize,
                                            PartitionInfo&      partitionInfo)
{
    const auto minBound = node->minBound;
    const auto maxBound = node->maxBound;

    const auto nodeSize = maxBound - minBound;

    if (nodeSize.x > maxSize.x ||
        nodeSize.y > maxSize.y ||
        nodeSize.z > maxSize.z)
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

    if (geometry->hasVertexAttribute("normal"))
    {
        geometry->computeNormals();
    }

    if (geometry->hasVertexAttribute("position") &&
        geometry->hasVertexAttribute("uv") &&
        geometry->hasVertexAttribute("tangent"))
    {
        geometry->computeTangentSpace(!geometry->hasVertexAttribute("normal"));
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

    for (auto sharedIndex : sharedIndices)
    {
        auto sharedIndexPositions = std::vector<math::vec3>();

        const auto position = positionAt(sharedIndex, partitionInfo);

        sharedIndexPositions.push_back(position);
        
        auto halfEdge = partitionInfo.halfEdges.at(sharedIndex);

        auto candidateHalfEdges = std::unordered_set<HalfEdge::Ptr>();

        candidateHalfEdges.insert(halfEdge);
        candidateHalfEdges.insert(halfEdge->next());
        candidateHalfEdges.insert(halfEdge->prec());

        if (halfEdge->adjacent() != nullptr)
        {
            candidateHalfEdges.insert(halfEdge->adjacent());

            sharedIndexPositions.push_back(positionAt(halfEdge->adjacent()->startNodeId(), partitionInfo));
        }

        if (halfEdge->next()->adjacent() != nullptr)
        {
            candidateHalfEdges.insert(halfEdge->next()->adjacent());

            sharedIndexPositions.push_back(positionAt(halfEdge->next()->adjacent()->startNodeId(), partitionInfo));
        }

        if (halfEdge->prec()->adjacent() != nullptr)
        {
            candidateHalfEdges.insert(halfEdge->prec()->adjacent());

            sharedIndexPositions.push_back(positionAt(halfEdge->prec()->adjacent()->startNodeId(), partitionInfo));
        }

        for (const auto& sharedIndexPosition : sharedIndexPositions)
        {
            for (auto mergedIndex : partitionInfo.mergedIndices.at(sharedIndexPosition))
            {
                if (mergedIndex == sharedIndex)
                    continue;

                if (partitionInfo.markedDiscontinousIndices.find(mergedIndex) !=
                    partitionInfo.markedDiscontinousIndices.end())
                    continue;

                partitionInfo.markedDiscontinousIndices.insert(mergedIndex);

                auto mergedIndexHalfEdge = partitionInfo.halfEdges.at(mergedIndex);

                for (auto i = 0u; i < 3u; ++i)
                {
                    candidateHalfEdges.insert(mergedIndexHalfEdge->face().at(i));
                    candidateHalfEdges.insert(mergedIndexHalfEdge->next()->face().at(i));
                    candidateHalfEdges.insert(mergedIndexHalfEdge->prec()->face().at(i));
                }

                auto discontinousHalfEdges = std::queue<HalfEdge::Ptr>();

                for (auto i = 0u; i < 3u; ++i)
                {
                    discontinousHalfEdges.push(mergedIndexHalfEdge->face().at(i));
                    discontinousHalfEdges.push(mergedIndexHalfEdge->next()->face().at(i));
                    discontinousHalfEdges.push(mergedIndexHalfEdge->prec()->face().at(i));
                }

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

                    const auto& secondaryDiscontinousHalfEdgeIndices = partitionInfo.mergedIndices.at(
                        discontinousHalfEdgeVertexPosition
                    );

                    for (auto secondaryDiscontinousHalfEdgeIndex : secondaryDiscontinousHalfEdgeIndices)
                    {
                        if (secondaryDiscontinousHalfEdgeIndex == discontinousHalfEdgeIndex)
                            continue;

                        auto secondaryDiscontinousHalfEdge = partitionInfo.halfEdges.at(secondaryDiscontinousHalfEdgeIndex);

                        for (auto i = 0u; i < 3u; ++i)
                        {
                            discontinousHalfEdges.push(secondaryDiscontinousHalfEdge->face().at(i));
                            discontinousHalfEdges.push(secondaryDiscontinousHalfEdge->next()->face().at(i));
                            discontinousHalfEdges.push(secondaryDiscontinousHalfEdge->prec()->face().at(i));
                        }
                    }
                }
            }
        }

        for (auto halfEdge : candidateHalfEdges)
        {
            partitionInfo.protectedIndices.insert(halfEdge->startNodeId());
        }
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
            auto& triangles = partitionNode->triangles.back();

            const auto triangleIndices = std::vector<unsigned int>(
                indices.begin() + triangleIndex * 3,
                indices.begin() + (triangleIndex + 1) * 3
            );

            auto& indices = partitionNode->indices.back();

            const auto expectedNumIndices = indices.size() + triangleIndices.size();

            if (_options.flags & Options::applyCrackFreePolicy)
            {
                if (expectedNumIndices >= _options.maxNumIndicesPerNode)
                {
                    splitNode(partitionNode, partitionInfo);
                }
                else
                {
                    triangles.push_back(triangleIndex);

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
    partitionInfo.positionAttributeOffset = 0u;

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
            transformMatrix = target->component<Transform>()->modelToWorldMatrix();
        }

        auto geometry = surface->geometry();

        auto positionVertexBuffer = geometry->vertexBuffer("position");

        const auto localIndexCount = geometry->indices()->numIndices();
        const auto localVertexCount = geometry->numVertices();

        const auto& localIndices = geometry->indices()->data();

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

    return true;
}

bool
MeshPartitioner::buildHalfEdges(PartitionInfo& partitionInfo)
{
    auto halfEdges = HalfEdgeCollection::create(partitionInfo.indices);

    partitionInfo.halfEdges.resize(halfEdges->halfEdges().size());

    for (auto halfEdge : halfEdges->halfEdges())
    {
        const auto index = halfEdge->startNodeId();

        partitionInfo.halfEdges[index] = halfEdge;

        const auto position = math::make_vec3(&partitionInfo.vertices.at(
            index * partitionInfo.vertexSize + partitionInfo.positionAttributeOffset
        ));

        auto it = partitionInfo.mergedIndices.insert(std::make_pair(
            position,
            std::unordered_set<unsigned int>()
        ));

        it.first->second.insert(index);
    }

    return true;
}

bool
MeshPartitioner::buildPartitions(PartitionInfo& partitionInfo)
{
    const auto minBound = partitionInfo.minBound;
    const auto maxBound = partitionInfo.maxBound;

    auto rootPartitionMinBound = minBound;
    auto rootPartitionMaxBound = maxBound;

    if (_options.flags & Options::uniformizeSize)
    {
        rootPartitionMinBound = _worldMinBound;
        rootPartitionMaxBound = _worldMaxBound;
    }

    auto& octreeRoot = partitionInfo.rootPartitionNode;

    octreeRoot = OctreeNodePtr(new OctreeNode(
        0,
        rootPartitionMinBound,
        rootPartitionMaxBound,
        nullptr
    ));

    if (_options.flags & Options::uniformizeSize)
    {
        auto nodeMinBound = minBound;
        auto nodeMaxBound = maxBound;

        // fixme apply transform according to partitionInfo.useRootSpace

        octreeRoot = pickBestPartitions(octreeRoot, nodeMinBound, nodeMaxBound, partitionInfo);

        partitionInfo.baseDepth = octreeRoot->depth;
    }
    else
    {
        partitionInfo.baseDepth = 0;
    }

    //octreeRoot = ensurePartitionSizeIsValid(
    //    octreeRoot,
    //    _options._partitionMaxSizeFunction
    //        ? _options._partitionMaxSizeFunction(root)
    //        : defaultPartitionMaxSizeFunction(root),
    //    math::mat4()
    //);

    for (auto i = 0; i < partitionInfo.indices.size(); i += 3)
    {
        insertTriangle(octreeRoot, i / 3, partitionInfo);
    }

    return true;
}

bool
MeshPartitioner::patchNode(Node::Ptr        node,
                           PartitionInfo&   partitionInfo)
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

            if (_options.flags & Options::uniformizeSize)
            {
                minBound = math::vec3(worldToModelMatrix * math::vec4(minBound, 1.f));
                maxBound = math::vec3(worldToModelMatrix * math::vec4(maxBound, 1.f));
            }

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

            auto newSurface = Surface::create(
                newGeometry,
                referenceMaterial,
                referenceEffect
            );

            if (_options.flags & Options::createOneNodePerSurface)
            {
                auto newNode = Node::create(node->name())
                    ->addComponent(Transform::create())
                    ->addComponent(BoundingBox::create(maxBound, minBound))
                    ->addComponent(newSurface);

                node->addChild(newNode);
            }
            else
            {
                node->addComponent(newSurface);
            }
        }
    }

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
