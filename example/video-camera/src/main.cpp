/*
Copyright (c) 2015 Aerys

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
#include "minko/MinkoVideoCamera.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;
using namespace minko::video;

int
main(int argc, char** argv)
{
    auto videoCamera = AbstractVideoCamera::Ptr();
    
    auto canvas = Canvas::create("Minko Example - Video Camera");
    auto sceneManager = SceneManager::create(canvas);

    canvas->context()->errorsEnabled(true);

    sceneManager->assets()->loader()
    ->queue("effect/Basic.effect")
    ->queue("effect/Background.effect");
  
    auto cubeGeometry = geometry::CubeGeometry::create(sceneManager->assets()->context());
    sceneManager->assets()->geometry("cubeGeometry", cubeGeometry);

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto mesh = scene::Node::create("mesh")
        ->addComponent(Transform::create());

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
    
    auto _ = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
        videoCamera = VideoCamera::create();
        videoCamera->start();
        
        auto previewComponent = VideoCameraPreview::create(sceneManager, canvas->context(), videoCamera);
        previewComponent->previewSurface()->effect(sceneManager->assets()->effect("effect/Background.effect"));
        
        auto previewNode = Node::create("previewNode")
        ->addComponent(Transform::create())
        ->addComponent(previewComponent);
        
        root->addChild(previewNode);
        
        auto meshMaterial = material::BasicMaterial::create();
        meshMaterial->diffuseColor(0xff0000ff);
        
        mesh->addComponent(Surface::create(
            sceneManager->assets()->geometry("cubeGeometry"),
            meshMaterial,
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
            mesh->component<Transform>()->matrix() * math::rotate(0.01f, math::vec3(0, 1, 0))
        );

        videoCamera->requestFrame();
        
        sceneManager->nextFrame(time, deltaTime, shouldRender);

    });
    
    sceneManager->assets()->loader()->load();
    canvas->run();
    
    videoCamera->stop();

    return 0;
}