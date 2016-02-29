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
#include "minko/MinkoDebug.hpp"

#include "TextureDebugDisplay.hpp"

using namespace minko;
using namespace minko::component;

scene::Node::Ptr lightNode;
scene::Node::Ptr debugNode = scene::Node::create("debug", scene::BuiltinLayout::DEBUG_ONLY);
bool projectionAuto = false;
DirectionalLight::Ptr directionalLight = DirectionalLight::create(.3f);
DirectionalLight::Ptr directionalLight2 = DirectionalLight::create(.3f);

std::array<FrustumDisplay::Ptr, 5> frustums;

void
initializeShadowMapping(scene::Node::Ptr root, file::AssetLibrary::Ptr assets)
{
    directionalLight->enableShadowMapping(512);
    directionalLight2->enableShadowMapping(256);

    lightNode = scene::Node::create("light")
        ->addComponent(AmbientLight::create())
        ->addComponent(directionalLight)
        ->addComponent(Transform::create(math::inverse(
            math::lookAt(math::vec3(1.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f))
        )));
    root->addChild(lightNode);

    if (directionalLight->shadowMappingEnabled())
    {
        auto debugDisplay = TextureDebugDisplay::create();

        debugDisplay->initialize(assets, directionalLight->shadowMap());
        debugNode->addComponent(debugDisplay);
    }

    auto lightNode2 = scene::Node::create()
        ->addComponent(directionalLight2)
        ->addComponent(Transform::create(math::inverse(
            math::lookAt(math::vec3(-1.f, 1.f, 0.5f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f))
        )));
    root->addChild(lightNode2);
}

