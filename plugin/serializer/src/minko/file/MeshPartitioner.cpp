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
#include "minko/file/MeshPartitioner.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::material;
using namespace minko::render;
using namespace minko::scene;

struct SurfaceIndexerHash
{
    inline
    std::size_t
    operator()(Surface::Ptr surface) const
    {
        auto hashValue = std::size_t();

        std::hash_combine(hashValue, surface->material());

        for (const auto& vertexBuffer : surface->geometry()->vertexBuffers())
            for (const auto& attribute : vertexBuffer->attributes())
                std::hash_combine(hashValue, attribute.name);

        return hashValue;
    }
};

struct SurfaceIndexerComparator
{
    inline
    bool
    operator()(Surface::Ptr left, Surface::Ptr right) const
    {
        auto leftAttributes = std::vector<VertexAttribute>();
        auto rightAttributes = std::vector<VertexAttribute>();

        for (auto vertexBuffer : left->geometry()->vertexBuffers())
            leftAttributes.insert(
            leftAttributes.end(),
            vertexBuffer->attributes().begin(),
            vertexBuffer->attributes().end()
        );

        for (auto vertexBuffer : right->geometry()->vertexBuffers())
            rightAttributes.insert(
            rightAttributes.end(),
            vertexBuffer->attributes().begin(),
            vertexBuffer->attributes().end()
        );

        if (leftAttributes.size() != rightAttributes.size())
            return false;

        for (auto i = 0u; i < leftAttributes.size(); ++i)
            if (leftAttributes.at(i).name != rightAttributes.at(i).name)
                return false;

        return left->material() == right->material();
    }
};

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

static
void
defaultWorldBoundsFunction(Node::Ptr root, math::vec3& minBound, math::vec3& maxBound)
{
    bounds(root, minBound, maxBound);
}

static
math::vec3
defaultPartitionMaxSizeFunction(Node::Ptr root)
{
    auto minBound = math::vec3();
    auto maxBound = math::vec3();

    bounds(root, minBound, maxBound);

    return (maxBound - minBound);
}

std::vector<std::vector<Surface::Ptr>>
MeshPartitioner::mergeSurfaces(const std::vector<Surface::Ptr>& surfaces)
{
    auto surfaceBuckets = std::unordered_map <
        Surface::Ptr,
        std::vector<Surface::Ptr>,
        SurfaceIndexerHash,
        SurfaceIndexerComparator
    > ();

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
    auto meshNodes = NodeSet::create(node)
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<Surface>(); });

    if (_options._worldBoundsFunction)
        _options._worldBoundsFunction(node, _worldMinBound, _worldMaxBound);
    else
        defaultWorldBoundsFunction(node, _worldMinBound, _worldMaxBound);

    if ((_options._flags & Options::mergeSurfaces) == 0)
    {
        for (auto meshNode : meshNodes->nodes())
        {
            auto surfaces = meshNode->components<Surface>();
            auto surfaceBuckets = mergeSurfaces(surfaces);

            for (auto surface : meshNode->components<Surface>())
                meshNode->removeComponent(surface);

            for (const auto& surfaceBucket : surfaceBuckets)
            {
                auto partitionNode = buildPartitions(surfaceBucket, node, false, meshNode->component<Transform>()->modelToWorldMatrix());

                if (_options._flags & Options::useBorderAsSharedTriangles)
                {
                    processBorders(partitionNode, _options._borderMinPrecision, _options._borderMaxDeltaPrecision);
                }

                patchNodeFromPartitions(meshNode, surfaceBucket.front(), partitionNode, assetLibrary);
            }
        }
    }
    else
    {
        node->component<Transform>()->updateModelToWorldMatrix();

        auto surfaces = std::vector<Surface::Ptr>();

        for (auto meshNode : meshNodes->nodes())
            for (auto surface : meshNode->components<Surface>())
                surfaces.push_back(surface);

        auto surfaceBuckets = mergeSurfaces(surfaces);

        for (const auto& surfaceBucket : surfaceBuckets)
        {
            auto partitionNode = buildPartitions(surfaceBucket, node, true, math::mat4());

            for (auto surface : surfaceBucket)
                surface->target()->removeComponent(surface);

            auto newNode = Node::create()
                ->addComponent(Transform::create())
                ->addComponent(BoundingBox::create());

            patchNodeFromPartitions(newNode, surfaceBucket.front(), partitionNode, assetLibrary);

            node->addChild(newNode);
        }

        node->component<Transform>()->matrix(math::mat4());
        node->component<Transform>()->updateModelToWorldMatrix();
    }
}

