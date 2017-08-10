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
using namespace minko::math;

const std::string TEXTURE_FILENAME = "texture/box.png";

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Visibility");

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

    std::cout << "Press [SPACE]\tto change a cube visibility" << std::endl;


    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
            Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 17.f))
        ))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

    root->addChild(camera);

    auto cubes = scene::Node::create("cubesContainer");

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        sceneManager->assets()->material("boxMaterial", material::BasicMaterial::create()->diffuseMap(sceneManager->assets()->texture(TEXTURE_FILENAME)));
        sceneManager->assets()->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()));

        for (int j = 3; j >= -3; --j)
        {
            for (int i = -3; i <= 3; ++i)
            {
                cubes
                    ->addChild(scene::Node::create("mesh")
                    ->addComponent(Surface::create(
                        sceneManager->assets()->geometry("cube"),
                        sceneManager->assets()->material("boxMaterial"),
                        sceneManager->assets()->effect("effect/Basic.effect")
                        ))
                    ->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(i * 2.f, j * 2.f)))
                );
            }
        }

        root->addChild(cubes);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
    {
        if (k->keyDown(input::Keyboard::SPACE))
        {
            auto numChildren = cubes->children().size();
            auto randomChild = cubes->children()[rand() % numChildren];

            randomChild->component<Surface>()->visible(!randomChild->component<Surface>()->visible());
        }
    });

    int frameId = 0;
    int cubeId = 0;

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime)
    {
        frameId++;
        sceneManager->nextFrame(time, deltaTime);

        if (frameId % 10 == 0)
        {
            cubeId++;
            auto numChildren = cubes->children().size();
            auto visibleChild = cubes->children()[(cubeId - 1) % numChildren];
            auto invisibleChild = cubes->children()[cubeId % numChildren];

            visibleChild->component<Surface>()->visible(true);
            invisibleChild->component<Surface>()->visible(false);
        }
    });

    sceneManager->assets()->loader()->load();
    canvas->run();
}


