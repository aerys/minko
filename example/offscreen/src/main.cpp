#include <time.h>

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoOffscreen.hpp"

#include <GL/osmesa.h>
#include <iostream>

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const uint      WINDOW_WIDTH  = 800;
const uint      WINDOW_HEIGHT = 600;

int main(int argc, char** argv)
{
    MinkoOffscreen::initialize("Minko Example - Offscreen", WINDOW_WIDTH, WINDOW_HEIGHT, true);

    auto sceneManager   = SceneManager::create(MinkoOffscreen::context());
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
        auto renderer = Renderer::create();
        renderer->backgroundColor(0x7F7F7FFF);
        camera->addComponent(renderer);
        camera->addComponent(Transform::create());
        camera->component<Transform>()->matrix()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f));
        camera->addComponent(PerspectiveCamera::create(.785f, 800.f / 600.f, .1f, 1000.f));
        root->addChild(camera);

        // setup mesh
        mesh->addComponent(Transform::create());
        mesh->addComponent(Surface::create(
            assets->geometry("cube"),
            data::Provider::create()
                ->set("material.diffuseColor",  Vector4::create(0.f, 0.f, 1.f, 1.f))
                ->set("material.diffuseMap",  assets->texture("texture/box.png")),
            assets->effect("effect/Basic.effect")
        ));
        root->addChild(mesh);
    });

    assets->loader()->load();

    auto enterFrame = MinkoOffscreen::enterFrame()->connect([&](float time, float deltaTime)
    {
        mesh->component<Transform>()->matrix()->prependRotationY(.01f);

        static int loop = 0;
        if (++loop % 100 == 0)
            MinkoOffscreen::takeScreenshot("screenshot.ppm");

        sceneManager->nextFrame(time, deltaTime);
    });

    MinkoOffscreen::run();
}
