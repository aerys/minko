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

static const std::string EFFECT_FILENAME = "effect/Phong.effect";

static const float CAMERA_SPEED = 20.0f;

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Fog");

    auto sceneManager = SceneManager::create(canvas);

    // setup assets
    sceneManager->assets()->loader()->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png");

    sceneManager->assets()->loader()
        ->queue(EFFECT_FILENAME);

    auto _ = sceneManager->assets()->loader()->complete()->connect([ = ](file::Loader::Ptr loader)
    {
        std::cout << "Press [L]\tto activate linear fog\n"
            << "Press [E]\tto activate exponential fog\n"
            << "Press [F]\tto activate square exponential fog\n"
            << "Press [N]\tto deactivate fog\n"
            << "Press [P]\tto increase fog density\n"
            << "Press [M]\tto decrease fog density"
            << std::endl;

        auto root = scene::Node::create("root")
            ->addComponent(sceneManager);

        auto teapotGeom = geometry::TeapotGeometry::create(sceneManager->assets()->context());
        teapotGeom->computeNormals();
        auto material = material::PhongMaterial::create();
        material->diffuseColor(0x00ff00ff);
        material->data()->set("fogTechnique", (int)material::FogTechnique::LIN);
        material->data()->set("fogBounds", math::vec2(10.f, 20.f));
        material->data()->set("fogColor", math::vec4(.5f, .5f, .5f, 1.f));
        auto mesh = scene::Node::create("mesh")
            ->addComponent(Transform::create(
                math::translate(math::vec3(0.f, 0.5f, 0.f)) * math::scale(math::vec3(.5f))
            ))
            ->addComponent(Surface::create(
                teapotGeom,
                material,
                sceneManager->assets()->effect(EFFECT_FILENAME)
            ));
        root->addChild(mesh);

        auto camera = scene::Node::create("camera")
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(Transform::create(math::inverse(
                math::lookAt(math::vec3(6.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f))
            )))
            ->addComponent(Camera::create(math::perspective(0.785f, canvas->aspectRatio(), .1f, 20.f)));
        root->addChild(camera);

        auto groundMaterial = material::PhongMaterial::create(material);
        groundMaterial->diffuseColor(0xFF0000FF);
        auto groundNode = scene::Node::create("ground")
            ->addComponent(Transform::create(math::scale(math::vec3(100.0f)) * math::rotate(-float(M_PI) * .5f, math::vec3(1.f, 0.f, 0.f))))
            ->addComponent(Surface::create(
                geometry::QuadGeometry::create(sceneManager->assets()->context()),
                groundMaterial,
                sceneManager->assets()->effect(EFFECT_FILENAME)
            ));
        root->addChild(groundNode);

        auto lights = scene::Node::create("lights")
            ->addComponent(AmbientLight::create(0.25f))
            ->addComponent(DirectionalLight::create()->diffuse(0.8f)->color(math::vec3(1.f)))
            ->addComponent(Transform::create(math::inverse(
                math::lookAt(math::vec3(0.f), math::vec3(-1.f), math::vec3(0.f, 1.f, 0.f))
            )));
        root->addChild(lights);

        math::vec3 cameraMove(0.f);

        auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
        {
            if (k->keyIsDown(input::Keyboard::LEFT))
                cameraMove = math::vec3(-1.0f, 0.0f, 0.0f);
            else if (k->keyIsDown(input::Keyboard::UP))
                cameraMove = math::vec3(0.0f, 0.0f, -1.0f);
            else if (k->keyIsDown(input::Keyboard::RIGHT))
                cameraMove = math::vec3(1.0f, 0.0f, 0.0f);
            else if (k->keyIsDown(input::Keyboard::DOWN))
                cameraMove = math::vec3(0.0f, 0.0f, 1.0f);

            else if (k->keyIsDown(input::Keyboard::P))
            {
                // material->fogDensity(material->fogDensity() * 2.0f);
                // groundMaterial->fogDensity(material->fogDensity() * 2.0f);

                // std::cout << "fog density: " << material->fogDensity() << std::endl;
            }
            else if (k->keyIsDown(input::Keyboard::M))
            {
                // material->fogDensity(material->fogDensity() / 2.0f);
                // groundMaterial->fogDensity(material->fogDensity() / 2.0f);

                // std::cout << "fog density: " << material->fogDensity() << std::endl;
            }

            else if (k->keyIsDown(input::Keyboard::N))
            {
                material->data()->unset("fogTechnique");
                groundMaterial->data()->unset("fogTechnique");

                std::cout << "fog is inactive" << std::endl;
            }
            else if (k->keyIsDown(input::Keyboard::L))
            {
                material->data()->set("fogTechnique", (int)material::FogTechnique::LIN);
                groundMaterial->data()->set("fogTechnique", (int)material::FogTechnique::LIN);

                std::cout << "fog type is linear" << std::endl;
            }
            else if (k->keyIsDown(input::Keyboard::E))
            {
                material->data()->set("fogTechnique", (int)material::FogTechnique::EXP);
                groundMaterial->data()->set("fogTechnique", (int)material::FogTechnique::EXP);

                std::cout << "fog type is exponential" << std::endl;
            }
            else if (k->keyIsDown(input::Keyboard::F))
            {
                material->data()->set("fogTechnique", (int)material::FogTechnique::EXP2);
                groundMaterial->data()->set("fogTechnique", (int)material::FogTechnique::EXP2);

                std::cout << "fog type is exponential2" << std::endl;
            }
        });

        auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
        {
            camera->component<Camera>()->projectionMatrix(math::perspective(0.785f, float(w) / float(h), .1f, 20.f));
        });

        auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float dt)
        {
            cameraMove = cameraMove * CAMERA_SPEED * (dt / 1000.0f);
            camera->component<Transform>()->matrix(
                camera->component<Transform>()->matrix() * math::translate(cameraMove)
            );
            cameraMove *= .9f;

            mesh->component<Transform>()->matrix(

            );

            sceneManager->nextFrame(time, dt);
        });

        canvas->run();
    });

    sceneManager->assets()->loader()->load();
}
