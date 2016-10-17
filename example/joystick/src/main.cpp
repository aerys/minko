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
using namespace minko::math;
using namespace minko::component;

std::unordered_map<input::Joystick::Ptr, scene::Node::Ptr> joystickToCube;
std::unordered_map<input::Joystick::Ptr, Signal<input::Joystick::Ptr, int, int>::Slot> joystickToButtonDownSlot;

Signal<AbstractCanvas::Ptr, input::Joystick::Ptr>::Slot joystickAdded;
Signal<AbstractCanvas::Ptr, input::Joystick::Ptr>::Slot joystickRemoved;

void
joystickButtonDownHandler(input::Joystick::Ptr joystick, int which, int buttonId)
{
    if (static_cast<input::Joystick::Button>(buttonId) == input::Joystick::Button::DPadUp)
        joystickToCube[joystick]->component<Transform>()->matrix(math::translate(math::vec3(0.f, 0.f, 0.1f)) * joystickToCube[joystick]->component<Transform>()->matrix());

    if (static_cast<input::Joystick::Button>(buttonId) == input::Joystick::Button::DPadDown)
        joystickToCube[joystick]->component<Transform>()->matrix(math::translate(math::vec3(0.f, 0.f, -0.1f)) * joystickToCube[joystick]->component<Transform>()->matrix());

    if (static_cast<input::Joystick::Button>(buttonId) == input::Joystick::Button::DPadLeft)
        joystickToCube[joystick]->component<Transform>()->matrix(math::translate(math::vec3(0.1f, 0.f, 0.f)) * joystickToCube[joystick]->component<Transform>()->matrix());

    if (static_cast<input::Joystick::Button>(buttonId) == input::Joystick::Button::DPadRight)
        joystickToCube[joystick]->component<Transform>()->matrix(math::translate(math::vec3(-0.1f, 0.f, 0.f)) * joystickToCube[joystick]->component<Transform>()->matrix());
}

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Tutorial - Joystick");
    auto sceneManager = SceneManager::create(canvas);
    auto assets = sceneManager->assets();

    // setup assets
    assets->loader()->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true);

    assets->loader()->queue("effect/Basic.effect");

    std::cout << "Plug a gamepad and move the cube." << std::endl;

    std::shared_ptr<scene::Node>        currentJoystickCube;
    std::shared_ptr<input::Joystick>    currentJoystick;

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Camera::create(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f)))
        ->addComponent(Transform::create(
            math::inverse(
                math::lookAt(
                    math::vec3(0.f, 0.f, -5.f), math::vec3(), math::vec3(0, 1, 0)
                )
            )
        ));

    root->addChild(camera);

    auto complete = assets->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
        std::cout << "Press any gamepad key to enable it." << std::endl;
