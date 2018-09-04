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

scene::Node::Ptr camera = nullptr;

Signal<input::Keyboard::Ptr>::Slot keyDown;
Signal<input::Touch::Ptr, int, float, float>::Slot touchDown;

scene::Node::Ptr
createPointLight(math::vec3 color, math::vec3 position, file::AssetLibrary::Ptr assets)
{
    static int lightId = 0;

    auto mat = material::Material::create();
    mat->data()
        ->set("diffuseMap",     assets->texture("texture/sprite-pointlight.png")->sampler())
        ->set("diffuseTint",    math::vec4(color, 1.f));

    auto pointLight = scene::Node::create("pointLight_" + std::to_string(lightId++))
        ->addComponent(PointLight::create(100.f))
        ->addComponent(Transform::create(math::translate(position)))
        ->addComponent(Surface::create(
            assets->geometry("quad"),
            mat,
            assets->effect("effect/Sprite.effect")
        ));

    pointLight->component<PointLight>()->color(color);
    pointLight->component<PointLight>()->diffuse(.2f);
    // pointLight->component<PointLight>()->layoutMask(lightId % 2 == 0 ? 1<<2 : 1);

    return pointLight;
}

void
addLight(SceneManager::Ptr sceneManager, scene::Node::Ptr lights)
{
    const auto MAX_NUM_LIGHTS = 400;

    if (lights->children().size() == MAX_NUM_LIGHTS)
    {
        std::cout << "cannot add more lights" << std::endl;
        return;
    }

    float r = (float)rand() / (float)RAND_MAX;
    float theta = 2.0f * float(M_PI) * r;
    math::vec3 color = math::rgbColor(math::vec3(r * 360.f, 1.f, .5f));
    math::vec3 pos = math::vec3(
       cosf(theta) * 5.f + rand() / (float(RAND_MAX) * 3.f),
       2.5f + rand() / float(RAND_MAX),
       sinf(theta) * 5.f + rand() / (float(RAND_MAX) * 3.f)
   );

    lights->addChild(createPointLight(color, pos, sceneManager->assets()));

    std::cout << lights->children().size() << " lights" << std::endl;
}

void
removeLight(scene::Node::Ptr lights)
{
    if (lights->children().size() == 0)
        return;

    lights->removeChild(lights->children().back());
    std::cout << lights->children().size() << " lights" << std::endl;
}