int main(int argc, char** argv)
{
    std::cout << "Press [C]\tto show the camera frustum\n"
        << "Press [L]\tto show the shadow cascade frustums"
        << std::endl;

    auto canvas = Canvas::create("Minko Application", 900, 600);
    auto sceneManager = SceneManager::create(canvas);
    auto debugDisplay = false;

    auto loader = sceneManager->assets()->loader();
    loader->queue("effect/Phong.effect");
    loader->queue("effect/debug/ShadowMappingDebug.effect");

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto debugRenderer = Renderer::create();
    debugRenderer->layoutMask(scene::BuiltinLayout::DEBUG_ONLY);
    debugRenderer->clearBeforeRender(false);
    auto renderer = Renderer::create(0x1f1f1fff);
    auto camera = scene::Node::create("camera")
        ->addComponent(renderer)
        ->addComponent(debugRenderer)
        ->addComponent(Transform::create(
            math::inverse(math::lookAt(math::vec3(0.f, 8.f, 8.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f))
        )))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio(), .785f, .1f, 100.f));

    root->addChild(camera);
    root->addChild(debugNode);

    scene::Node::Ptr teapot;

    auto _ = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
        renderer->effect(sceneManager->assets()->effect("effect/Phong.effect"));

        auto cube = scene::Node::create("cube", scene::BuiltinLayout::DEFAULT | scene::BuiltinLayout::CAST_SHADOW)
            ->addComponent(Surface::create(
                geometry::CubeGeometry::create(sceneManager->assets()->context()),
                material::BasicMaterial::create()
                    ->diffuseColor(math::vec4(1.f, .3f, .3f, 1.f))
            ))
            ->addComponent(Transform::create(math::translate(math::vec3(-1.5f, .5f, 0.f))));
        root->addChild(cube);

        auto sphere = scene::Node::create("sphere", scene::BuiltinLayout::DEFAULT | scene::BuiltinLayout::CAST_SHADOW)
            ->addComponent(Surface::create(
                geometry::SphereGeometry::create(sceneManager->assets()->context(), 40),
                material::BasicMaterial::create()
                    ->diffuseColor(math::vec4(.3f, .3f, 1.f, 1.f))
            ))
            ->addComponent(Transform::create(math::translate(math::vec3(1.5f, .5f, 0.f))));
        root->addChild(sphere);

        teapot = scene::Node::create("teapot", scene::BuiltinLayout::DEFAULT | scene::BuiltinLayout::CAST_SHADOW)
            ->addComponent(Surface::create(
                geometry::TeapotGeometry::create(sceneManager->assets()->context())->computeNormals(),
                material::BasicMaterial::create()
                    ->diffuseColor(math::vec4(.3f, 1.f, .3f, 1.f))
            ))
            ->addComponent(Transform::create(math::scale(math::vec3(.3f))));
        root->addChild(teapot);

        auto ground = scene::Node::create("ground")
            ->addComponent(Surface::create(
                geometry::QuadGeometry::create(sceneManager->assets()->context()),
                material::BasicMaterial::create()
                    ->diffuseColor(0xffffffff)
            ))
            ->addComponent(Transform::create(
                math::rotate(-math::half_pi<float>(), math::vec3(1.f, 0.f, 0.f))
                * math::scale(math::vec3(10.f))
            ));
        root->addChild(ground);

        initializeShadowMapping(root, sceneManager->assets());

        auto perspective = camera->component<PerspectiveCamera>();
        camera->component<Transform>()->updateModelToWorldMatrix();
        directionalLight->target()->component<Transform>()->updateModelToWorldMatrix();
        directionalLight->computeShadowProjection(perspective->viewMatrix(), perspective->projectionMatrix());

        directionalLight2->target()->component<Transform>()->updateModelToWorldMatrix();
        directionalLight2->computeShadowProjection(perspective->viewMatrix(), perspective->projectionMatrix());
    });

    bool cameraMoved = true;

    auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
    {
        if (k->keyIsDown(input::Keyboard::Key::UP))
        {
            camera->component<Transform>()->matrix(
                camera->component<Transform>()->matrix() * math::translate(math::vec3(0.f, 0.f, -.1f))
            );
            cameraMoved = true;
        }
        if (k->keyIsDown(input::Keyboard::Key::DOWN))
        {
            camera->component<Transform>()->matrix(
                camera->component<Transform>()->matrix() * math::translate(math::vec3(0.f, 0.f, .1f))
            );
            cameraMoved = true;
        }
        if (k->keyIsDown(input::Keyboard::Key::R))
        {
            if (renderer->effect() == sceneManager->assets()->effect("effect/Phong.effect"))
                renderer->effect(sceneManager->assets()->effect("effect/debug/ShadowMappingDebug.effect"));
            else
                renderer->effect(sceneManager->assets()->effect("effect/Phong.effect"));
        }
        if (k->keyIsDown(input::Keyboard::Key::Q))
        {
            camera->component<Transform>()->matrix(
                camera->component<Transform>()->matrix() * math::translate(math::vec3(-.1f, 0.f, 0.f))
            );
            cameraMoved = true;
        }
        if (k->keyIsDown(input::Keyboard::Key::D))
        {
            camera->component<Transform>()->matrix(
                camera->component<Transform>()->matrix() * math::translate(math::vec3(.1f, 0.f, 0.f))
            );
            cameraMoved = true;
        }
        if (k->keyIsDown(input::Keyboard::Key::LEFT))
        {
            camera->component<Transform>()->matrix(
                math::rotate(-.03f, math::vec3(0.f, 1.f, .0f)) * camera->component<Transform>()->matrix()
            );
            cameraMoved = true;
        }
        if (k->keyIsDown(input::Keyboard::Key::RIGHT))
        {
            camera->component<Transform>()->matrix(
                math::rotate(.03f, math::vec3(0.f, 1.f, .0f)) * camera->component<Transform>()->matrix()
            );
            cameraMoved = true;
        }
        if (k->keyIsDown(input::Keyboard::Key::C))
        {
            auto p = camera->component<PerspectiveCamera>();

            if (debugNode->hasComponent(frustums[4]))
                debugNode->removeComponent(frustums[4]);
            else
            {
                frustums[4] = FrustumDisplay::create(p->viewProjectionMatrix());
                debugNode->addComponent(frustums[4]);
            }
        }
        if (k->keyIsDown(input::Keyboard::Key::L))
        {
            auto p = camera->component<PerspectiveCamera>();
            std::array<math::vec4, 4> colors = {
                math::vec4(1.f, 0.f, 0.f, .1f),
                math::vec4(0.f, 1.f, 0.f, .1f),
                math::vec4(0.f, 0.f, 1.f, .1f),
                math::vec4(1.f, 1.f, 0.f, .1f)
            };

            for (auto i = 0u; i < directionalLight->numShadowCascades(); ++i)
            {
                if (frustums[i] && debugNode->hasComponent(frustums[i]))
                {
                    debugNode->removeComponent(frustums[i]);
                    debugDisplay = false;
                    cameraMoved = true;
                }
                else
                {
                    frustums[i] = FrustumDisplay::create(
                        directionalLight->shadowProjections()[i] * math::inverse(directionalLight->target()->component<Transform>()->modelToWorldMatrix())
                    );
                    frustums[i]->material()->diffuseColor(colors[i]);
                    debugNode->addComponent(frustums[i]);
                    debugDisplay = true;
                }

            }
        }
        if (k->keyIsDown(input::Keyboard::Key::A))
        {
            if (directionalLight->shadowMappingEnabled())
                directionalLight->disableShadowMapping(k->keyIsDown(input::Keyboard::Key::SHIFT));
            else
            {
                directionalLight->enableShadowMapping();
                cameraMoved = true;
            }
        }
        if (k->keyIsDown(input::Keyboard::Key::Z))
        {
            if (directionalLight2->shadowMappingEnabled())
                directionalLight2->disableShadowMapping(k->keyIsDown(input::Keyboard::Key::SHIFT));
            else
            {
                directionalLight2->enableShadowMapping();
                cameraMoved = true;
            }
        }
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
        cameraMoved = true;
    });

    float yaw = -0.8f;
    float pitch = 0.9f;//float(M_PI) * .5f;
    // float pitch = float(M_PI) * .5f;
    auto minPitch = 0.f + 0.1f;
    auto maxPitch = float(M_PI) * .5f - .1f;
    // auto maxPitch = float(M_PI) - .1f;
    auto lookAt = math::vec3(0.f, 0.f, 0.f);
    float distance = 6.f;
    float minDistance = 1.f;
    float maxDistance = 40.f;
    float zoomSpeed = 0.f;

    // handle mouse signals
    auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
    {
        zoomSpeed -= float(v) * .1f;
    });

    Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
    auto cameraRotationXSpeed = 0.f;
    auto cameraRotationYSpeed = 0.f;

    auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
    {
        mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr, int dx, int dy)
        {
            cameraRotationYSpeed = (float)dx * .02f;
            cameraRotationXSpeed = (float)dy * -.02f;
        });
    });

    auto mouseUp = canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr m)
    {
        mouseMove = nullptr;
    });

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
        distance += zoomSpeed;
        zoomSpeed *= 0.9f;
        if (math::abs(zoomSpeed) > 0.01f)
        {
            if (distance < minDistance)
                distance = minDistance;
            else if (distance > maxDistance)
                distance = maxDistance;
            cameraMoved = true;
        }

        if (math::abs(cameraRotationYSpeed) > 0.001f)
        {
            yaw += cameraRotationYSpeed;
            cameraRotationYSpeed *= 0.9f;
            cameraMoved = true;
        }

        if (math::abs(cameraRotationXSpeed) > 0.001f)
        {
            pitch += cameraRotationXSpeed;
            cameraRotationXSpeed *= 0.9f;
            cameraMoved = true;
            if (pitch > maxPitch)
                pitch = maxPitch;
            else if (pitch < minPitch)
                pitch = minPitch;
        }

        if (cameraMoved)
        {
            camera->component<Transform>()->matrix(math::inverse(math::lookAt(
                math::vec3(
                    lookAt.x + distance * std::cos(yaw) * std::sin(pitch),
                    lookAt.y + distance * std::cos(pitch),
                    lookAt.z + distance * std::sin(yaw) * std::sin(pitch)
                ),
                lookAt,
                math::vec3(0.f, 1.f, 0.f)
            )));

            if (!debugDisplay)
            {
                auto p = camera->component<PerspectiveCamera>();

                directionalLight->computeShadowProjection(p->viewMatrix(), p->projectionMatrix(), 20.f);
                directionalLight2->computeShadowProjection(p->viewMatrix(), p->projectionMatrix(), 20.f);
            }
            cameraMoved = false;
        }

        teapot->component<Transform>()->matrix(
            math::rotate(-0.02f, math::vec3(0.f, 1.f, 0.f)) * teapot->component<Transform>()->matrix()
        );

        sceneManager->nextFrame(time, deltaTime);
    });

    sceneManager->assets()->loader()->load();
    canvas->run();

    return 0;
}
