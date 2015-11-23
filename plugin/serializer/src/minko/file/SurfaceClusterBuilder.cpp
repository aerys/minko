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
#include "minko/file/SurfaceClusterBuilder.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;
using namespace minko::scene;

SurfaceClusterBuilder::NodeInfo::NodeInfo() :
    surfaces(),
    numVertices(0u),
    numTriangles(0u),
    bounds(),
    size(),
    xyArea(0.f),
    xzArea(0.f),
    yzArea(0.f),
    volume(0.f),
    vertexDensity(0.f),
    triangleDensity(0.f),
    worldVolumeRatio(0.f),
    worldNumVerticesRatio(0.f),
    worldNumTrianglesRatio(0.f),
    worldSizeRatio(),
    worldXyAreaRatio(0.f),
    worldXzAreaRatio(0.f),
    worldYzAreaRatio(0.f)
{
}

SurfaceClusterBuilder::SurfaceClusterBuilder() :
    AbstractWriterPreprocessor<Node::Ptr>(),
    _statusChanged(StatusChangedSignal::create()),
    _progressRate(0.f)
{
}

void
SurfaceClusterBuilder::process(Node::Ptr& node, AssetLibrary::Ptr assetLibrary)
{
    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(shared_from_this(), "SurfaceClusterBuilder: start");

    if (!_surfaceClusters.empty())
    {
        if (!node->hasComponent<Transform>())
            node->addComponent(Transform::create());

        node->component<Transform>()->updateModelToWorldMatrix();

        cacheNodeInfo(node, _rootNodeInfo, _surfaceNodeInfo);

        if (!_surfaceNodeInfo.empty())
            buildClusters();
    }

    _progressRate = 1.f;

    if (statusChanged() && statusChanged()->numCallbacks() > 0u)
        statusChanged()->execute(shared_from_this(), "SurfaceClusterBuilder: stop");
}

