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

#include "minko/component/RendererTest.hpp"

using namespace minko;
using namespace minko::component;

TEST_F(RendererTest, Create)
{
    Renderer::create();
}

TEST_F(RendererTest, AddAndRemoveSurfaces)
{
    auto fx = MinkoTests::loadEffect("effect/Basic.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create()
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(PerspectiveCamera::create(1.f))
        ->addComponent(renderer);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto s1 = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material,
        fx
    );

    auto s2 = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material,
        fx
    );

    auto s3 = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material,
        fx
    );

    root->addComponent(s1);
    root->addComponent(s2);
    root->addComponent(s3);
    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 3);

    root->removeComponent(s1);
    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 2);

    root->removeComponent(s2);
    root->removeComponent(s3);
    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 0);
}

TEST_F(RendererTest, AddAndRemoveSurfaceBubbleUp)
{
    auto fx = MinkoTests::loadEffect("effect/Basic.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create()
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(PerspectiveCamera::create(1.f))
        ->addComponent(renderer);

    auto surfaceNode = scene::Node::create();
    root->addChild(surfaceNode);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto s1 = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material,
        fx
    );

    surfaceNode->addComponent(s1);
    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 1);

    surfaceNode->removeComponent(s1);
    surfaceNode->addComponent(s1);
    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 1);
}

TEST_F(RendererTest, OneSurfaceLayoutMaskFail)
{
    auto fx = MinkoTests::loadEffect("effect/Basic.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create()
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(PerspectiveCamera::create(1.f))
        ->addComponent(renderer);

    auto surfaceNode = scene::Node::create();
    root->addChild(surfaceNode);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto surface = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material,
        fx
    );

    surface->layoutMask(scene::BuiltinLayout::DEBUG_ONLY);
    surfaceNode->addComponent(surface);

    renderer->render(MinkoTests::canvas()->context());

    ASSERT_EQ(renderer->numDrawCalls(), 0);
}

TEST_F(RendererTest, OneSurfaceLayoutMaskPass)
{
    auto fx = MinkoTests::loadEffect("effect/Basic.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create()
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(PerspectiveCamera::create(1.f))
        ->addComponent(renderer);

    auto surfaceNode = scene::Node::create();
    root->addChild(surfaceNode);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto surface = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material,
        fx
    );

    surface->layoutMask(scene::BuiltinLayout::DEFAULT);
    surfaceNode->addComponent(surface);

    renderer->render(MinkoTests::canvas()->context());

    ASSERT_EQ(renderer->numDrawCalls(), 1);
}

TEST_F(RendererTest, PostProcessEffect)
{
    auto fx = MinkoTests::loadEffect("effect/deferred/OneForwardPassOneDeferredPass.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create()
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(PerspectiveCamera::create(1.f))
        ->addComponent(renderer);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto s1 = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material,
        fx
    );

    auto s2 = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material,
        fx
    );

    auto s3 = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material,
        fx
    );

    root->addComponent(s1);
    root->addComponent(s2);
    root->addComponent(s3);
    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 4);
    root->removeComponent(s1);
    ASSERT_EQ(renderer->numDrawCalls(), 3);
    root->removeComponent(s2);
    ASSERT_EQ(renderer->numDrawCalls(), 2);
    root->removeComponent(s3);
    ASSERT_EQ(renderer->numDrawCalls(), 0);
}
