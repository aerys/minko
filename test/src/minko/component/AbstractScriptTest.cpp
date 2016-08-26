/*
Copyright (c) 2016 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR auto renderer = Renderer::create();
auto root = scene::Node::create()
    ->addComponent(SceneManager::create(MinkoTests::canvas()))
    ->addComponent(PerspectiveCamera::create(1.f))
    ->addComponent(renderer);PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "AbstractScriptTest.hpp"

using namespace minko;
using namespace minko::component;

void
AbstractScriptTest::TestSurfaceScript::start(scene::Node::Ptr target)
{
    AbstractScript::start(target);

    target->addComponent(Surface::create(
        geometry::CubeGeometry::create(MinkoTests::canvas()->context()),
        material::BasicMaterial::create()->set({
            { "diffuseColor",   math::vec4(1.f)}
        }),
        MinkoTests::loadEffect("effect/Basic.effect")
    ));
}

void
AbstractScriptTest::TestSurfaceScript::stop(scene::Node::Ptr target)
{
    AbstractScript::stop(target);

    target->removeComponent(target->component<Surface>());
}

TEST_F(AbstractScriptTest, RemoveChildImpliesScriptStopThatRemovesSurface)
{
    auto root = scene::Node::create()
        ->addComponent(SceneManager::create(MinkoTests::canvas()))
        ->addComponent(PerspectiveCamera::create(1.f))
        ->addComponent(Renderer::create());

    auto node = scene::Node::create()
        ->addComponent(std::make_shared<TestSurfaceScript>());

    root->addChild(node);

    root->component<SceneManager>()->nextFrame(0.f, 0.f);

    ASSERT_EQ(root->component<Renderer>()->numDrawCalls(), 1);

    root->removeChild(node);

    root->component<SceneManager>()->nextFrame(0.f, 0.f);

    ASSERT_EQ(root->component<Renderer>()->numDrawCalls(), 0);
}
