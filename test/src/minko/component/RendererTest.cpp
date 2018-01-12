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
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
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
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
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

TEST_F(RendererTest, CascadedRemoveNodeAndOutOfSceneNodeChangeLayout)
{
    // Related to smartshape-app#193

    const auto fx = MinkoTests::loadEffect("effect/Basic.effect");
    const auto renderer = Renderer::create();
    const auto root = scene::Node::create()
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
        ->addComponent(renderer);
    const auto material = material::BasicMaterial::create();
    const auto surface = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material,
        fx
    );

    // root
    // |- a
    //    |- b (Surface)

    const auto a = scene::Node::create();
    const auto b = scene::Node::create()
        ->addComponent(surface);
    b->layout(scene::BuiltinLayout::DEFAULT);

    a->addChild(b);
    root->addChild(a);

    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 1);

    auto removedSlot = Signal<scene::Node::Ptr, scene::Node::Ptr, scene::Node::Ptr>::Slot();
    removedSlot = a->removed().connect([&removedSlot, &a, &b](scene::Node::Ptr node,
                                                              scene::Node::Ptr target,
                                                              scene::Node::Ptr parent)
    {
        removedSlot = nullptr;

        // Cascaded node deletion
        a->removeChild(b);
    });

    // Remove parent node
    root->removeChild(a);

    // Change layout of out-of-scene node
    // It can be any arbitrary layout as long as
    // it still matches the Renderer's layoutMask
    b->layout(b->layout() | scene::BuiltinLayout::PICKING);

    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 0);
}

TEST_F(RendererTest, OneSurfaceLayoutMaskFail)
{
    auto fx = MinkoTests::loadEffect("effect/Basic.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create()
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
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
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
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

TEST_F(RendererTest, DeferredPassDrawCallCount)
{
    auto fx = MinkoTests::loadEffect("effect/deferred/OneForwardPassOneDeferredPass.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create()
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
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
    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 3);
    root->removeComponent(s2);
    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 2);
    root->removeComponent(s3);
    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 0);
}

TEST_F(RendererTest, RendererLayoutMaskChanged)
{
    auto fx = MinkoTests::loadEffect("effect/Basic.effect");
    auto renderer = Renderer::create();
    auto root = scene::Node::create()
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
        ->addComponent(renderer);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto s = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material,
        fx
    );

    root->addComponent(s);
    renderer->render(MinkoTests::canvas()->context());
    ASSERT_EQ(renderer->numDrawCalls(), 1);

    renderer->layoutMask(0u);

    renderer->render(MinkoTests::canvas()->context());

    ASSERT_EQ(renderer->numDrawCalls(), 0);
}

TEST_F(RendererTest, Priority)
{
    auto renderer1 = Renderer::create(0, nullptr, nullptr, "default", 2.f);
    auto renderer2 = Renderer::create(0, nullptr, nullptr, "default", 1.f);
    auto renderer3 = Renderer::create(0, nullptr, nullptr, "default", 0.f);
    auto sceneManager = SceneManager::create(MinkoTests::canvas());
    auto root = scene::Node::create()
        ->addComponent(sceneManager)
        ->addComponent(renderer3)
        ->addComponent(renderer1)
        ->addComponent(renderer2);

    int i = 0;

    auto _ = renderer1->renderingBegin()->connect([&](Renderer::Ptr r)
    {
        ASSERT_EQ(i, 0);
        i++;
    });

    auto __ = renderer2->renderingBegin()->connect([&](Renderer::Ptr r)
    {
        ASSERT_EQ(i, 1);
        i++;
    });

    auto ___ = renderer3->renderingBegin()->connect([&](Renderer::Ptr r)
    {
        ASSERT_EQ(i, 2);
        i++;
    });

    sceneManager->nextFrame(0.f, 0.f);

    ASSERT_EQ(i, 3);
}

TEST_F(RendererTest, SetEffect)
{
    auto basic = MinkoTests::loadEffect("effect/Basic.effect");
    auto phong = MinkoTests::loadEffect("effect/Phong.effect");
    auto renderer = Renderer::create(0, nullptr, basic);
    auto sceneManager = SceneManager::create(MinkoTests::canvas());
    auto root = scene::Node::create()
        ->addComponent(sceneManager)
        ->addComponent(Camera::create(math::perspective(.785f, 1.f, 0.1f, 1000.f)))
        ->addComponent(renderer);

    auto material = material::BasicMaterial::create();
    material->diffuseColor(math::vec4(1.f));

    auto s = Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material
    );

    root->addComponent(s);

    sceneManager->nextFrame(0.f, 0.f);
    for (const auto& sortPropertiesToDrawCalls : renderer->drawCallPool().drawCalls())
    {
        for (const auto& drawCalls : sortPropertiesToDrawCalls.second)
        {
            for (auto drawCall : drawCalls)
            {
                ASSERT_EQ(drawCall->pass(), basic->technique("default")[0]);
            }
        }
    }

    renderer->effect(phong);
    sceneManager->nextFrame(0.f, 0.f);
    for (const auto& sortPropertiesToDrawCalls : renderer->drawCallPool().drawCalls())
    {
        for (const auto& drawCalls : sortPropertiesToDrawCalls.second)
        {
            for (auto drawCall : drawCalls)
            {
                ASSERT_EQ(drawCall->pass(), phong->technique("default")[0]);
            }
        }
    }
}
