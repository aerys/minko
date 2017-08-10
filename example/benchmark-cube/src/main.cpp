/*
Copyright (c) 2013 Aerys

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

using namespace minko;
using namespace minko::component;

void
createRandomCube(scene::Node::Ptr root, geometry::Geometry::Ptr geom, render::Effect::Ptr effect)
{
    auto node = scene::Node::create();
    auto r = math::sphericalRand(1.f);
    auto material = material::BasicMaterial::create();

    material->diffuseColor(math::vec4((r + 1.f) * .5f, 1.f));

    node->addComponent(Transform::create(
        math::translate(r * 50.f) * math::scale(math::vec3(.2f))
    ));
    node->addComponent(Surface::create(geom, material, effect));

    root->addChild(node);
}

int main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Benchmark Cube");
    auto sceneManager = SceneManager::create(canvas);
    auto root = scene::Node::create("root")->addComponent(sceneManager);

    sceneManager->assets()->loader()->queue("effect/Basic.effect");
    sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));

    auto mesh = scene::Node::create("mesh");

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
            math::inverse(math::lookAt(math::vec3(0.f, 0.f, 150.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f)))
        ))
        ->addComponent(Camera::create(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f)));

    auto meshes = scene::Node::create();

    root->addChild(camera);
    root->addChild(meshes);

    auto numFrames = 0;
    auto t = 0;
    auto p = 0;
    auto ready = false;

    auto loaderComplete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
        ready = true;
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime)
    {
        camera->component<Transform>()->matrix(
            math::rotate(0.01f, math::vec3(0.f, 1.f, 0.f))
            * camera->component<Transform>()->matrix()
        );

        if (!ready)
            return;

        if (canvas->framerate() > 30.f)
            t++;
        else
            p++;

        if (t > 10)
        {
            t = 0;
            p = 0;

            for (auto i = 0; i < 100; ++i)
                createRandomCube(
                    meshes,
                    sceneManager->assets()->geometry("cube"),
                    sceneManager->assets()->effect("effect/Basic.effect")
                );
        }

        if (p > 10 && meshes->children().size() > 0)
        {
            t = 0;
            p = 0;
            meshes->removeChild(meshes->children().back());
        }

        if (++numFrames % 100 == 0)
            std::cout << "num meshes = " << meshes->children().size()
                << ", num draw calls = " << camera->component<Renderer>()->numDrawCalls()
                << ", framerate = " << canvas->framerate() << std::endl;

        sceneManager->nextFrame(time, deltaTime);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<Camera>()->projectionMatrix(math::perspective(.785f, float(w) / float(h), 0.1f, 1000.f));
    });

    sceneManager->assets()->loader()->load();
    canvas->run();

    return 0;
}