void toggleNormalMap(file::AssetLibrary::Ptr assets, scene::Node::Ptr sphere)
{
    auto data = sphere->component<Surface>()->material()->data();
    bool hasNormalMap = data->hasProperty("normalMap");

    std::cout << "mesh does" << (!hasNormalMap ? " not " : " ")
    << "have a normal map:\t" << (hasNormalMap ? "remove" : "add")
    << " it" << std::endl;

    if (hasNormalMap)
        data->unset("normalMap");
    else
        data->set("normalMap", assets->texture("texture/normalmap-cells.png")->sampler());
}

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Light");

    canvas->context()->errorsEnabled(true);

    auto sceneManager        = SceneManager::create(canvas);
    auto root                = scene::Node::create("root")->addComponent(sceneManager);
    auto assets                = sceneManager->assets();
    auto sphereMaterial        = material::PhongMaterial::create()
        ->shininess(16.f)
        ->specularColor(math::vec4(1.0f, 1.0f, 1.0f, 1.0f))
        ->diffuseColor(math::vec4(1.f, 1.f, 1.f, 1.f));

    auto lights                = scene::Node::create("lights");

    std::cout << "Press [N]\tto toggle normal mapping\nPress [A]\tto add random light\nPress [R]\tto remove random light" << std::endl;

    // setup assets
    assets
        ->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
        ->geometry("quad", geometry::QuadGeometry::create(sceneManager->assets()->context()))
        ->geometry("sphere", geometry::SphereGeometry::create(assets->context(), 32, 32, true)->computeTangentSpace(false));

    assets->loader()->options()
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png");

    assets->loader()
        ->queue("texture/normalmap-cells.png")
        ->queue("texture/sprite-pointlight.png")
        ->queue("effect/Basic.effect")
        ->queue("effect/Sprite.effect")
        ->queue("effect/Phong.effect");

    auto _ = assets->loader()->complete()->connect([=](file::Loader::Ptr loader)
    {
        // ground
        auto ground = scene::Node::create("ground")
            // ->layouts(1 << 2 | 1)
            ->addComponent(Surface::create(
                assets->geometry("quad"),
                material::BasicMaterial::create()
                    ->diffuseColor(math::vec4(1.f, 1.f, 1.f, 1.f)),
                assets->effect("effect/Phong.effect")
            ))
            ->addComponent(Transform::create(math::scale(math::vec3(50.f)) * math::rotate(-1.57f, math::vec3(1.f, 0., 0.f))));

        // sphere
        auto sphere = scene::Node::create("sphere")
            ->addComponent(Surface::create(
                assets->geometry("sphere"),
                sphereMaterial,
                assets->effect("effect/Phong.effect")
            ))
            ->addComponent(Transform::create(math::translate(math::vec3(0.f, 2.f, 0.f)) * math::scale(math::vec3(3.f))));

        // spotLight
        auto spotLight = scene::Node::create("spotLight")
            ->addComponent(SpotLight::create(.15f, .4f))
            ->addComponent(Transform::create(math::inverse(
                math::lookAt(math::vec3(5.f, 20.f, 0.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f))
            )));

        spotLight->component<SpotLight>()->diffuse(.1f);

        lights->addComponent(Transform::create());

        root->addChild(ground);
        root->addChild(sphere);
        root->addChild(spotLight);
        root->addChild(lights);

        // handle keyboard signals
        keyDown = canvas->keyboard()->keyDown()->connect([=](input::Keyboard::Ptr k)
        {
            if (k->keyIsDown(input::Keyboard::A))
            {
                addLight(sceneManager, lights);
            }

            if (k->keyIsDown(input::Keyboard::R))
            {
                removeLight(lights);
            }

            if (k->keyIsDown(input::Keyboard::N))
            {
               toggleNormalMap(assets, sphere);
            }
        });

        // handle touch signals
        touchDown = canvas->touch()->touchDown()->connect([=](input::Touch::Ptr t, int, float x, float y)
        {
            x = x / canvas->width();
            y = y / canvas->height();

            // top left corner
            if (x > 0 && x < 0.25 && y > 0 && y < 0.25)
                addLight(sceneManager, lights);

            // top right corner
            if (x > 0.75 && x < 1 && y > 0 && y < 0.25)
                removeLight(lights);

            // bottom left corner
            if (x > 0 && x < 0.25 && y > 0.75 && y < 1)
                toggleNormalMap(assets, sphere);
        });
    });

    // camera init
    camera = scene::Node::create("camera")
        ->addComponent(Renderer::create())
        ->addComponent(Camera::create(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f)))
        ->addComponent(Transform::create(
            math::lookAt(math::vec3(0.f, 2.f, 0.f), math::vec3(10.f, 10.f, 10.f), math::vec3(0.f, 1.f, 0.f))
        ));
    root->addChild(camera);

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, unsigned int w, unsigned int h)
    {
        camera->component<Camera>()->projectionMatrix(math::perspective(.785f, canvas->aspectRatio(), 0.1f, 1000.f));
    });

    auto yaw        = 0.f;
    auto pitch      = float(M_PI) * .5f;
    auto minPitch   = 0.f + 1e-5;
    auto maxPitch   = float(M_PI) - 1e-5;
    auto lookAt     = math::vec3(0.f, 2.f, 0.f);
    auto distance   = 20.f;

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

    auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, float time, float deltaTime, bool shouldRender)
    {
        yaw += cameraRotationYSpeed;
        cameraRotationYSpeed *= 0.9f;

        pitch += cameraRotationXSpeed;
        cameraRotationXSpeed *= 0.9f;

        if (pitch > maxPitch)
            pitch = maxPitch;
        else if (pitch < minPitch)
            pitch = minPitch;

        camera->component<Transform>()->matrix(math::inverse(math::lookAt(
            math::vec3(
                lookAt.x + distance * std::cos(yaw) * std::sin(pitch),
                lookAt.y + distance * std::cos(pitch),
                lookAt.z + distance * std::sin(yaw) * std::sin(pitch)
            ),
            lookAt,
            math::vec3(0.f, 1.f, 0.f)
        )));

        lights->component<Transform>()->matrix(
            lights->component<Transform>()->matrix() * math::rotate(.005f, math::vec3(0.f, 1.f, 0.f))
        );

        sceneManager->nextFrame(time, deltaTime, shouldRender);
    });

    assets->loader()->load();
    canvas->run();
}