void
SurfaceClusterBuilder::cacheNodeInfo(Node::Ptr                  root,
                                     NodeInfo&                  rootNodeInfo,
                                     std::vector<NodeInfo>&     surfaceNodeInfo)
{
    auto surfaces = std::vector<Surface::Ptr>();

    auto surfaceNodes = NodeSet::create(root)
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<Surface>(); });

    surfaces.reserve(surfaceNodes->size());

    for (auto surfaceNode : surfaceNodes->nodes())
        for (auto surface : surfaceNode->components<Surface>())
            surfaces.push_back(surface);

    surfaceNodeInfo.reserve(surfaces.size());

    for (auto surface : surfaces)
    {
        surfaceNodeInfo.push_back(NodeInfo());

        auto& nodeInfo = surfaceNodeInfo.back();

        auto target = surface->target();
        auto geometry = surface->geometry();

        if (!target->hasComponent<BoundingBox>())
            target->addComponent(BoundingBox::create());

        auto box = target->component<BoundingBox>()->box();

        nodeInfo.surfaces.push_back(surface);

        nodeInfo.numVertices = geometry->numVertices();
        nodeInfo.numTriangles = geometry->indices()->numIndices() / 3u;

        nodeInfo.bounds = std::make_pair(box->bottomLeft(), box->topRight());
        nodeInfo.size = nodeInfo.bounds.second - nodeInfo.bounds.first;
        nodeInfo.xyArea = nodeInfo.size.x * nodeInfo.size.y;
        nodeInfo.xzArea = nodeInfo.size.x * nodeInfo.size.z;
        nodeInfo.yzArea = nodeInfo.size.y * nodeInfo.size.z;
        nodeInfo.volume = nodeInfo.size.x * nodeInfo.size.y * nodeInfo.size.z;
        nodeInfo.vertexDensity = nodeInfo.numVertices / nodeInfo.volume;
        nodeInfo.triangleDensity = nodeInfo.numTriangles / nodeInfo.volume;

        rootNodeInfo.surfaces.push_back(surface);

        rootNodeInfo.numVertices += nodeInfo.numVertices;
        rootNodeInfo.numTriangles += nodeInfo.numTriangles;

        rootNodeInfo.bounds = std::make_pair(
            math::vec3(
                math::min(nodeInfo.bounds.first.x, rootNodeInfo.bounds.first.x),
                math::min(nodeInfo.bounds.first.y, rootNodeInfo.bounds.first.y),
                math::min(nodeInfo.bounds.first.z, rootNodeInfo.bounds.first.z)
            ),
            math::vec3(
                math::max(nodeInfo.bounds.second.x, rootNodeInfo.bounds.second.x),
                math::max(nodeInfo.bounds.second.y, rootNodeInfo.bounds.second.y),
                math::max(nodeInfo.bounds.second.z, rootNodeInfo.bounds.second.z)
            )
        );
    }

    rootNodeInfo.size = rootNodeInfo.bounds.second - rootNodeInfo.bounds.first;
    rootNodeInfo.xyArea = rootNodeInfo.size.x * rootNodeInfo.size.y;
    rootNodeInfo.xzArea = rootNodeInfo.size.x * rootNodeInfo.size.z;
    rootNodeInfo.yzArea = rootNodeInfo.size.y * rootNodeInfo.size.z;
    rootNodeInfo.volume = rootNodeInfo.size.x * rootNodeInfo.size.y * rootNodeInfo.size.z;
    rootNodeInfo.vertexDensity = rootNodeInfo.numVertices / rootNodeInfo.volume;
    rootNodeInfo.triangleDensity = rootNodeInfo.numTriangles / rootNodeInfo.volume;

    for (auto& nodeInfo : surfaceNodeInfo)
    {
        nodeInfo.worldVolumeRatio = nodeInfo.volume / rootNodeInfo.volume;
        nodeInfo.worldNumVerticesRatio = nodeInfo.numVertices / float(rootNodeInfo.numVertices);
        nodeInfo.worldNumTrianglesRatio = nodeInfo.numTriangles / float(rootNodeInfo.numTriangles);
        nodeInfo.worldSizeRatio = nodeInfo.size / rootNodeInfo.size;
        nodeInfo.worldXyAreaRatio = nodeInfo.xyArea / rootNodeInfo.xyArea;
        nodeInfo.worldXzAreaRatio = nodeInfo.xzArea / rootNodeInfo.xzArea;
        nodeInfo.worldYzAreaRatio = nodeInfo.yzArea / rootNodeInfo.yzArea;
    }
}

SurfaceClusterBuilder::Ptr
SurfaceClusterBuilder::registerSurfaceCluster(scene::Layout layout, const SurfaceClusterPredicateFunction& predicate)
{
    _surfaceClusters.emplace_back(layout, predicate);

    return std::static_pointer_cast<SurfaceClusterBuilder>(shared_from_this());
}


void
SurfaceClusterBuilder::buildClusters()
{
    for (const auto& nodeInfo : _surfaceNodeInfo)
    {
        for (const auto& surfaceClusterEntry : _surfaceClusters)
        {
            if (clusterAccepts(surfaceClusterEntry, nodeInfo))
            {
                addToCluster(surfaceClusterEntry, nodeInfo);

                break;
            }
        }
    }
}

bool
SurfaceClusterBuilder::clusterAccepts(const SurfaceClusterEntry&   surfaceClusterEntry,
                                      const NodeInfo&              nodeInfo)
{
    return surfaceClusterEntry.predicate(nodeInfo);
}

void
SurfaceClusterBuilder::addToCluster(const SurfaceClusterEntry&   surfaceClusterEntry,
                                    const NodeInfo&              nodeInfo)
{
    if (nodeInfo.surfaces.empty())
        return;

    auto target = nodeInfo.surfaces.front()->target();

    target->layout(target->layout() | surfaceClusterEntry.layout);
}
