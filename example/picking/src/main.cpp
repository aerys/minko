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

Signal<scene::Node::Ptr>::Slot pickingMouseClick;
Signal<scene::Node::Ptr>::Slot pickingMouseRightClick;
Signal<scene::Node::Ptr>::Slot pickingMouseOver;
Signal<scene::Node::Ptr>::Slot pickingMouseOut;

Signal<input::Mouse::Ptr>::Slot mouseLeftButtonDown;
Signal<input::Mouse::Ptr>::Slot mouseLeftButtonUp;

math::vec2 multiSelectFirstPoint;
math::vec2 multiSelectSecondPoint;

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Picking");
    auto sceneManager = SceneManager::create(canvas);

    // Setup assets
    sceneManager->assets()->loader()->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png");

    sceneManager->assets()->loader()
        ->queue("effect/Basic.effect")
        ->queue("effect/Picking.effect");

    auto redMaterial = material::BasicMaterial::create();
    redMaterial->diffuseColor(0xFF0000FF);

    auto greenMaterial = material::BasicMaterial::create();
    greenMaterial->diffuseColor(0x00FF00FF);

    auto blueMaterial = material::BasicMaterial::create();
    blueMaterial->diffuseColor(0x0000FFFF);

    sceneManager->assets()
        ->material("redMaterial",           redMaterial)
        ->material("greenMaterial",         greenMaterial)
        ->material("blueMaterial",          blueMaterial)
        ->geometry("cube",                  geometry::CubeGeometry::create(sceneManager->assets()->context()))
        ->geometry("sphere",                geometry::SphereGeometry::create(sceneManager->assets()->context()))
        ->geometry("quad",                 geometry::QuadGeometry::create(sceneManager->assets()->context()));

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto fov = .785f;
    auto aspectRatio = canvas->aspectRatio();
    auto zNear = 0.1f;
    auto zFar = 1000.f;

    auto cameraComponent = Camera::create(math::perspective(fov, aspectRatio, zNear, zFar));

    auto camera = scene::Node::create("camera")
        ->addComponent(Transform::create(
            math::inverse(math::lookAt(math::vec3(0.f, 0.f, 4.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f)))
        ))
        ->addComponent(cameraComponent);

    cameraComponent->data()
        ->set("fov", fov)
        ->set("aspectRatio", aspectRatio)
        ->set("zNear", zNear)
        ->set("zFar", zFar);

    root->addChild(camera);

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        auto cube = scene::Node::create("cubeNode")
            ->addComponent(Surface::create(
            sceneManager->assets()->geometry("cube"),
            sceneManager->assets()->material("redMaterial"),
            sceneManager->assets()->effect("effect/Basic.effect")))
            ->addComponent(Transform::create(
            math::translate(math::vec3(-1.4f, 0.f, 0.f))))
            ->layout(scene::BuiltinLayout::DEFAULT | scene::BuiltinLayout::PICKING);

        auto sphere = scene::Node::create("sphereNode")
            ->addComponent(Surface::create(
                sceneManager->assets()->geometry("sphere"),
                sceneManager->assets()->material("greenMaterial"),
                sceneManager->assets()->effect("effect/Basic.effect")))
            ->addComponent(Transform::create())
            ->layout(scene::BuiltinLayout::DEFAULT | scene::BuiltinLayout::PICKING);

        auto quad = scene::Node::create("quadNode")
            ->addComponent(Surface::create(
                sceneManager->assets()->geometry("quad"),
                sceneManager->assets()->material("blueMaterial"),
                sceneManager->assets()->effect("effect/Basic.effect")))
            ->addComponent(Transform::create(math::translate(math::vec3(1.4f, 0.f, 0.f))))
            ->layout(scene::BuiltinLayout::DEFAULT | scene::BuiltinLayout::PICKING);

        root
            ->addChild(cube)
            ->addChild(sphere)
            ->addChild(quad);

        // Picking component
        auto picking = Picking::create(camera, false, true);
        //picking->debug(true);

        root->addComponent(picking);

        pickingMouseClick = picking->mouseClick()->connect([&](scene::Node::Ptr node)
        {
            std::cout << "Click: " << node->name() << std::endl;
        });

        pickingMouseRightClick = picking->mouseRightClick()->connect([&](scene::Node::Ptr node)
        {
            std::cout << "Right Click: " << node->name() << std::endl;
        });

        pickingMouseOver = picking->mouseOver()->connect([&](scene::Node::Ptr node)
        {
            std::cout << "Mouse In: " << node->name() << std::endl;
        });

        pickingMouseOut = picking->mouseOut()->connect([&](scene::Node::Ptr node)
        {
            std::cout << "Mouse Out: " << node->name() << std::endl;
        });

        // Multiselect area
        mouseLeftButtonDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
        {
            multiSelectFirstPoint = math::vec2(m->x(), m->y());
        });

        mouseLeftButtonUp = canvas->mouse()->leftButtonUp()->connect([=](input::Mouse::Ptr m)
        {
            multiSelectSecondPoint = math::vec2(m->x(), m->y());

            auto bottomLeft = math::vec2(
                math::min(multiSelectFirstPoint.x, multiSelectSecondPoint.x),
                math::max(multiSelectFirstPoint.y, multiSelectSecondPoint.y)
            );

            auto topRight = math::vec2(
                math::max(multiSelectFirstPoint.x, multiSelectSecondPoint.x),
                math::min(multiSelectFirstPoint.y, multiSelectSecondPoint.y)
            );

            auto width = static_cast<int>(topRight.x - bottomLeft.x);
            auto height = static_cast<int>(bottomLeft.y - topRight.y);

            LOG_INFO("Picking nodes from an area of size: " << width << "x" << height << " pixels");

            auto pickedNodes = picking->pickArea(bottomLeft, topRight, true);

            for (auto node : pickedNodes)
                LOG_INFO(node.first->name());
        });
    });

    camera->addComponent(Renderer::create(0x7f7f7fff));

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<Camera>()->projectionMatrix(math::perspective(fov, aspectRatio, zNear, zFar));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime, bool shouldRender)
    {
        sceneManager->nextFrame(time, deltaTime);
    });

    sceneManager->assets()->loader()->load();
    canvas->run();
}


