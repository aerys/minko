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

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

#define WINDOW_WIDTH      800
#define WINDOW_HEIGHT     600

int
main(int argc, char** argv)
{
    auto canvas         = Canvas::create("Minko Example - Flares");
    auto context        = canvas->context();
    auto sceneManager   = SceneManager::create(canvas->context());
    auto assets         = sceneManager->assets();
    auto defaultLoader  = assets->loader();
    auto root           = scene::Node::create("root")->addComponent(sceneManager);

    defaultLoader->options()
        ->generateMipmaps(true)
        ->resizeSmoothly(true)
        ->registerParser<file::PNGParser>("png")
        ->registerParser<file::JPEGParser>("jpg");

    defaultLoader
        ->queue("texture/skybox.jpg", file::Options::create(defaultLoader->options())->isCubeTexture(true))
        ->queue("texture/sprite-pointlight.png")
        ->queue("effect/PseudoLensFlare/PseudoLensFlare.effect")
        ->queue("effect/Basic.effect")
        ->queue("effect/Sprite.effect")
        ->queue("effect/Phong.effect");

    // post-processing
    auto ppScene = scene::Node::create()->addComponent(Renderer::create());
    auto ppTarget = render::Texture::create(context, math::clp2(WINDOW_WIDTH), math::clp2(WINDOW_HEIGHT), false, true);
    ppTarget->upload();

    auto _ = defaultLoader->complete()->connect([=](file::Loader::Ptr loader)
    {
        ppScene->addComponent(Surface::create(
            geometry::QuadGeometry::create(context),
            data::Provider::create()->set("backbuffer", ppTarget),
            assets->effect("effect/PseudoLensFlare/PseudoLensFlare.effect")
        ));

        // sky
        auto sky = scene::Node::create("sky")
            ->addComponent(Transform::create(
                Matrix4x4::create()->appendScale(500.f)
            ))
            ->addComponent(Surface::create(
                geometry::CubeGeometry::create(context),
                material::BasicMaterial::create()
                    ->diffuseCubeMap(assets->cubeTexture("texture/skybox.jpg"))
                    ->triangleCulling(render::TriangleCulling::FRONT),
                assets->effect("effect/Basic.effect")
            ));

        root->addChild(sky);

        // sprite
        auto sprite = scene::Node::create("sprite")
            ->addComponent(Surface::create(
                geometry::QuadGeometry::create(context),
                material::BasicMaterial::create()->diffuseMap(assets->texture("texture/sprite-pointlight.png")),
                assets->effect("effect/Sprite.effect")
            ))
            ->addComponent(Transform::create(
                Matrix4x4::create()->appendTranslation(100.f, 100.f, -150.f)->prependScale(100.f)
            ));

        root->addChild(sprite);
    });

    // camera init
    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create())
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()))
        ->addComponent(Transform::create());

    root->addChild(camera);

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, unsigned int w, unsigned int h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    auto yaw = -4.03f;
    auto pitch = 2.05f;
    auto minPitch = 0.f + 1e-5;
    auto maxPitch = float(M_PI) - 1e-5;
    auto lookAt = Vector3::create(0.f, 0.f, 0.f);
    auto distance = 15.f;

    // handle mouse signals
    auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
    {
        distance += (float)v / 10.f;
    });

    Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
    auto cameraRotationXSpeed = 0.f;
    auto cameraRotationYSpeed = 0.f;

    auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
    {
        mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr, int dx, int dy)
        {
            cameraRotationYSpeed = float(dx) * .0025f;
            cameraRotationXSpeed = float(dy) * -.0025f;
        });
    });

    auto mouseUp = canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr m)
    {
        mouseMove = nullptr;
    });

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
        yaw += cameraRotationYSpeed;
        cameraRotationYSpeed *= 0.9f;

        pitch += cameraRotationXSpeed;
        cameraRotationXSpeed *= 0.9f;

        if (pitch > maxPitch)
            pitch = maxPitch;
        else if (pitch < minPitch)
            pitch = minPitch;

        camera->component<Transform>()->matrix()->lookAt(
            lookAt,
            Vector3::create(
                lookAt->x() + distance * std::cosf(yaw) * sinf(pitch),
                lookAt->y() + distance * std::cosf(pitch),
                lookAt->z() + distance * std::sinf(yaw) * sinf(pitch)
            )
        );

        sceneManager->nextFrame(time, deltaTime, ppTarget);
        ppScene->component<Renderer>()->render(context);
    });

    assets->loader()->load();

    canvas->run();
}
