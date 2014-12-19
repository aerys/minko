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
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoASSIMP.hpp"
#include "minko/MinkoSerializer.hpp"

std::string MODEL_FILENAME = "cube.scene";
using namespace minko;
using namespace minko::scene;
using namespace minko::component;

int
main(int argc, char** argv)
{
    auto inputFileName = std::string(MODEL_FILENAME);

    for (auto i = 1; i < argc; ++i)
    {
        const auto arg = std::string(argv[i]);
        if (arg == "-i")
            inputFileName = std::string(argv[++i]);
    }

    auto canvas = Canvas::create("Minko Example - Scene files");
    auto sceneManager = SceneManager::create(canvas);
    auto defaultLoader = sceneManager->assets()->loader();
    auto fxLoader = file::Loader::create(defaultLoader);

    fxLoader
        //->queue("effect/Phong.effect")
        ->queue("effect/Basic.effect");
    defaultLoader->options()
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png")
        ->registerParser<file::JPEGParser>("jpg")
        ->registerParser<file::OBJParser>("obj")
        ->registerParser<file::ColladaParser>("dae")
        ->registerParser<file::SceneParser>("scene");

    auto fxComplete = fxLoader->complete()->connect([&](file::Loader::Ptr loader)
    {
        defaultLoader->options()->effect(sceneManager->assets()->effect("effect/Basic.effect"));
        defaultLoader->options()->disposeTextureAfterLoading(true);
        defaultLoader->queue(inputFileName);
        defaultLoader->load();
    });

    auto fxError = defaultLoader->error()->connect([&](file::Loader::Ptr loader, const file::Error& error)
    {
        std::cout << "File loading error: " << error.what() << std::endl;
    });

    
    sceneManager->assets()
        ->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
        ->geometry("sphere", geometry::SphereGeometry::create(sceneManager->assets()->context(), 20, 20));
    
    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);
    
    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
        math::inverse(math::lookAt(math::vec3(0.f, 0.f, 20.f), math::zero<math::vec3>(), math::vec3(0.f, 1.f, 0.f))
        )))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));
    
    root->addChild(camera);

    auto _ = defaultLoader->complete()->connect([=](file::Loader::Ptr loader)
    {
        auto sceneNode = sceneManager->assets()->symbol(inputFileName);
        
        root->addChild(sceneNode);
        
        if (!sceneNode->hasComponent<Transform>())
            sceneNode->addComponent(Transform::create());

        //sceneNode->component<Transform>()->matrix(math::scale(math::vec3(0.01f)) * sceneNode->component<Transform>()->matrix());
    });

    auto yaw = float(M_PI) * 0.25f;
    auto pitch = float(M_PI) * .25f;
    auto roll = 0.f;
    float minPitch = 0.f + float(1e-5);
    float maxPitch = float(M_PI) - float(1e-5);
    auto lookAt = math::vec3(0.f, 0.f, 0.f);
    auto distance = 25.f;
    Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
    auto cameraRotationXSpeed = 0.f;
    auto cameraRotationYSpeed = 0.f;

    // handle mouse signals
    auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
    {
        distance += float(v) * 10;
    });

    mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr m, int dx, int dy)
    {
        if (m->leftButtonIsDown())
        {
            cameraRotationYSpeed = float(dx) * .01f;
            cameraRotationXSpeed = float(dy) * -.01f;
        }
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, unsigned int w, unsigned int h)
    {
        root->children()[0]->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime)
    {
        yaw += cameraRotationYSpeed;
        cameraRotationYSpeed *= 0.9f;
        pitch += cameraRotationXSpeed;
        cameraRotationXSpeed *= 0.9f;

        if (pitch > maxPitch)
            pitch = maxPitch;
        else if (pitch < minPitch)
            pitch = minPitch;

        camera->component<Transform>()->matrix(math::inverse(math::lookAt((
            math::vec3(
                lookAt.x + distance * std::cos(yaw) * std::sin(pitch),
                lookAt.y + distance * std::cos(pitch),
                lookAt.z + distance * std::sin(yaw) * std::sin(pitch)
            )),
            lookAt,
            math::vec3(0.f, 1.f, 0.f)
            )));
        sceneManager->nextFrame(time, deltaTime);
    });

    fxLoader->load();
    canvas->run();
}