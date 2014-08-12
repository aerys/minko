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

#include "SurfaceTest.hpp"

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::component;

void
SurfaceTest::SetUp()
{
    _sceneManager = SceneManager::create(MinkoTests::context());
    
    auto loader = _sceneManager->assets()->loader();
    loader->options()->loadAsynchronously(false);
    loader->queue("effect/Basic.effect");
    loader->load();

    auto redMaterial = material::BasicMaterial::create();
    redMaterial->diffuseColor(math::vec4(1.f, 0.f, 0.f, 1.f));

    auto greenMaterial = material::BasicMaterial::create();
    greenMaterial->diffuseColor(math::vec4(0.f, 1.f, 0.f, 1.f));

    auto blueMaterial = material::BasicMaterial::create();
    blueMaterial->diffuseColor(math::vec4(0.f, 0.f, 1.f, 1.f));

    _sceneManager->assets()
        ->geometry("cube", geometry::CubeGeometry::create(MinkoTests::context()))
        ->geometry("sphere", geometry::SphereGeometry::create(MinkoTests::context()))
        ->geometry("quad", geometry::QuadGeometry::create(MinkoTests::context()))
        ->material("red", redMaterial)
        ->material("green", greenMaterial)
        ->material("blue", blueMaterial);
}

TEST_F(SurfaceTest, Create)
{
    try
    {
        auto s = Surface::create(
            geometry::Geometry::create(),
            material::Material::create(),
            _sceneManager->assets()->effect("effect/Basic.effect")
        );
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}

TEST_F(SurfaceTest, SingleSurface)
{
    auto node = scene::Node::create("a");

    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("cube"),
        _sceneManager->assets()->material("red"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));

    ASSERT_EQ(node->data().get<uint>("geometry.length"), 1);
    ASSERT_EQ(node->data().get<uint>("material.length"), 1);
    ASSERT_EQ(node->data().get<uint>("effect.length"), 1);
    ASSERT_TRUE(node->data().hasProperty("geometry[0].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].position"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[0].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].uv"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[0].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[0].diffuseColor"), math::vec4(1.f, 0.f, 0.f, 1.f));
}

TEST_F(SurfaceTest, MultipleSurfaces)
{
    auto node = scene::Node::create("a");
    
    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("cube"),
        _sceneManager->assets()->material("red"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));

    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("sphere"),
        _sceneManager->assets()->material("green"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));

    ASSERT_EQ(node->data().get<uint>("geometry.length"), 2);
    ASSERT_EQ(node->data().get<uint>("material.length"), 2);
    ASSERT_EQ(node->data().get<uint>("effect.length"), 2);
    ASSERT_TRUE(node->data().hasProperty("geometry[0].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].position"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[0].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].uv"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[0].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[0].diffuseColor"), math::vec4(1.f, 0.f, 0.f, 1.f));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].position"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].uv"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[1].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[1].diffuseColor"), math::vec4(0.f, 1.f, 0.f, 1.f));
}

TEST_F(SurfaceTest, RemoveFirstSurface)
{
    auto node = scene::Node::create("a");
    
    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("cube"),
        _sceneManager->assets()->material("red"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));
    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("sphere"),
        _sceneManager->assets()->material("green"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));
    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("quad"),
        _sceneManager->assets()->material("blue"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));

    ASSERT_EQ(node->data().get<uint>("geometry.length"), 3);
    ASSERT_EQ(node->data().get<uint>("material.length"), 3);
    ASSERT_EQ(node->data().get<uint>("effect.length"), 3);
    ASSERT_TRUE(node->data().hasProperty("geometry[0].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].position"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[0].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].uv"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[0].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[0].diffuseColor"), math::vec4(1.f, 0.f, 0.f, 1.f));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].position"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].uv"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[1].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[1].diffuseColor"), math::vec4(0.f, 1.f, 0.f, 1.f));
    ASSERT_TRUE(node->data().hasProperty("geometry[2].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[2].position"), _sceneManager->assets()->geometry("quad")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[2].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[2].uv"), _sceneManager->assets()->geometry("quad")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[2].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[2].diffuseColor"), math::vec4(0.f, 0.f, 1.f, 1.f));

    node->removeComponent(node->component<Surface>(0));

    ASSERT_EQ(node->data().get<uint>("geometry.length"), 2);
    ASSERT_EQ(node->data().get<uint>("material.length"), 2);
    ASSERT_EQ(node->data().get<uint>("effect.length"), 2);
    ASSERT_TRUE(node->data().hasProperty("geometry[0].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].position"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[0].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].uv"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[0].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[0].diffuseColor"), math::vec4(0.f, 1.f, 0.f, 1.f));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].position"), _sceneManager->assets()->geometry("quad")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].uv"), _sceneManager->assets()->geometry("quad")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[1].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[1].diffuseColor"), math::vec4(0.f, 0.f, 1.f, 1.f));
}

