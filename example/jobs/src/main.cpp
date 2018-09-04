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
#include "minko/MinkoSDL.hpp"

#include "minko/component/CountJob.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const std::string TEXTURE_FILENAME = "texture/box.png";

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Jobs");

    auto sceneManager = SceneManager::create(canvas);

    // setup assets
    sceneManager->assets()->loader()->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png");

    sceneManager->assets()->loader()
        ->queue(TEXTURE_FILENAME)
        ->queue("effect/Basic.effect");

    sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));

    auto taskManager = JobManager::create(60);

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager)
        ->addComponent(taskManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
            math::inverse(
                math::lookAt(
                    math::vec3(0.f, 0.f, 3.f), math::vec3(), math::vec3(0, 1, 0)
                )
            )
        ))
        ->addComponent(Camera::create(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f)));

    root->addChild(camera);

    auto mesh = scene::Node::create("mesh")
        ->addComponent(Transform::create());

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        auto cubeGeometry    = geometry::CubeGeometry::create(sceneManager->assets()->context());

        for (uint i = 0; i < 1000; ++i)
            taskManager->pushJob(CountJob::create());

        sceneManager->assets()->geometry("cubeGeometry", cubeGeometry);

        mesh->addComponent(Surface::create(
            sceneManager->assets()->geometry("cubeGeometry"),
            material::BasicMaterial::create()->diffuseMap(sceneManager->assets()->texture(TEXTURE_FILENAME)),
            sceneManager->assets()->effect("effect/Basic.effect")
        ));

        root->addChild(mesh);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<Camera>()->projectionMatrix(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime, bool shouldRender)
    {
        mesh->component<Transform>()->matrix(
            mesh->component<Transform>()->matrix() * math::rotate(0.02f, math::vec3(0, 1, 0))
        );

        sceneManager->nextFrame(time, deltaTime, shouldRender);
    });

    sceneManager->assets()->loader()->load();
    canvas->run();
}


