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
	sceneManager->assets()->loader()->options()
        ->registerParser<file::OBJParser>("obj")
        ->registerParser<file::ColladaParser>("dae")
        ->registerParser<file::JPEGParser>("jpg");

	sceneManager->assets()->loader()
		->queue("effect/Basic.effect")
		->queue("effect/Phong.effect");

    // add the model to the asset list
	sceneManager->assets()->loader()->queue(OBJ_MODEL_FILENAME);
	sceneManager->assets()->loader()->queue(DAE_MODEL_FILENAME);

	sceneManager->assets()->loader()->options()->generateMipmaps(true);
	sceneManager->assets()->loader()->options()->effect(sceneManager->assets()->effect("effect/Basic.effect"));

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
        auto root = scene::Node::create("root")->addComponent(sceneManager);

        auto camera = scene::Node::create("camera")
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(Transform::create(
            Matrix4x4::create()->lookAt(Vector3::create(0.f, 0.f, 0.f), Vector3::create(0.f, 0.f, 5.f))
            ))
            ->addComponent(PerspectiveCamera::create(
            (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, float(M_PI) * 0.25f, .1f, 1000.f)
            );
        root->addChild(camera);

		auto objModel = sceneManager->assets()->symbol(OBJ_MODEL_FILENAME);
		auto daeModel = sceneManager->assets()->symbol(DAE_MODEL_FILENAME);

        // change scale for the obj file
        objModel->component<Transform>()->matrix()->appendScale(0.01f);

        // change position
        objModel->component<Transform>()->matrix()->translation(-1.f, -1.f, 0.f);
        daeModel->component<Transform>()->matrix()->translation(1.f, -1.f, 0.f);

        // add to the scene
        root->addChild(objModel);
        root->addChild(daeModel);

        auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
        {
            sceneManager->nextFrame(t, dt);
        });

        canvas->run();
    });

    sceneManager->assets()->loader()->load();

    return 0;
}