MeshPartitioner::OctreeNodePtr
MeshPartitioner::pickBestPartitions(OctreeNodePtr     root,
                                    const math::vec3& modelMinBound,
                                    const math::vec3& modelMaxBound)
{
    auto currentMinBound = root->_minBound;
    auto currentMaxBound = root->_maxBound;

    auto validNode = OctreeNodePtr();

    if (!contains(math::Box::create(root->_maxBound, root->_minBound), math::Box::create(modelMaxBound, modelMinBound)))
        return root;

    splitNode(root, math::mat4());

    for (auto child : root->_children)
    {
        if (contains(math::Box::create(child->_maxBound, child->_minBound), math::Box::create(modelMaxBound, modelMinBound)))
        {
            validNode = pickBestPartitions(child, modelMinBound, modelMaxBound);

            break;
        }
    }

    if (validNode == nullptr)
    {
        root->_children.clear();

        return root;
    }

    return validNode;
}

MeshPartitioner::OctreeNodePtr
MeshPartitioner::ensurePartitionSizeIsValid(OctreeNodePtr       node,
                                            const math::vec3&   maxSize)
{
    const auto nodeSize = node->_maxBound - node->_minBound;

    if (nodeSize.x > maxSize.x ||
        nodeSize.y > maxSize.y ||
        nodeSize.z > maxSize.z)
    {
        splitNode(node, math::mat4());

        for (auto child : node->_children)
        {
            ensurePartitionSizeIsValid(child, maxSize);
        }
    }

    return node;
}

