/*
Copyright (c) 2015 Aerys

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

#include "minko/Minko.hpp"
#include "minko/MinkoSDL.hpp"

#include "TextureDebugDisplay.hpp"

using namespace minko;
using namespace minko::component;

void
createRandomCube(scene::Node::Ptr root, geometry::Geometry::Ptr geom, render::Effect::Ptr effect)
{
    auto node = scene::Node::create("cube", scene::BuiltinLayout::DEFAULT | 256);
    auto r = math::sphericalRand(1.f);
    auto material = material::BasicMaterial::create();

    material->diffuseColor(math::vec4((r + 1.f) * .5f, 1.f));

    node->addComponent(Transform::create(
        math::translate(r * 50.f) * math::scale(math::vec3(10.f))
    ));
    node->addComponent(Surface::create(geom, material, effect));

    root->addChild(node);
}

int main(int argc, char** argv)
{
    auto canvas         = Canvas::create("Minko Example - God's Ray", 800, 600);
    auto sceneManager   = SceneManager::create(canvas);
    auto root           = scene::Node::create("root")->addComponent(sceneManager);
    auto assets         = sceneManager->assets();
    auto context        = canvas->context();

    context->errorsEnabled(true);

    // setup assets
    assets->loader()
        ->queue("effect/LightScattering/LightScattering.effect")
        ->queue("effect/Basic.effect");

    assets->geometry("cube", geometry::CubeGeometry::create(context));

    auto renderer = Renderer::create();
    renderer->layoutMask(renderer->layoutMask() & ~scene::BuiltinLayout::DEBUG_ONLY);

    auto camera = scene::Node::create("camera")
        ->addComponent(renderer)
        ->addComponent(Transform::create(
            math::inverse(math::lookAt(math::vec3(0.f), math::vec3(0.f, 0.f, 1.f), math::vec3(0.f, 1.f, 0.f)))
        ))
        ->addComponent(PerspectiveCamera::create(800.f / 600.f, float(M_PI) * 0.25f, .1f, 1000.f));
    root->addChild(camera);

    auto meshes = scene::Node::create();
    root->addChild(meshes);

    // post-processing
    auto ppRenderer = Renderer::create();
    auto ppScene = scene::Node::create()->addComponent(ppRenderer);
    auto ppTarget = render::Texture::create(context, math::clp2(canvas->width()), math::clp2(canvas->height()), false, true);
    auto ppMaterial = material::BasicMaterial::create();

    ppMaterial->data()->set("backbuffer", ppTarget->sampler());
    ppTarget->upload();

    auto debugDisplay1 = TextureDebugDisplay::create();
    auto debugNode1 = scene::Node::create("debug", scene::BuiltinLayout::DEBUG_ONLY);
    debugDisplay1->initialize(assets, ppTarget);
    debugNode1->addComponent(debugDisplay1);

    auto _ = assets->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        for (auto i = 0; i < 100; ++i)
            createRandomCube(
                meshes,
                assets->geometry("cube"),
                assets->effect("effect/Basic.effect")
            );

        ppScene->addComponent(Surface::create(
            geometry::QuadGeometry::create(context),
            ppMaterial,
            assets->effect("effect/LightScattering/LightScattering.effect")
        ));

        ppScene->addChild(debugNode1);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
        camera->component<Transform>()->matrix(
            math::rotate(0.01f, math::vec3(0.f, 1.f, 0.f))
            * camera->component<Transform>()->matrix()
        );

        sceneManager->nextFrame(time, deltaTime, ppTarget);
        ppScene->component<Renderer>()->render(context);
    });

    assets->loader()->load();
    canvas->run();

    return 0;
}
