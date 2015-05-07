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
#include "minko/log/Logger.hpp"
#include "minko/sensors/Attitude.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::sensors;

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Sensors");
    auto sceneManager = SceneManager::create(canvas);

    auto attitude = Attitude::create();
    attitude->initialize();
    attitude->startTracking();

    sceneManager->assets()->loader()->queue("effect/Basic.effect");

    auto cubeGeometry = geometry::CubeGeometry::create(sceneManager->assets()->context());
    auto quadGeometry = geometry::QuadGeometry::create(sceneManager->assets()->context());
    sceneManager->assets()->geometry("cubeGeometry", cubeGeometry);
    sceneManager->assets()->geometry("quadGeometry", quadGeometry);

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto greenCube = scene::Node::create("greenCube")
        ->addComponent(Transform::create(math::translate(math::vec3(0.f, 0.f, -2.5f))));

    auto blueCube = scene::Node::create("blueCube")
        ->addComponent(Transform::create(math::translate(math::vec3(-2.5f, 0.f, 0.f))));

    auto pinkCube = scene::Node::create("pinkCube")
        ->addComponent(Transform::create(math::translate(math::vec3(2.5f, 0.f, 0.f))));

    auto groundTransform = Transform::create();
    groundTransform->matrix(math::scale(math::vec3(10.f, 10.f, 10.f)) * groundTransform->matrix());
    groundTransform->matrix(math::rotate((float)(-M_PI_2), math::vec3(1.f, 0.f, 0.f)) * groundTransform->matrix());
    groundTransform->matrix(math::translate(math::vec3(0.f, -1.f, 0.f)) * groundTransform->matrix());

    auto ground = scene::Node::create("ground")
        ->addComponent(groundTransform);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(
            Transform::create(
                math::inverse(
                    math::lookAt(
                        math::vec3(0.f, 0.f, 3.f), math::vec3(), math::vec3(0.f, 1.f, 0.f)
                    )
                )
            )
        )
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

    root->addChild(greenCube);
    root->addChild(blueCube);
    root->addChild(pinkCube);
    root->addChild(ground);
    root->addChild(camera);

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        auto greenMaterial = material::BasicMaterial::create();
        greenMaterial->diffuseColor(math::vec4(0.f, 1.f, 0.f, 1.f));
        auto blueMaterial = material::BasicMaterial::create();
        blueMaterial->diffuseColor(math::vec4(0.f, 0.f, 1.f, 1.f));
        auto pinkMaterial = material::BasicMaterial::create();
        pinkMaterial->diffuseColor(math::vec4(1.f, 0.f, 1.f, 1.f));
        auto groundMaterial = material::BasicMaterial::create();
        groundMaterial->diffuseColor(math::vec4(1.f, 0.f, 0.f, 1.f));

        greenCube->addComponent(Surface::create(
            sceneManager->assets()->geometry("cubeGeometry"),
            greenMaterial,
            sceneManager->assets()->effect("effect/Basic.effect")
        ));

        blueCube->addComponent(Surface::create(
            sceneManager->assets()->geometry("cubeGeometry"),
            blueMaterial,
            sceneManager->assets()->effect("effect/Basic.effect")
        ));

        pinkCube->addComponent(Surface::create(
            sceneManager->assets()->geometry("cubeGeometry"),
            pinkMaterial,
            sceneManager->assets()->effect("effect/Basic.effect")
        ));

        ground->addComponent(Surface::create(
            sceneManager->assets()->geometry("quadGeometry"),
            groundMaterial,
            sceneManager->assets()->effect("effect/Basic.effect")
        ));
    });

    auto touchDown = false;
    auto touchDownSlot = canvas->touch()->touchDown()->connect([&](input::Touch::Ptr t, int, float x, float y)
    {
        touchDown = true;
    });
    
    auto touchUpSlot = canvas->touch()->touchUp()->connect([&](input::Touch::Ptr t, int, float x, float y)
    {
        touchDown = false;
    });
    
    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });
    
    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
        greenCube->component<Transform>()->matrix(
            greenCube->component<Transform>()->matrix() * math::rotate(0.01f, math::vec3(0, 1, 0))
        );

        // Rebuild camera matrix with rotation matrix retrieve from sensors
        auto cameraPosition = camera->component<Transform>()->matrix()[3].xyz();
        auto rotationMatrix = math::transpose(attitude->rotationMatrix());
        auto direction = math::vec3(-rotationMatrix[2][0], -rotationMatrix[2][1], -rotationMatrix[2][2]);
        auto newMatrix = math::translate(cameraPosition) * rotationMatrix;

        camera->component<Transform>()->matrix(newMatrix);

        if (touchDown)
            camera->component<Transform>()->matrix(math::translate(direction * 0.01f) * camera->component<Transform>()->matrix());
        
        sceneManager->nextFrame(time, deltaTime);
    });
    
    sceneManager->assets()->loader()->load();
    canvas->run();

    return 0;
}
