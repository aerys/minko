/*
Copyright (c) 2013 Aerys

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

#include "gtest/gtest.h"

#include "minko/MinkoTests.hpp"

#include "minko/component/Surface.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/MeshPartitioner.hpp"
#include "minko/file/MeshPartitionerTest.hpp"
#include "minko/file/StreamingOptions.hpp"
#include "minko/geometry/CubeGeometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/scene/Node.hpp"

using namespace minko;
using namespace minko::file;

scene::Node::Ptr
MeshPartitionerTest::createScene()
{
    auto root = scene::Node::create("root")
        ->addComponent(component::Transform::create(math::scale(math::vec3(0.25f))))
        ->addComponent(component::SceneManager::create(MinkoTests::canvas()));

    auto assetLibrary = root->component<component::SceneManager>()->assets();

    auto cubeGeometry0 = geometry::CubeGeometry::create(assetLibrary->context());
    auto cubeGeometry1 = geometry::CubeGeometry::create(assetLibrary->context());

    auto material = material::Material::create();

    auto n0 = scene::Node::create("n0")
        ->addComponent(component::Transform::create(math::translate(math::vec3(1.f, 2.f, 5.f))))
        ->addComponent(component::Surface::create(
            cubeGeometry0,
            material,
            nullptr
        ));

    auto n1 = scene::Node::create("n1")
        ->addComponent(component::Transform::create(
            math::translate(math::vec3(1.f, 2.f, 5.f)) *
            math::rotate(1.5f, math::vec3(0.f, 0.f, 1.f))))
        ->addComponent(component::Surface::create(
            cubeGeometry1,
            material,
            nullptr
        ));

    root->addChild(n0);
    root->addChild(n1);

    return root;
}

void
MeshPartitionerTest::getWorldPositions(const std::vector<component::Surface::Ptr>&   surfaces,
                                       std::vector<math::vec3>&                      positions)
{
    for (auto surface : surfaces)
    {
        auto geometry = surface->geometry();
        auto transformMatrix = surface->target()->component<component::Transform>()->modelToWorldMatrix(true);

        const auto numVertices = geometry->numVertices();
        auto positionVertexBuffer = geometry->vertexBuffer("position");
        const auto& positionVertexAttribute = positionVertexBuffer->attribute("position");

        positions.reserve(positions.size() + numVertices);

        for (auto i = 0u; i < numVertices; ++i)
        {
            auto position = math::make_vec3(
                &positionVertexBuffer->data()[i * *positionVertexAttribute.vertexSize + positionVertexAttribute.offset]
            );

            position = math::vec3(transformMatrix * math::vec4(position, 1.f));

            positions.push_back(position);
        }
    }
}

TEST_F(MeshPartitionerTest, Create)
{
    auto meshPartitioner = MeshPartitioner::create(MeshPartitioner::Options(), StreamingOptions::create());
}

TEST_F(MeshPartitionerTest, Process)
{
    auto scene = createScene();

    auto meshPartitioner = MeshPartitioner::create(MeshPartitioner::Options(), StreamingOptions::create());

    meshPartitioner->process(scene, scene->component<component::SceneManager>()->assets());
}

TEST_F(MeshPartitionerTest, PreservedVertexPositionsWhenMerging)
{
    auto scene = createScene();

    auto sourceN0 = scene->children()[0u];
    auto sourceN1 = scene->children()[1u];

    auto sourceWorldPositions = std::vector<math::vec3>();

    getWorldPositions({
        sourceN0->component<component::Surface>(),
        sourceN1->component<component::Surface>()
    }, sourceWorldPositions);

    auto options = MeshPartitioner::Options();

    options.flags = MeshPartitioner::Options::mergeSurfaces |
                    MeshPartitioner::Options::applyCrackFreePolicy |
                    MeshPartitioner::Options::createOneNodePerSurface;
    options.maxNumIndicesPerNode = 65536;
    options.maxNumTrianglesPerNode = 60000;

    auto meshPartitioner = MeshPartitioner::create(options, StreamingOptions::create());

    meshPartitioner->process(scene, scene->component<component::SceneManager>()->assets());

    auto destinationN0 = scene->children()[2u]->children()[0u]->children()[0u];

    auto destinationWorldPositions = std::vector<math::vec3>();

    getWorldPositions({
        destinationN0->component<component::Surface>()
    }, destinationWorldPositions);

    ASSERT_EQ(sourceWorldPositions.size(), destinationWorldPositions.size());

    for (auto i = 0u; i < sourceWorldPositions.size(); ++i)
    {
        const auto epsilonEqual = math::epsilonEqual(sourceWorldPositions.at(i), destinationWorldPositions.at(i), 1e-5f);

        ASSERT_TRUE(epsilonEqual.x && epsilonEqual.y && epsilonEqual.z);
    }
}
