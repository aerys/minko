#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoOffscreen.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;
using namespace minko::file;

int main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Offscreen", 1280, 720, true);

    auto sceneManager   = SceneManager::create(canvas);
    auto assets         = sceneManager->assets();
    auto mesh           = scene::Node::create("mesh");

    // setup assets
    assets->loader()->options()
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png");

    assets
        ->geometry("cube", geometry::CubeGeometry::create(assets->context()));

    assets->loader()
        ->queue("texture/box.png")
        ->queue("effect/Basic.effect");

    auto _ = assets->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        auto root   = scene::Node::create("root");
        auto camera = scene::Node::create("camera");

        root->addComponent(sceneManager);

        // setup camera
        camera->addComponent(Renderer::create(0x7F7F7FFF));
        camera->addComponent(Transform::create());
        camera->component<Transform>()->matrix()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f));
        camera->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));
        root->addChild(camera);

        // setup mesh
        mesh->addComponent(Transform::create());
        mesh->addComponent(Surface::create(
            assets->geometry("cube"),
            material::Material::create()
                ->set("material.diffuseColor", Vector4::create(0.f, 0.f, 1.f, 1.f))
                ->set("material.diffuseMap", assets->texture("texture/box.png")),
            assets->effect("effect/Basic.effect")
        ));
        root->addChild(mesh);
    });

    assets->loader()->load();

    PNGWriter::Ptr writer = PNGWriter::create();
    std::shared_ptr<std::vector<unsigned char>> buffer(new std::vector<unsigned char>(canvas->width() * canvas->height() * 4));

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
        mesh->component<Transform>()->matrix()->prependRotationY(.01f);

        static int lastTime = 0;

        if (time - lastTime > 1000)
        {
            std::cout << "Taking screenshot (screenshot.png)." << std::endl;
            lastTime = time;
            canvas->context()->readPixels(&*buffer->begin());
            writer->write("screenshot.png", *buffer, canvas->width(), canvas->height());
        }

        sceneManager->nextFrame(time, deltaTime);
    });

    canvas->run();
}