TEST_F(SurfaceTest, RemoveNthSurface)
{
    auto node = scene::Node::create("a");

    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("cube"),
        _sceneManager->assets()->material("red"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));
    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("sphere"),
        _sceneManager->assets()->material("green"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));
    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("quad"),
        _sceneManager->assets()->material("blue"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));

    ASSERT_EQ(node->data().get<uint>("geometry.length"), 3);
    ASSERT_EQ(node->data().get<uint>("material.length"), 3);
    ASSERT_EQ(node->data().get<uint>("effect.length"), 3);
    ASSERT_TRUE(node->data().hasProperty("geometry[0].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].position"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[0].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].uv"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[0].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[0].diffuseColor"), math::vec4(1.f, 0.f, 0.f, 1.f));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].position"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].uv"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[1].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[1].diffuseColor"), math::vec4(0.f, 1.f, 0.f, 1.f));
    ASSERT_TRUE(node->data().hasProperty("geometry[2].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[2].position"), _sceneManager->assets()->geometry("quad")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[2].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[2].uv"), _sceneManager->assets()->geometry("quad")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[2].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[2].diffuseColor"), math::vec4(0.f, 0.f, 1.f, 1.f));

    node->removeComponent(node->component<Surface>(1));

    ASSERT_EQ(node->data().get<uint>("geometry.length"), 2);
    ASSERT_EQ(node->data().get<uint>("material.length"), 2);
    ASSERT_EQ(node->data().get<uint>("effect.length"), 2);
    ASSERT_TRUE(node->data().hasProperty("geometry[0].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].position"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[0].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].uv"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[0].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[0].diffuseColor"), math::vec4(1.f, 0.f, 0.f, 1.f));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].position"), _sceneManager->assets()->geometry("quad")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].uv"), _sceneManager->assets()->geometry("quad")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[1].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[1].diffuseColor"), math::vec4(0.f, 0.f, 1.f, 1.f));
}

TEST_F(SurfaceTest, RemoveLastSurface)
{
    auto node = scene::Node::create("a");

    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("cube"),
        _sceneManager->assets()->material("red"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));
    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("sphere"),
        _sceneManager->assets()->material("green"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));
    node->addComponent(Surface::create(
        _sceneManager->assets()->geometry("quad"),
        _sceneManager->assets()->material("blue"),
        _sceneManager->assets()->effect("effect/Basic.effect")
    ));

    ASSERT_EQ(node->data().get<uint>("geometry.length"), 3);
    ASSERT_EQ(node->data().get<uint>("material.length"), 3);
    ASSERT_EQ(node->data().get<uint>("effect.length"), 3);
    ASSERT_TRUE(node->data().hasProperty("geometry[0].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].position"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[0].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].uv"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[0].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[0].diffuseColor"), math::vec4(1.f, 0.f, 0.f, 1.f));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].position"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].uv"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[1].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[1].diffuseColor"), math::vec4(0.f, 1.f, 0.f, 1.f));
    ASSERT_TRUE(node->data().hasProperty("geometry[2].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[2].position"), _sceneManager->assets()->geometry("quad")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[2].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[2].uv"), _sceneManager->assets()->geometry("quad")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[2].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[2].diffuseColor"), math::vec4(0.f, 0.f, 1.f, 1.f));

    node->removeComponent(node->component<Surface>(2));

    ASSERT_EQ(node->data().get<uint>("geometry.length"), 2);
    ASSERT_EQ(node->data().get<uint>("material.length"), 2);
    ASSERT_EQ(node->data().get<uint>("effect.length"), 2);
    ASSERT_TRUE(node->data().hasProperty("geometry[0].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].position"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[0].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[0].uv"), _sceneManager->assets()->geometry("cube")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[0].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[0].diffuseColor"), math::vec4(1.f, 0.f, 0.f, 1.f));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].position"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].position"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("position"));
    ASSERT_TRUE(node->data().hasProperty("geometry[1].uv"));
    ASSERT_EQ(node->data().get<render::VertexAttribute>("geometry[1].uv"), _sceneManager->assets()->geometry("sphere")->getVertexAttribute("uv"));
    ASSERT_TRUE(node->data().hasProperty("material[1].diffuseColor"));
    ASSERT_EQ(node->data().get<math::vec4>("material[1].diffuseColor"), math::vec4(0.f, 1.f, 0.f, 1.f));
}
