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

using namespace minko;
using namespace minko::component;

int main(int argc, char** argv)
{
    auto canvas = Canvas::create("My Minko App", 960, 540);
    auto sceneManager = SceneManager::create(canvas);
    auto assets = sceneManager->assets();
    auto defaultLoader = sceneManager->assets()->loader();
    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto fxLoader = file::Loader::create(defaultLoader)
        ->queue("effect/Phong.effect")
        ->queue("effect/Basic.effect");

    scene::Node::Ptr cube = nullptr;

    auto fxComplete = fxLoader->complete()->connect([&](file::Loader::Ptr loader)
    {
        defaultLoader->options()
            ->effect(assets->effect("effect/Phong.effect"));

        cube = scene::Node::create("cube")
            ->addComponent(Transform::create())
            ->addComponent(Surface::create(
                geometry::CubeGeometry::create(assets->context()),
                material::Material::create()->set({
                    { "diffuseColor", math::vec4(.5f, .5f, .5f, 1.f) }
                }),
                assets->effect("effect/Phong.effect")
            ));
        root->addChild(cube);

        auto camera = scene::Node::create("camera")
            ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()))
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(Transform::create(math::inverse(math::lookAt(
                math::vec3(2.f, 1.f, 2.f),
                math::vec3(0.f, 0.f, 0.f),
                math::vec3(0.f, 1.f, 0.f)
            ))));
        root->addChild(camera);

        auto lights = scene::Node::create("lights")
            ->addComponent(DirectionalLight::create())
            ->addComponent(AmbientLight::create())
            ->addComponent(Transform::create(math::inverse(math::lookAt(
                math::vec3(0.f, 2.f, 5.f),
                math::vec3(0.f, 0.f, 0.f),
                math::vec3(0.f, 1.f, 0.f)
            ))));
        root->addChild(lights);
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime)
    {
        if (cube)
            cube->component<Transform>()->matrix(
                cube->component<Transform>()->matrix()
                * math::rotate(.01f, math::vec3(0.f, 1.f, 0.f))
            );

        sceneManager->nextFrame(time, deltaTime);
    });

    fxLoader->load();
    canvas->run();

    return 0;
}
