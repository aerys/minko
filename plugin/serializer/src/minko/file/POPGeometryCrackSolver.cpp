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
#include "minko/data/HalfEdgeCollection.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/POPGeometryCrackSolver.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::render;
using namespace minko::scene;

POPGeometryCrackSolver::POPGeometryCrackSolver()
{
}

void
POPGeometryCrackSolver::process(Node::Ptr& node, AssetLibrary::Ptr assetLibrary)
{
    auto meshNodes = NodeSet::create(node)
        ->descendants(true)
        ->where([](Node::Ptr descendant) -> bool { return descendant->hasComponent<Surface>(); });

    for (auto meshNode : meshNodes->nodes())
    {
        for (auto surface : meshNode->components<Surface>())
        {
            auto geometry = surface->geometry();

            if (acceptsGeometry(meshNode, surface, geometry))
                processGeometry(geometry, assetLibrary);
        }
    }
}

bool
POPGeometryCrackSolver::acceptsGeometry(Node::Ptr node, Surface::Ptr surface, Geometry::Ptr geometry)
{
    return geometry->data()->hasProperty("type") && geometry->data()->get<std::string>("type") == std::string("pop") &&
           geometry->data()->hasProperty("hasNeighbors") && geometry->data()->get<bool>("hasNeighbors");
}

void
POPGeometryCrackSolver::processGeometry(Geometry::Ptr geometry, AssetLibrary::Ptr assetLibrary)
{
    auto halfEdges = HalfEdgeCollection::create(std::vector<unsigned int>(
        geometry->indices()->data().begin(),
        geometry->indices()->data().end()
    ));

    const auto numIndices = geometry->indices()->numIndices();
    const auto numVertices = geometry->numVertices();

    auto regularVertexSet = std::set<unsigned short>();
    auto protectedVertexSet = std::set<unsigned short>();

    for (auto halfEdge : halfEdges->halfEdges())
    {
        auto vertexIndex = halfEdge->startNodeId();

        if (halfEdge->adjacent() == nullptr)
        {
            protectedVertexSet.insert(halfEdge->startNodeId());
            protectedVertexSet.insert(halfEdge->next()->startNodeId());
        }
        else if (halfEdge->prec()->adjacent() == nullptr)
        {
            protectedVertexSet.insert(halfEdge->startNodeId());
            protectedVertexSet.insert(halfEdge->prec()->startNodeId());
        }
        else
        {
            regularVertexSet.insert(halfEdge->startNodeId());
        }
    }

    const auto protectedFlagVertexAttributeSize = 1u;
    const auto protectedFlagVertexAttributeOffset = 0u;

    auto protectedFlagVertexBufferData = std::vector<float>(numVertices * protectedFlagVertexAttributeSize);

    for (auto regularVertex : regularVertexSet)
    {
        const auto protectedFlagVertexBufferDataOffset =
            regularVertex * protectedFlagVertexAttributeSize + protectedFlagVertexAttributeOffset;

        protectedFlagVertexBufferData[protectedFlagVertexBufferDataOffset] = 0.f;
    }

    for (auto protectedVertex : protectedVertexSet)
    {
        const auto protectedFlagVertexBufferDataOffset =
            protectedVertex * protectedFlagVertexAttributeSize + protectedFlagVertexAttributeOffset;

        protectedFlagVertexBufferData[protectedFlagVertexBufferDataOffset] = 1.f;
    }

    auto protectedFlagVertexBuffer = VertexBuffer::create(assetLibrary->context(), protectedFlagVertexBufferData);

    protectedFlagVertexBuffer->addAttribute(
        "popProtected",
        protectedFlagVertexAttributeSize,
        protectedFlagVertexAttributeOffset
    );

    geometry->addVertexBuffer(protectedFlagVertexBuffer);
}
