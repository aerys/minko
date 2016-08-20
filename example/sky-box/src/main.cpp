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
#include "minko/MinkoJPEG.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::scene;

const std::string   SKYBOX_TEXTURE  = "texture/cloudySea-diffuse.jpg";
const unsigned int  NUM_OBJECTS     = 15;

Node::Ptr
createTransparentObject(float scale, float rotationY, geometry::Geometry::Ptr geom, render::Effect::Ptr fx)
{
    assert(NUM_OBJECTS > 0);

    auto        randomAxis  = math::normalize(math::vec3(float(std::rand()), float(std::rand()), float(std::rand())));
    const float randomAng   = 2.0f * float(M_PI) * rand() / float(RAND_MAX);

    auto m = math::mat4(1.f);
    m = math::rotate(m, randomAng, randomAxis);
    m = math::translate(m, math::vec3(1.f, 0.f, 0.f));
    m = math::scale(m, math::vec3(scale));
    m = math::rotate(m, rotationY, math::vec3(0.f, 1.f, 0.f));

    return scene::Node::create()
        ->addComponent(Transform::create(m))
        ->addComponent(Surface::create(
            geom,
            material::BasicMaterial::create()
                ->diffuseColor(math::vec4(math::rgbColor(math::vec3((rotationY / (2.f * float(M_PI))) * 360, 1.0f, 0.5f)), 0.5f))
                ->triangleCulling(render::TriangleCulling::BACK),
            fx
        ));
}

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Skybox");
    auto sceneManager = SceneManager::create(canvas);
    auto loader = sceneManager->assets()->loader();

    loader->options()
        ->resizeSmoothly(true)
        ->generateMipmaps(true)
        ->registerParser<file::JPEGParser>("jpg");

    loader
        ->queue(SKYBOX_TEXTURE)
        ->queue("effect/Basic.effect")
        ->queue("effect/Skybox/Skybox.effect");

    auto root = scene::Node::create("root")
        ->addComponent(sceneManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(
            math::inverse(math::lookAt(math::vec3(0), math::vec3(0, 0, 3), math::vec3(0, 1, 0)))
        ))
        ->addComponent(PerspectiveCamera::create(canvas->aspectRatio()));

    auto sky = scene::Node::create("sky")
        ->addComponent(Transform::create(
            math::scale(math::mat4(1.f), math::vec3(100.f))
        ));

    auto objects = scene::Node::create("objects")
        ->addComponent(Transform::create(
            //math::rotate(math::mat4(1.f), .2f, math::vec3(1.f, 0.f, 0.f))
        ));

    auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        auto assets = sceneManager->assets();

        sky->addComponent(Surface::create(
                geometry::SphereGeometry::create(assets->context(), 16, 16),
                material::Material::create()->set({
                    { "diffuseLatLongMap", assets->texture(SKYBOX_TEXTURE)->sampler() }
                }),
                assets->effect("effect/Skybox/Skybox.effect")
            ));

        assert(NUM_OBJECTS > 0);

        const float scale = 1.25f * float(M_PI) / float(NUM_OBJECTS);
        const float dAngle = 2.0f * float(M_PI) / float(NUM_OBJECTS);

        auto cubeGeom = geometry::CubeGeometry::create(sceneManager->assets()->context());
        for (unsigned int objId = 0; objId < NUM_OBJECTS; ++objId)
            objects->addChild(createTransparentObject(scale, objId * dAngle, cubeGeom, assets->effect("effect/Basic.effect")));

        root
            ->addChild(camera)
            ->addChild(sky)
            ->addChild(objects);
    });

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
    {
        camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
    });

    auto yaw = 0.f;
    auto pitch = float(M_PI) * .5f;
    auto minPitch = 0.1f;
    auto maxPitch = float(M_PI) - .1f;
    auto lookAt = math::vec3(0.f, 0.f, 0.f);
    auto distance = 4.f;

    // handle mouse signals
    auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
    {
        distance += float(v) / 10.f;
    });

    Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
    auto cameraRotationXSpeed = 0.f;
    auto cameraRotationYSpeed = 0.f;

    auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
    {
        mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr, int dx, int dy)
        {
            cameraRotationYSpeed = float(dx) * .01f;
            cameraRotationXSpeed = float(dy) * -.01f;
        });
    });

    auto mouseUp = canvas->mouse()->leftButtonUp()->connect([&](input::Mouse::Ptr m)
    {
        mouseMove = nullptr;
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

        if (distance <= 0.f)
            distance = 0.1f;

        camera->component<Transform>()->matrix(math::inverse(math::lookAt(
            math::vec3(
                lookAt.x + distance * std::cos(yaw) * std::sin(pitch),
                lookAt.y + distance * std::cos(pitch),
                lookAt.z + distance * std::sin(yaw) * std::sin(pitch)
            ),
            lookAt,
            math::vec3(0.f, 1.f, 0.f)
        )));

        auto skyTransform = sky->component<Transform>();
        auto objectsTransform = objects->component<Transform>();

        skyTransform->matrix(math::rotate(skyTransform->matrix(), .001f, math::vec3(0.f, 1.f, 0.f)));
        objectsTransform->matrix(math::rotate(objectsTransform->matrix(), -.02f, math::vec3(0.f, 1.f, 0.f)));

        sceneManager->nextFrame(time, deltaTime);
    });

    loader->load();
    canvas->run();
}