#endif

        joystickAdded = canvas->joystickAdded()->connect([&](AbstractCanvas::Ptr abscanvas, input::Joystick::Ptr joystick)
        {
            auto mesh = scene::Node::create("mesh")
                ->addComponent(Transform::create())
                ->addComponent(Surface::create(
                    geometry::CubeGeometry::create(sceneManager->assets()->context()),
                    material::BasicMaterial::create()->diffuseColor(math::vec4(
                        float(rand()) / float(RAND_MAX),
                        float(rand()) / float(RAND_MAX),
                        float(rand()) / float(RAND_MAX),
                        1.f
                    )),
                    sceneManager->assets()->effect("effect/Basic.effect")
                ));

            currentJoystick = joystick;
            currentJoystickCube = mesh;

            joystickToCube[joystick] = mesh;
            joystickToButtonDownSlot[joystick] = joystick->joystickButtonDown()->connect(
            [&](input::Joystick::Ptr joystick, int which, int buttonId)
            {
                joystickButtonDownHandler(joystick, which, buttonId);
            });

            root->addChild(mesh);
        });

        joystickRemoved = canvas->joystickRemoved()->connect([&](AbstractCanvas::Ptr canvas, input::Joystick::Ptr joystick)
        {
            root->removeChild(joystickToCube[joystick]);
            joystickToButtonDownSlot.erase(joystick);
            joystickToCube.erase(joystick);
        });
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, minko::uint w, minko::uint h)
    {
        auto aspectRatio = float(w) / float(h);
        camera->component<Camera>()->projectionMatrix(math::perspective(.785f, aspectRatio, 0.1f, 1000.f));
    });

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr ac, float t, float dt)
    {
        auto canvas = std::dynamic_pointer_cast<Canvas>(ac);
        auto joysticksList = canvas->joysticks();

        for (auto it = joysticksList.begin(); it != joysticksList.end(); ++it)
        {
            auto joy = it->second;

            if (joy->isButtonDown(input::Joystick::Button::DPadUp))
                std::cout << "DPadUp pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::DPadDown))
                std::cout << "DPadDown pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::DPadLeft))
                std::cout << "DPadLeft pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::DPadRight))
                std::cout << "DPadRight pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::Start))
                std::cout << "Start pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::Select))
                std::cout << "Select pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::L3))
                std::cout << "L3 pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::R3))
                std::cout << "R3 pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::LB))
                std::cout << "LB pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::RB))
                std::cout << "RB pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::A))
                std::cout << "A pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::B))
                std::cout << "B pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::X))
                std::cout << "X pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::Y))
                std::cout << "Y pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::Home))
                std::cout << "Home pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::LT))
                std::cout << "LT pressed !" << std::endl;

            if (joy->isButtonDown(input::Joystick::Button::RT))
                std::cout << "RT pressed !" << std::endl;

            // Left stick axis
            if (canvas->getJoystickAxis(joy, 0) > 5000.f)
                joystickToCube[joy]->component<Transform>()->matrix(math::translate(math::vec3(-0.1f, 0.f, 0.f)) * joystickToCube[joy]->component<Transform>()->matrix());

            if (canvas->getJoystickAxis(joy, 0) < -5000.f)
                joystickToCube[joy]->component<Transform>()->matrix(math::translate(math::vec3(0.1f, 0.f, 0.f)) * joystickToCube[joy]->component<Transform>()->matrix());

            if (canvas->getJoystickAxis(joy, 1) > 5000.f)
                joystickToCube[joy]->component<Transform>()->matrix(math::translate(math::vec3(0.f, 0.f, -0.1f)) * joystickToCube[joy]->component<Transform>()->matrix());

            if (canvas->getJoystickAxis(joy, 1) < -5000.f)
                joystickToCube[joy]->component<Transform>()->matrix(math::translate(math::vec3(0.f, 0.f, 0.1f)) * joystickToCube[joy]->component<Transform>()->matrix());

            // Right stick axis
            if (canvas->getJoystickAxis(joy, 2) > 5000.f)
                joystickToCube[joy]->component<Transform>()->matrix(joystickToCube[joy]->component<Transform>()->matrix() * math::rotate(-0.1f, math::vec3(0.f, 1.f, 0.f)));

            else if (canvas->getJoystickAxis(joy, 2) < -5000.f)
                joystickToCube[joy]->component<Transform>()->matrix(joystickToCube[joy]->component<Transform>()->matrix() * math::rotate(0.1f, math::vec3(0.f, 1.f, 0.f)));

            else if (canvas->getJoystickAxis(joy, 3) > 5000.f)
                joystickToCube[joy]->component<Transform>()->matrix(joystickToCube[joy]->component<Transform>()->matrix() * math::rotate(-0.1f, math::vec3(0.f, 0.f, 1.f)));

            else if (canvas->getJoystickAxis(joy, 3) < -5000.f)
                joystickToCube[joy]->component<Transform>()->matrix(joystickToCube[joy]->component<Transform>()->matrix() * math::rotate(0.1f, math::vec3(0.f, 0.f, 1.f)));
        }

        sceneManager->nextFrame(t, dt);
    });

    sceneManager->assets()->loader()->load();
    canvas->run();
}
