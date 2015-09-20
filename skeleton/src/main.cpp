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
    auto canvas = Canvas::create("Minko Application", 800, 600);

    auto sceneManager = SceneManager::create(canvas);

    sceneManager->assets()->loader()
        ->queue("effect/Basic.effect");

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
          math::inverse(
            math::lookAt(math::vec3(0.f, 0.f, 3.f), math::vec3(), math::vec3(0, 1, 0))
	  )
        ))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

    root->addChild(camera);

    auto mesh = scene::Node::create("mesh")
        ->addComponent(Transform::create());

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        mesh->addComponent(Surface::create(
            geometry::CubeGeometry::create(sceneManager->assets()->context()),
            material::Material::create(),
            sceneManager->assets()->effect("effect/Basic.effect")
        ));

        mesh->component<Surface>()->material()->data()->set("diffuseColor", math::vec4(1.f,1.f,1.f,1.f));
        root->addChild(mesh);

        auto light = scene::Node::create("light")
            ->addComponent(AmbientLight::create())
            ->addComponent(DirectionalLight::create())
            ->addComponent(Transform::create(
                math::lookAt(math::vec3(-2.f, -1.f, -1.f), math::vec3(), math::vec3(0, 1, 0))
            ));

        root->addChild(light);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
	mesh->component<Transform>()->matrix(
        	mesh->component<Transform>()->matrix() * math::rotate(0.001f * deltaTime, math::vec3(0,1,0))
	);
        sceneManager->nextFrame(time, deltaTime);
    });

    sceneManager->assets()->loader()->load();
    canvas->run();

    return 0;
}
