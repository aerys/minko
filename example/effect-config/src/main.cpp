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
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
//using namespace minko::math;
using namespace minko::render;

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Effect Config");
    auto sceneManager = SceneManager::create(canvas);

    // setup assets
    sceneManager->assets()->loader()->options()
        ->generateMipmaps(true);

    sceneManager->assets()->loader()->options()
        ->registerParser<file::JPEGParser>("jpg");

    sceneManager->assets()
        ->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
        ->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context()));

    sceneManager->assets()->loader()
        ->queue("effect/windows.jpg")
        ->queue("effect/osx.jpg")
        ->queue("effect/linux.jpg")
        ->queue("effect/Basic.effect")
        ->queue("effect/PlatformTexture.effect");

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

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
        /* mesh->addComponent(Surface::create(
            sceneManager->assets()->geometry("cube"),
            material::Material::create()->set("diffuseColor", Vector4::one()),
            sceneManager->assets()->effect("effect/PlatformTexture.effect")
        )); */
        
        //this might be wrong
        auto meshMaterial = material::BasicMaterial::create();
        meshMaterial->diffuseColor(0xff0000ff);
        
        mesh->addComponent(Surface::create(
            sceneManager->assets()->geometry("cubeGeometry"),
            meshMaterial,
            sceneManager->assets()->effect("effect/PlatformTexture.effect")
        ));
        
        root->addChild(mesh);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        root->children()[0]->component<Camera>()->projectionMatrix(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime, bool shouldRender)
    {
        mesh->component<Transform>()->matrix(
            mesh->component<Transform>()->matrix() * math::rotate(0.01f, math::vec3(0, 1, 0))
        );
        sceneManager->nextFrame(time, deltaTime, shouldRender);
    });

    sceneManager->assets()->loader()->load();

    canvas->run();

    return 0;
}
