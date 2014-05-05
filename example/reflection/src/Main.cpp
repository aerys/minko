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
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoFX.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;

int
main(int argc, char** argv)
{
    // Camera info
    auto cameraPosition = Vector3::create(0.f, 3.f, -5.f);
    auto cameraDirection = Vector3::create(0.0f, 0.0f, 1.0f);
    auto cameraTarget = Vector3::create()/*cameraDirection + cameraPosition*/;

    auto yaw = 0.f;
    auto pitch = (float) PI * .5f;
    auto minPitch = 0.f + 1e-5;
    auto maxPitch = (float) PI - 1e-5;
    auto lookAt = Vector3::create(0.f, 0.f, 0.f);
    auto distance = 10.f;

    // Reflection plane info
    int planeHeight = 0;

    auto canvas = Canvas::create("Minko Example - Reflection", WINDOW_WIDTH, WINDOW_HEIGHT);
    auto sceneManager = component::SceneManager::create(canvas->context());

    auto loader = sceneManager->assets()->loader();

    loader
        ->queue("effect/Basic.effect")
        ->queue("effect/Reflection/PlanarReflection.effect")
        ->queue("effect/Reflection/ApplyPlanarReflection.effect")
        ->queue("texture/box.png")
        ;

    loader->options()
        ->registerParser<file::PNGParser>("png")
        ;

    auto complete = loader->complete()->connect([&](file::Loader::Ptr loader)
    {
        auto root = scene::Node::create("root")
            ->addComponent(sceneManager);

        auto reflectionComponent = Reflection::create(sceneManager->assets(), WINDOW_WIDTH, WINDOW_HEIGHT, 0xff000000);

        auto camera = scene::Node::create("camera")
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(reflectionComponent)
            ->addComponent(PerspectiveCamera::create(
            (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, (float) PI * 0.25f, .1f, 1000.f)
            )
            ->addComponent(Transform::create(Matrix4x4::create()
            ->lookAt(cameraTarget, cameraPosition)))
            ;

        //root->addChild(reflectedCamera);
        root->addChild(camera);

        // create reflection effects
        auto applyReflectionEffect = sceneManager->assets()->effect("effect/Reflection/ApplyPlanarReflection.effect");

        if (!applyReflectionEffect)
            throw std::logic_error("The apply reflection effect has not been loaded.");

        auto cube = scene::Node::create("cube")
            ->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(0.f, 1.f, 0.f)))
            ->addComponent(Surface::create(
            geometry::CubeGeometry::create(sceneManager->assets()->context()),
            material::BasicMaterial::create()->diffuseMap(sceneManager->assets()->texture("texture/box.png")),
            sceneManager->assets()->effect("effect/Basic.effect")
            ));
        root->addChild(cube);

        auto sphere = scene::Node::create("sphere")
            ->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(-1.f, -0.25f, 0.f)))
            ->addComponent(Surface::create(
            geometry::SphereGeometry::create(sceneManager->assets()->context()),
            material::BasicMaterial::create()->diffuseColor(Vector4::create(1.f, 0.f, 0.f, 1.f)),
            sceneManager->assets()->effect("effect/Basic.effect")
            ));
        root->addChild(sphere);

        auto reflectionPlane = scene::Node::create("reflectionPlane")
            ->addComponent(Transform::create(Matrix4x4::create()
            ->appendScale(5.f)
            ->appendRotationX(-PI / 2.f)
            ->appendTranslation(0.f, planeHeight, 0.f)))
            ->addComponent(Surface::create(
                geometry::QuadGeometry::create(sceneManager->assets()->context()),
                    material::Material::create()->set("diffuseMap", sceneManager->assets()->texture("texture/box.png")),
                    applyReflectionEffect
                )
            );
        root->addChild(reflectionPlane);

        // handle mouse signals
        auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
        {
            distance += (float) v / 10.f;
        });

        Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
        auto cameraRotationXSpeed = 0.f;
        auto cameraRotationYSpeed = 0.f;

        auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
        {
            mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr, int dx, int dy)
            {
                cameraRotationYSpeed = (float) dx * .01f;
                cameraRotationXSpeed = (float) dy * -.01f;
            });
        });

        auto mouseUp = canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr m)
        {
            mouseMove = nullptr;
        });

        // Get reflection map from camera's reflection component
        reflectionPlane->component<Surface>()->material()->set(
            "diffuseMap", camera->component<Reflection>()->getRenderTarget());

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
                lookAt->x() + distance * cosf(yaw) * sinf(pitch),
                lookAt->y() + distance * cosf(pitch),
                lookAt->z() + distance * sinf(yaw) * sinf(pitch)
                )
                );

            cube->component<Transform>()->matrix()->appendRotationY(0.01f);

            sceneManager->nextFrame(time, deltaTime);

            std::cout << canvas->framerate() << std::endl;
        });

        canvas->run();
    });

    loader->load();

    return 0;
}
