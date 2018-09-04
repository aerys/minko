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

using namespace minko;
using namespace minko::component;
//using namespace minko::math;

scene::Node::Ptr camera = nullptr;

int
main(int argc, char** argv)
{
    auto canvas        = Canvas::create("Minko Example - Frustum");

    canvas->context()->errorsEnabled(true);

    auto sceneManager        = SceneManager::create(canvas);
    auto root                = scene::Node::create("root")->addComponent(sceneManager);

    auto quadTreeRoot        = scene::Node::create("quadTreeRoot");
    auto cubeGroup           = scene::Node::create("cubeGroup");

    cubeGroup->addComponent(component::Transform::create());

    // setup assets
    sceneManager->assets()->loader()->options()
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png");

    sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));

    sceneManager->assets()->loader()->queue("effect/Basic.effect");

    // camera init
    camera = scene::Node::create("camera")
        ->addComponent(Renderer::create())
        ->addComponent(Camera::create(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f)));
        ->addComponent(Culling::create(math::Frustum::create(), "camera.worldToScreenMatrix"))
        ->addComponent(Transform::create(
            Matrix4x4::create()->lookAt(
                math::vec3(0.f, 0.f, 0.f),
                math::vec3(rand() % 200 - 100.f, rand() % 200 - 100.f, rand() % 200 - 100.f)
            )
        ));

    root->addChild(camera);

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<Camera>()->projectionMatrix(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime, bool shouldRender)
    {
        camera->component<Transform>()->matrix()->lock()->appendRotationY(0.02f)->appendRotationZ(-0.014f)->unlock();
        sceneManager->nextFrame(time, deltaTime, shouldRender);
        std::cout << "Num drawCalls : " << camera->component<Renderer>()->numDrawCalls() << std::endl;
    });

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        std::shared_ptr<material::BasicMaterial> material = material::BasicMaterial::create()->diffuseColor(0xFF00FFFF);

        for (uint i = 0; i < 25; ++i)
        {
            auto mesh = scene::Node::create("mesh")
                ->addComponent(Transform::create(math::Matrix4x4::create()
                    ->appendTranslation(rand() % 200 - 100.f, rand() % 200 - 100.f, rand() % 200 - 100.f)
                ))
                ->addComponent(Surface::create(
                    sceneManager->assets()->geometry("cube"),
                    material,
                    sceneManager->assets()->effect("effect/Basic.effect")
                ));
            mesh->layouts(mesh->layouts() | scene::Layout::Group::CULLING);

            cubeGroup->addChild(mesh);
        }

        root->addChild(cubeGroup);
    });

    sceneManager->assets()->loader()->load();

    canvas->run();
}
