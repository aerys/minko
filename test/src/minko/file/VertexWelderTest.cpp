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
#include "minko/file/VertexWelder.hpp"
#include "minko/file/VertexWelderTest.hpp"
#include "minko/geometry/CubeGeometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/scene/Node.hpp"

using namespace minko;
using namespace minko::file;

scene::Node::Ptr
VertexWelderTest::createScene()
{
    auto root = scene::Node::create("root")
        ->addComponent(component::SceneManager::create(MinkoTests::canvas()));

    auto assetLibrary = root->component<component::SceneManager>()->assets();

    auto meshGeometry = geometry::CubeGeometry::create(assetLibrary->context());
    meshGeometry->computeTangentSpace(false);

    auto mesh = scene::Node::create("mesh")
        ->addComponent(component::Surface::create(
            meshGeometry,
            material::Material::create(),
            nullptr
        ));

    root->addChild(mesh);

    return root;
}

TEST_F(VertexWelderTest, Create)
{
    auto vertexWelder = VertexWelder::create();
}

TEST_F(VertexWelderTest, Process)
{
    auto scene = createScene();

    auto vertexWelder = VertexWelder::create();

    vertexWelder->process(scene, scene->component<component::SceneManager>()->assets());
}

TEST_F(VertexWelderTest, VertexAttributeConservativeOrdering)
{
    auto scene = createScene();

    auto mesh = scene->children()[0u];
    auto geometry = mesh->component<component::Surface>()->geometry();
    auto sourceVertexAttributes = std::vector<render::VertexAttribute>();
    auto destinationVertexAttributes = std::vector<render::VertexAttribute>();

    for (auto vertexBuffer : geometry->vertexBuffers())
        for (const auto& vertexAttribute : vertexBuffer->attributes())
            sourceVertexAttributes.emplace_back(vertexAttribute);

    auto vertexWelder = VertexWelder::create();

    vertexWelder->process(scene, scene->component<component::SceneManager>()->assets());

    for (auto vertexBuffer : geometry->vertexBuffers())
        for (const auto& vertexAttribute : vertexBuffer->attributes())
            destinationVertexAttributes.emplace_back(vertexAttribute);

    ASSERT_EQ(sourceVertexAttributes.size(), destinationVertexAttributes.size());

    for (auto i = 0u; i < sourceVertexAttributes.size(); ++i)
        ASSERT_EQ(sourceVertexAttributes.at(i).name, destinationVertexAttributes.at(i).name);
}
