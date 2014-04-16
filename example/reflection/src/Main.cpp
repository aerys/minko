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

void
UpdateVirtualCamera(scene::Node::Ptr camera, scene::Node::Ptr virtualCamera, float planeHeight)
{

}

int
main(int argc, char** argv)
{
    // FPS camera info
    auto cameraPosition = Vector3::create(0.0f, 0.0f, 0.0f);
    auto cameraDirection = Vector3::create(0.0f, 0.0f, 1.0f);
    auto cameraTarget = cameraDirection + cameraPosition;
    auto cameraSpeed = 100.0f;
    auto cameraPitch = 0.0f;
    auto cameraYaw = 0.0f;

    // Reflection plane info
    int planeHeight = 0;

    auto canvas = Canvas::create("Minko Example - Reflection", WINDOW_WIDTH, WINDOW_HEIGHT);
    auto sceneManager = component::SceneManager::create(canvas->context());

    // Mouse input
    auto mouse = canvas->mouse();
    mouse->setPosition((uint) (WINDOW_WIDTH / 2), (uint) (WINDOW_HEIGHT / 2));

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

        auto position = Vector3::create(0.f, 3.f, -5.f);
        auto reflectedPosition = Vector3::create(position->x(), -position->y(), position->z());
        auto cameraTarget = Vector3::create();

        // Render target
        auto ppTarget = render::Texture::create(
            sceneManager->assets()->context(), clp2(WINDOW_WIDTH), clp2(WINDOW_HEIGHT), false, true);

        auto reflectedCamera = scene::Node::create("reflectedCamera")
            ->addComponent(Renderer::create(0xff000000, ppTarget, sceneManager->assets()->effect("effect/Reflection/PlanarReflection.effect"), 10000))
            ->addComponent(PerspectiveCamera::create(
            (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, (float) PI * 0.25f, .1f, 1000.f))
            ->addComponent(Transform::create(Matrix4x4::create()
            ->lookAt(cameraTarget, reflectedPosition)));

        auto reflectedWorldToScreenMatrix = reflectedCamera->component<PerspectiveCamera>()
            ->data()->get<Matrix4x4::Ptr>("worldToScreenMatrix");

        auto reflectedModelToWorldMatrix = reflectedCamera->component<Transform>()->modelToWorldMatrix();

        auto reflectionComponent = Reflection::create(sceneManager->assets(), WINDOW_WIDTH, WINDOW_HEIGHT, 0xff000000);

        auto camera = scene::Node::create("camera")
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(reflectionComponent)
            ->addComponent(PerspectiveCamera::create(
            (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, (float) PI * 0.25f, .1f, 1000.f)
            )
            ->addComponent(Transform::create(Matrix4x4::create()
            ->lookAt(cameraTarget, position)))
            ;


        //root->addChild(reflectedCamera);
        root->addChild(camera);

        // create reflection effects
        auto reflectionEffect = sceneManager->assets()->effect("effect/Reflection/PlanarReflection.effect");
        auto applyReflectionEffect = sceneManager->assets()->effect("effect/Reflection/ApplyPlanarReflection.effect");

        if (!reflectionEffect)
            throw std::logic_error("The reflection effect has not been loaded.");

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
            //->addComponent(Reflection::create(sceneManager->assets()->context(), WINDOW_WIDTH, WINDOW_HEIGHT, 0xff000000))
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

        // Move the camera around the object
        Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
        float cameraRotationSpeedX = 0.f;
        float cameraRotationSpeedY = 0.f;

        auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr mouse)
        {
            mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr mouse, int dx, int dy)
            {
                cameraRotationSpeedX = (float) -dx * .01f;
                cameraRotationSpeedY = (float) -dy * .01f;
            });
        });

        auto mouseUp = canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr mouse)
        {
            mouseMove = nullptr;
        });

        auto ppRenderer = Renderer::create();
        auto ppScene = scene::Node::create()
            ->addComponent(Surface::create(
            geometry::QuadGeometry::create(sceneManager->assets()->context()),
            material::BasicMaterial::create()->diffuseMap(ppTarget),
            sceneManager->assets()->effect("effect/Basic.effect")
            ));

        auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint t, float dt)
        {
            auto reflectedProjectionMatrix = reflectedCamera->component<PerspectiveCamera>()
                ->data()->get<Matrix4x4::Ptr>("projectionMatrix");

            auto test = Matrix4x4::create(reflectedProjectionMatrix);

            camera->component<Transform>()->matrix()->appendRotationY(cameraRotationSpeedX);
            //reflectedCamera->component<Transform>()->matrix()->appendRotationY(cameraRotationSpeedX);

            camera->component<Transform>()->matrix()->appendRotationX(cameraRotationSpeedY);
            //reflectedCamera->component<Transform>()->matrix()->appendRotationX(-cameraRotationSpeedY);

            cameraRotationSpeedX *= .99f;
            cameraRotationSpeedY *= .99f;

            cube->component<Transform>()->matrix()->appendRotationY(0.01f);
            
            camera->component<Reflection>()->updateReflectionMatrix();

            // Get reflection map from camera's reflection component
            reflectionPlane->component<Surface>()->material()->set(
                "diffuseMap", camera->component<Reflection>()->getRenderTarget());


            //reflectionPlane->component<Surface>()->material()->set("diffuseMap", ppTarget);


            sceneManager->nextFrame(t, dt);

            std::cout << canvas->framerate() << std::endl;
        });

        canvas->run();
    });

    loader->load();

    return 0;
}
