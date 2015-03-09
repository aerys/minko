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

scene::Node::Ptr
createRandomCube(geometry::Geometry::Ptr geom, render::Effect::Ptr effect)
{
    auto r = math::sphericalRand(1.f);

    auto material = material::BasicMaterial::create()
        ->diffuseColor(math::vec4((r + 1.f) * .5f, 1.f));
        // ->diffuseColor(math::vec4(0.f, 0.f, 0.f, 1.f));

    auto node = scene::Node::create()
        ->addComponent(Transform::create(
            math::translate(r * 50.f) * math::scale(math::vec3(10.f))
        ))
        ->addComponent(Surface::create(
            geom,
            material,
            effect
        ));

    return node;
}

int main(int argc, char** argv)
{
    auto canvas         = Canvas::create("Minko Example - Light Scattering", 800, 600);
    auto sceneManager   = SceneManager::create(canvas);
    auto root           = scene::Node::create("root")->addComponent(sceneManager);
    auto assets         = sceneManager->assets();
    auto context        = canvas->context();

    context->errorsEnabled(true);

    // setup assets
    assets->loader()
        ->queue("effect/LightScattering/EmissionMap.effect")
        ->queue("effect/LightScattering/LightScattering.effect")
        ->queue("effect/Basic.effect");

    assets->geometry("cube", geometry::CubeGeometry::create(context));

    // standard
    auto renderer = Renderer::create();
    renderer->layoutMask(renderer->layoutMask() & ~scene::BuiltinLayout::DEBUG_ONLY);
    renderer->backgroundColor(0x23097aff);

    // forward
    auto fwdRenderer = Renderer::create();
    auto fwdTarget = render::Texture::create(context, math::clp2(canvas->width()), math::clp2(canvas->height()), false, true);
    fwdTarget->upload();

    // post-processing
    auto ppRenderer = Renderer::create();
    auto ppScene = scene::Node::create()->addComponent(ppRenderer);
    auto ppTarget = render::Texture::create(context, math::clp2(canvas->width()), math::clp2(canvas->height()), false, true);
    auto ppMaterial = material::BasicMaterial::create();

    ppMaterial->data()->set("lightbuffer", fwdTarget->sampler());
    ppMaterial->data()->set("backbuffer", ppTarget->sampler());
    ppTarget->upload();

    // scene
    auto debugNode1 = scene::Node::create("debug1", scene::BuiltinLayout::DEBUG_ONLY);
    auto debugNode2 = scene::Node::create("debug2", scene::BuiltinLayout::DEBUG_ONLY);

    auto camera = scene::Node::create("camera")
        ->addComponent(Transform::create(
            math::inverse(math::lookAt(math::vec3(0.f), math::vec3(0.f, 0.f, 1.f), math::vec3(0.f, 1.f, 0.f)))
        ))
        ->addComponent(PerspectiveCamera::create(800.f / 600.f, float(M_PI) * 0.25f, .1f, 1000.f))
        ->addComponent(renderer);
    root->addChild(camera);

    auto helio = scene::Node::create("helio")
        ->addComponent(Transform::create());
    root->addChild(helio);

    auto sun = scene::Node::create("sun");

    auto _ = assets->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
        for (auto i = 0; i < 100; ++i)
            root->addChild(createRandomCube(
                assets->geometry("cube"),
                assets->effect("effect/Basic.effect")
            ));

        auto sunMaterial = material::BasicMaterial::create({
            { "diffuseColor", math::vec4(1.f, 0.32f, 0.05f, 1.f) },
            { "isLightSource", true }
        });

        helio
            ->addChild(scene::Node::create()
                ->addComponent(Transform::create(
                    math::translate(math::mat4(1.f), math::vec3(0.f, 0.f, 100.f))
                ))
                ->addChild(sun
                    ->addComponent(Transform::create(
                        math::scale(math::mat4(1.f), math::vec3(10.f))
                    ))
                    ->addComponent(Surface::create(
                        geometry::SphereGeometry::create(context),
                        sunMaterial,
                        assets->effect("effect/Basic.effect")
                    ))
                )
            );

        ppScene->addComponent(Surface::create(
            geometry::QuadGeometry::create(context),
            ppMaterial,
            assets->effect("effect/LightScattering/LightScattering.effect")
        ));

        // forward
        fwdRenderer = Renderer::create(
            0x000000ff,
            fwdTarget,
            assets->effect("effect/LightScattering/EmissionMap.effect")
        );
        fwdRenderer->layoutMask(fwdRenderer->layoutMask() & ~scene::BuiltinLayout::DEBUG_ONLY);
        camera->addComponent(fwdRenderer);

        auto debugDisplay1 = TextureDebugDisplay::create();
        debugDisplay1->initialize(assets, fwdTarget);
        debugNode1->addComponent(debugDisplay1);
        ppScene->addChild(debugNode1);

        auto debugDisplay2 = TextureDebugDisplay::create();
        debugDisplay2->initialize(assets, ppTarget);
        debugDisplay2->material()->data()->set("spritePosition", math::vec2(10, 440));
        debugNode2->addComponent(debugDisplay2);
        ppScene->addChild(debugNode2);
    });

    auto resized = canvas->resized()->connect([=](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    auto enterFrame = canvas->enterFrame()->connect([=](Canvas::Ptr canvas, float time, float deltaTime)
    {
        camera->component<Transform>()->matrix(
            math::rotate(0.001f, math::vec3(0.f, 1.f, 0.f))
            * camera->component<Transform>()->matrix()
        );

        helio->component<Transform>()->matrix(
            math::rotate(0.001f, math::vec3(0.f, 1.f, 0.f))
            * helio->component<Transform>()->matrix()
        );

        math::vec3 worldSpaceLightPosition = math::vec3(sun->component<Transform>()->modelToWorldMatrix()[3]);
        math::vec3 screenSpaceLightPosition = camera->component<PerspectiveCamera>()->project(worldSpaceLightPosition);
        screenSpaceLightPosition = math::vec3(screenSpaceLightPosition.x / canvas->width(), screenSpaceLightPosition.y / canvas->height(), 1.f);
        // std::cout << glm::to_string(screenSpaceLightPosition) << std::endl;

        ppMaterial->data()->set("screenSpaceLightPosition", screenSpaceLightPosition.xy());

        // Rendering in "black and white" to fwdTarget.
        fwdRenderer->render(context);

        // Rendering the scene normally to ppTarget.
        sceneManager->nextFrame(time, deltaTime, ppTarget);

        // Blending fwdTarget with ppTarget, enabling light scattering.
        ppRenderer->render(context);
    });

    assets->loader()->load();
    canvas->run();

    return 0;
}