MeshPartitioner::OctreeNodePtr
MeshPartitioner::buildPartitions(const std::vector<Surface::Ptr>&   surfaces,
                                 Node::Ptr                          root,
                                 bool                               transformPositions,
                                 const math::mat4&                  transformMatrix)
{
    if (surfaces.empty())
        return nullptr;

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
    _vertexSize = vertexSize;

    {
        auto attributeOffset = 0;

        for (auto vertexBuffer : referenceGeometry->vertexBuffers())
        {
            if (vertexBuffer->hasAttribute("position"))
            {
                _positionAttributeOffset = attributeOffset + vertexBuffer->attribute("position").offset;

                break;
            }

            attributeOffset += vertexBuffer->vertexSize();
        }
    }

    auto& indices = _indices;
    auto& vertices = _vertices;

    indices.resize(indexCount, 0);
    vertices.resize(vertexCount * vertexSize, 0.0f);

    indices = std::vector<int>(indexCount, 0);
    vertices = std::vector<float>(vertexCount * vertexSize, 0.0f);

    auto indexOffset = 0;
    auto vertexOffset = 0;

    auto minBound = math::vec3(
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max()
    );

    auto maxBound = math::vec3(
        -std::numeric_limits<float>::max(),
        -std::numeric_limits<float>::max(),
        -std::numeric_limits<float>::max()
    );

    for (auto surface : surfaces)
    {
        auto transformMatrix = math::mat4();

        if (transformPositions)
        {
            transformMatrix = surface->target()->component<Transform>()->modelToWorldMatrix();
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
        
                    if (transformPositions)
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

    auto rootPartitionMinBound = minBound;
    auto rootPartitionMaxBound = maxBound;

    if (_options._flags & Options::uniformizeSize)
    {
        rootPartitionMinBound = _worldMinBound;
        rootPartitionMaxBound = _worldMaxBound;
    }

    auto octreeRoot = OctreeNodePtr(new OctreeNode(
        0,
        rootPartitionMinBound,
        rootPartitionMaxBound,
        nullptr
    ));

    if (_options._flags & Options::uniformizeSize)
    {
        auto nodeMinBound = minBound;
        auto nodeMaxBound = maxBound;

        nodeMinBound = math::vec3(transformMatrix * math::vec4(nodeMinBound, 1.f));
        nodeMaxBound = math::vec3(transformMatrix * math::vec4(nodeMaxBound, 1.f));

        octreeRoot = pickBestPartitions(octreeRoot, nodeMinBound, nodeMaxBound);

        _baseDepth = octreeRoot->_depth;
    }
    else
    {
        _baseDepth = 0;
    }

    //octreeRoot = ensurePartitionSizeIsValid(
    //    octreeRoot,
    //    _options._partitionMaxSizeFunction
    //        ? _options._partitionMaxSizeFunction(root)
    //        : defaultPartitionMaxSizeFunction(root)
    //);

    for (auto i = 0; i < indices.size(); i += 3)
    {
        insertTriangle(octreeRoot, i / 3, transformMatrix);
    }

    return octreeRoot;
}

void
MeshPartitioner::patchNodeFromPartitions(Node::Ptr          node,
                                         Surface::Ptr       referenceSurface,
                                         OctreeNodePtr      partitionNode,
                                         AssetLibrary::Ptr  assetLibrary)
{
    const auto modelToWorldMatrix = node->component<Transform>()->modelToWorldMatrix();
    const auto worldToModelMatrix = math::inverse(modelToWorldMatrix);

    auto referenceGeometry = referenceSurface->geometry();
    auto referenceMaterial = referenceSurface->material();
    auto referenceEffect = referenceSurface->effect();

    static auto currentGeometryId = 0;
    static auto currentMaterialId = 0;

    auto maxDepth = computeDepth(partitionNode);

    auto pendingNodes = std::queue<OctreeNodePtr>();

    pendingNodes.push(partitionNode);

    while (!pendingNodes.empty())
    {
        auto pendingNode = pendingNodes.front();

        pendingNodes.pop();

        for (auto childPartitionNode : pendingNode->_children)
        {
            pendingNodes.push(childPartitionNode);
        }

        auto newGeometries = std::list<Geometry::Ptr>();

        if (!pendingNode->_children.empty())
        {
            for (const auto& triangles : pendingNode->_sharedTriangles)
            {
                auto newGeometry = createGeometry(referenceGeometry, triangles);

                if (newGeometry == nullptr)
                    continue;

                if (_options._flags & Options::useBorderAsSharedTriangles)
                {
                    newGeometry->data()->set("isSharedPartition", true);

                    newGeometry->data()->set("borderMinPrecision", _options._borderMinPrecision);
                    newGeometry->data()->set("borderMaxDeltaPrecision", _options._borderMaxDeltaPrecision);
                }

                newGeometries.push_back(newGeometry);
            }
        }
        else
        {
            for (const auto& triangles : pendingNode->_triangles)
            {
                auto newGeometry = createGeometry(referenceGeometry, triangles);

                if (newGeometry == nullptr)
                    continue;

                newGeometries.push_back(newGeometry);
            }
        }

        for (auto newGeometry : newGeometries)
        {
            if (newGeometry == nullptr)
                continue;

            auto minBound = pendingNode->_minBound;
            auto maxBound = pendingNode->_maxBound;

            if (_options._flags & Options::uniformizeSize)
            {
                minBound = math::vec3(worldToModelMatrix * math::vec4(minBound, 1.f));
                maxBound = math::vec3(worldToModelMatrix * math::vec4(maxBound, 1.f));
            }

            newGeometry->data()->set("partitioningMaxDepth", maxDepth - _baseDepth);
            newGeometry->data()->set("partitioningDepth", pendingNode->_depth - _baseDepth);
            newGeometry->data()->set("partitioningMinBound", minBound);
            newGeometry->data()->set("partitioningMaxBound", maxBound);

            auto geometryName = "geometry_" + std::to_string(currentGeometryId++);
            auto geometryNameLastSeparatorPos = geometryName.find_last_of("/\\");

            if (geometryNameLastSeparatorPos != std::string::npos)
                geometryName = geometryName.substr(geometryNameLastSeparatorPos + 1);

            assetLibrary->geometry(
                geometryName,
                newGeometry
            );

            auto newSurface = Surface::create(
                newGeometry,
                referenceMaterial,
                referenceEffect
            );

            if (_options._flags & Options::createOneNodePerSurface)
            {
                static auto nameId = 0;

                auto newNode = Node::create("node" + std::to_string(nameId++))
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
}

void
MeshPartitioner::processBorders(OctreeNodePtr   partitionNode,
                                int             borderMinPrecision,
                                int             borderMaxDeltaPrecision)
{
    auto pendingNodes = std::queue<OctreeNodePtr>();

    pendingNodes.push(partitionNode);

    while (!pendingNodes.empty())
    {
        auto pendingNode = pendingNodes.front();

        pendingNodes.pop();

        for (auto childPartitionNode : pendingNode->_children)
        {
            pendingNodes.push(childPartitionNode);
        }

        if (!pendingNode->_children.empty())
            continue;

        const auto isXMinLimit = pendingNode->_minBound.x <= partitionNode->_minBound.x;
        const auto isYMinLimit = pendingNode->_minBound.y <= partitionNode->_minBound.y;
        const auto isZMinLimit = pendingNode->_minBound.z <= partitionNode->_minBound.z;
        
        const auto isXMaxLimit = pendingNode->_maxBound.x >= partitionNode->_maxBound.x;
        const auto isYMaxLimit = pendingNode->_maxBound.y >= partitionNode->_maxBound.y;
        const auto isZMaxLimit = pendingNode->_maxBound.z >= partitionNode->_maxBound.z;

        const auto boxSize = pendingNode->_maxBound - pendingNode->_minBound;

        const auto borderSize = boxSize / static_cast<float>(std::pow(2.0f, borderMinPrecision));

        const auto minClusterIndex = math::one<math::vec3>();
        const auto maxClusterIndex = boxSize / borderSize - math::one<math::vec3>();

        for (auto& triangles : pendingNode->_triangles)
        for (auto triangleIt = triangles.begin(); triangleIt != triangles.end();)
        {
            const auto triangleIndex = *triangleIt;

            const auto positions = std::vector<math::vec3>
            {
                math::make_vec3(
                    &_vertices.at(_indices.at(triangleIndex * 3 + 0) * _vertexSize + _positionAttributeOffset)),
                    math::make_vec3(
                    &_vertices.at(_indices.at(triangleIndex * 3 + 1) * _vertexSize + _positionAttributeOffset)),
                    math::make_vec3(
                    &_vertices.at(_indices.at(triangleIndex * 3 + 2) * _vertexSize + _positionAttributeOffset))
            };

            auto vertexInBorderCount = 0;

            for (const auto& position : positions)
            {
                const auto localPosition = position - pendingNode->_minBound;

                const auto clusterIndex = localPosition / borderSize;

                if ((!isXMinLimit && clusterIndex.x <= minClusterIndex.x) ||
                    (!isYMinLimit && clusterIndex.y <= minClusterIndex.y) ||
                    (!isZMinLimit && clusterIndex.z <= minClusterIndex.z) ||
                    (!isXMaxLimit && clusterIndex.x >= maxClusterIndex.x) ||
                    (!isYMaxLimit && clusterIndex.y >= maxClusterIndex.y) ||
                    (!isZMaxLimit && clusterIndex.z >= maxClusterIndex.z))
                {
                    ++vertexInBorderCount;
                }
            }

            if (vertexInBorderCount >= 1)
            {
                if (!pendingNode->_parent.expired())
                {
                    auto& sharedTriangles = pendingNode->_parent.lock()->_sharedTriangles;

                    if (sharedTriangles.back().size() < _options._maxTriangleCountPerNode)
                        sharedTriangles.back().push_back(triangleIndex);
                    else
                        sharedTriangles.push_back(std::vector<int>{ triangleIndex });
                }

                triangleIt = triangles.erase(triangleIt);
            }
            else
            {
                ++triangleIt;
            }
        }
    }
}

Geometry::Ptr
MeshPartitioner::createGeometry(Geometry::Ptr referenceGeometry, const std::vector<int>& triangleIndices)
{
    if (triangleIndices.empty())
        return nullptr;

    auto geometry = Geometry::create();

    auto vertexCount = 0;
    auto indexCount = triangleIndices.size() * 3;

    const auto vertexSize = referenceGeometry->vertexSize();

    auto globalIndexToLocalIndexMap = std::unordered_map<int, unsigned short>(indexCount);

    auto indices = std::vector<unsigned short>(indexCount);

    auto currentIndex = 0;

    for (auto i = 0; i < triangleIndices.size(); ++i)
    {
        for (auto j = 0; j < 3; ++j)
        {
            auto result = globalIndexToLocalIndexMap.insert(std::make_pair(
                _indices.at(triangleIndices.at(i) * 3 + j),
                currentIndex));

            if (result.second)
            {
                indices[i * 3 + j] = currentIndex;

                ++currentIndex;

                ++vertexCount;
            }
            else
            {
                indices[i * 3 + j] = result.first->second;
            }
        }
    }

    geometry->indices(IndexBuffer::create(referenceGeometry->indices()->context(), indices));

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
                auto globalIndex = _indices.at(triangleIndex * 3 + j);
                auto localIndex = globalIndexToLocalIndexMap.at(globalIndex);

                std::copy(
                    _vertices.begin() + globalIndex * vertexSize + globalAttributeOffset,
                    _vertices.begin() + globalIndex * vertexSize + localVertexSize + globalAttributeOffset,
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

    return geometry;
}

void
MeshPartitioner::insertTriangle(OctreeNodePtr       partitionNode,
                                int                 triangleIndex,
                                const math::mat4&   transformMatrix)
{
    if (partitionNode->_children.empty())
    {
        auto& triangles = partitionNode->_triangles.back();

        if (triangles.size() < _options._maxTriangleCountPerNode)
        {
            triangles.push_back(triangleIndex);

            return;
        }
        else
        {
            if (partitionNode->_depth >= _options._maxDepth)
            {
                partitionNode->_triangles.push_back(std::vector<int>{ triangleIndex });

                return;
            }
            else
            {
                splitNode(partitionNode, transformMatrix);
            }
        }
    }

    auto positions = std::vector<math::vec3>
    {
        math::make_vec3(
            &_vertices.at(_indices.at(triangleIndex * 3 + 0) * _vertexSize + _positionAttributeOffset)),
        math::make_vec3(
            &_vertices.at(_indices.at(triangleIndex * 3 + 1) * _vertexSize + _positionAttributeOffset)),
        math::make_vec3(
            &_vertices.at(_indices.at(triangleIndex * 3 + 2) * _vertexSize + _positionAttributeOffset))
    };

    auto localIndices = std::vector<int>();

    for (const auto& position : positions)
    {
        const auto transformedPosition = math::vec3(transformMatrix * math::vec4(position, 1.f));

        auto nodeCenter = (partitionNode->_maxBound + partitionNode->_minBound) * 0.5f;

        auto x = transformedPosition.x < nodeCenter.x ? 0 : 1;
        auto y = transformedPosition.y < nodeCenter.y ? 0 : 1;
        auto z = transformedPosition.z < nodeCenter.z ? 0 : 1;

        localIndices.push_back(indexAt(x, y, z));
    }

    if (localIndices.at(0) == localIndices.at(1) &&
        localIndices.at(1) == localIndices.at(2))
    {
        insertTriangle(partitionNode->_children.at(localIndices.at(0)), triangleIndex, transformMatrix);
    }
    else
    {
        auto& sharedTriangles = partitionNode->_sharedTriangles;

        if (sharedTriangles.back().size() < _options._maxTriangleCountPerNode)
            sharedTriangles.back().push_back(triangleIndex);
        else
            sharedTriangles.push_back(std::vector<int>{ triangleIndex });
    }
}

void
MeshPartitioner::splitNode(OctreeNodePtr        partitionNode,
                           const math::mat4&    transformMatrix)
{
    partitionNode->_children.resize(8);

    auto nodeCenter = (partitionNode->_maxBound + partitionNode->_minBound) * 0.5f;
    auto nodeHalfSize = (partitionNode->_maxBound - partitionNode->_minBound) * 0.5f;

    for (auto x = 0; x < 2; ++x)
    for (auto y = 0; y < 2; ++y)
    for (auto z = 0; z < 2; ++z)
    {
        partitionNode->_children[indexAt(x, y, z)] = OctreeNodePtr(new OctreeNode(
            partitionNode->_depth + 1,
            partitionNode->_minBound + math::vec3(
                x * nodeHalfSize.x,
                y * nodeHalfSize.y,
                z * nodeHalfSize.z),
            partitionNode->_minBound + math::vec3(
                (x + 1) * nodeHalfSize.x,
                (y + 1) * nodeHalfSize.y,
                (z + 1) * nodeHalfSize.z),
            partitionNode));
    }

    for (const auto& triangles : partitionNode->_triangles)
    for (auto triangle : triangles)
    {
        insertTriangle(partitionNode, triangle, transformMatrix);
    }

    partitionNode->_triangles = std::vector<std::vector<int>>(1, std::vector<int>());
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

        for (auto childPartitionNode : pendingNode->_children)
        {
            pendingNodes.push(childPartitionNode);
        }

        depth = std::max(depth, pendingNode->_depth);
    }

    return depth;
}
