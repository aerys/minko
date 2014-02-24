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
#include "minko/MinkoASSIMP.hpp"
#include "minko/MinkoJPEG.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;

const std::string OBJ_MODEL_FILENAME = "model/pirate.obj";
const std::string DAE_MODEL_FILENAME = "model/pirate.dae";

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Tutorial - Load 3D files", WINDOW_WIDTH, WINDOW_HEIGHT);
    auto sceneManager = SceneManager::create(canvas->context());

    // setup assets
    sceneManager->assets()
        ->registerParser<file::ASSIMPParser>("obj")
        ->registerParser<file::ASSIMPParser>("dae")
        ->registerParser<file::JPEGParser>("jpg");

    sceneManager->assets()->load("effect/Basic.effect");
    
    // add the model to the asset list
    sceneManager->assets()->queue(OBJ_MODEL_FILENAME);
    sceneManager->assets()->queue(DAE_MODEL_FILENAME);

    sceneManager->assets()->defaultOptions()->generateMipmaps(true);
    sceneManager->assets()->defaultOptions()->effect(sceneManager->assets()->effect("effect/Basic.effect"));

    auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
    {
        auto root = scene::Node::create("root")->addComponent(sceneManager);

        auto camera = scene::Node::create("camera")
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(Transform::create(
            Matrix4x4::create()->lookAt(Vector3::create(0.f, 0.f, 0.f), Vector3::create(0.f, 0.f, 5.f))
            ))
            ->addComponent(PerspectiveCamera::create(
            (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, (float) PI * 0.25f, .1f, 1000.f)
            );
        root->addChild(camera);

        auto objModel = assets->symbol(OBJ_MODEL_FILENAME);
        auto daeModel = assets->symbol(DAE_MODEL_FILENAME);

        objModel->addComponent(Transform::create(Matrix4x4::create()->appendScale(.01f)));
        daeModel->addComponent(Transform::create(Matrix4x4::create()->appendScale(.01f)));

        objModel->component<Transform>()->matrix()->translation(-1.f, 0.f, 0.f);
        daeModel->component<Transform>()->matrix()->translation(1.f, 0.f, 0.f);

        root->addChild(objModel);
        root->addChild(daeModel);

        Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
        float cameraRotationSpeed = 0.f;

        auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr mouse)
        {
            mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr mouse, int dx, int dy)
            {
                cameraRotationSpeed = (float) -dx * .01f;
            });
        });

        auto mouseUp = canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr mouse)
        {
            mouseMove = nullptr;
        });

        auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint t, float dt)
        {
            camera->component<Transform>()->matrix()->appendRotationY(cameraRotationSpeed);
            cameraRotationSpeed *= .99f;

            sceneManager->nextFrame();
        });

        canvas->run();
    });

    sceneManager->assets()->load();

    return 0;
}