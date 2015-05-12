/*
Copyright (c) 2013 Aerys

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

const std::string   ENVMAP          = "cloudySea";
const std::string   MAP_DIFFUSE     = "texture/" + ENVMAP + "/" + ENVMAP + "-diffuse.jpg";
const std::string   MAP_RADIANCE    = "texture/" + ENVMAP + "/" + ENVMAP + "-radiance.jpg";
const std::string   MAP_IRRADIANCE  = "texture/" + ENVMAP + "/" + ENVMAP + "-irradiance.jpg";

int main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - PBR", 800, 600);
    auto sceneManager = SceneManager::create(canvas);
    auto root = scene::Node::create("root")->addComponent(sceneManager);
    auto assets = sceneManager->assets();
    auto context = canvas->context();

    context->errorsEnabled(true);
    std::cout << context->driverInfo() << std::endl;

	// setup assets
    auto options = assets->loader()->options();

	options
		->resizeSmoothly(true)
		->registerParser<file::JPEGParser>("jpg");

    assets->loader()
        ->queue(MAP_DIFFUSE, options->clone()->generateMipmaps(true))
        ->queue(MAP_IRRADIANCE)
        ->queue(MAP_RADIANCE, options->clone()->parseMipMaps(true))
        ->queue("texture/ground.jpg")
        ->queue("effect/Basic.effect")
        ->queue("effect/Skybox/Skybox.effect")
		->queue("effect/PBR.effect");

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x2f2f2fff))
        ->addComponent(Transform::create(math::inverse(
			math::lookAt(math::vec3(1.f, 1.f, 0.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f))
		)))
		->addComponent(PerspectiveCamera::create(800.f / 600.f, 0.785f, 0.1f, 50.f));
    root->addChild(camera);

    auto light = scene::Node::create("light")
        ->addComponent(AmbientLight::create(0.4f))
        ->addComponent(DirectionalLight::create(1.f))
        ->addComponent(Transform::create(math::inverse(
            math::lookAt(math::vec3(1.f, 5.f, 0.f), math::vec3(0.f), math::vec3(0.f, 1.f, 0.f))
        )));
    light->component<DirectionalLight>()->enableShadowMapping(512);
    root->addChild(light);

	auto _ = assets->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
        auto skybox = scene::Node::create("skybox")
            ->addComponent(Surface::create(
                geometry::CubeGeometry::create(context),
                material::Material::create({
                    { "diffuseLatLongMap", assets->texture(MAP_DIFFUSE)->sampler() },
                    { "gammaCorrection", 2.2f}
                }),
                assets->effect("effect/Skybox/Skybox.effect")
            ));
        root->addChild(skybox);

        auto groundMaterial = material::Material::create({
            { "roughness",          1.f },
            { "metalness",          0.0f },
            { "specularColor",      math::vec4(0.f, 0.f, 0.f, 1.f) },
            { "albedoMap",          assets->texture("texture/ground.jpg")->sampler() },
            { "diffuseMap",          assets->texture("texture/ground.jpg")->sampler() },
            { "albedoColor",        math::vec4(.8f, .8f, .8f, 1.f) },
            { "irradianceMap",      assets->texture(MAP_IRRADIANCE)->sampler() },
            { "radianceMap",        assets->texture(MAP_RADIANCE)->sampler() },
            { "gammaCorrection",    2.2f }
        });
        auto ground = scene::Node::create("ground", scene::BuiltinLayout::DEFAULT | 256)
            ->addComponent(Surface::create(
                geometry::QuadGeometry::create(sceneManager->canvas()->context()),
                groundMaterial,
                assets->effect("effect/PBR.effect")
            ))
            ->addComponent(Transform::create(
                math::translate(math::vec3(0.f, -0.5f, 0.f))
                * math::rotate(-(float)M_PI_2, math::vec3(1.f, 0.f, 0.f))
                * math::scale(math::vec3(20.f))
            ));
        root->addChild(ground);

        auto sphereGeom = geometry::SphereGeometry::create(sceneManager->canvas()->context(), 40, 40)->computeTangentSpace(false);
        auto meshes = scene::Node::create("meshes");
        const int numSpheres = 10;

        for (auto i = 0; i < numSpheres; i++)
        {
            for (auto j = 0; j < numSpheres; j++)
            {
                auto mesh = scene::Node::create("mesh", scene::BuiltinLayout::DEFAULT | 256);
                auto mat = material::Material::create({
                    { "gammaCorrection",    2.2f },
                    { "albedoColor",        math::vec4(1.f, 1.f, 1.f, 1.f) },
                    { "specularColor",      math::vec4(1.f, 1.f, 1.f, 1.f) },
                    { "metalness",          (float)j / (float)(numSpheres - 1) },
                    { "roughness",          (float)i / (float)(numSpheres - 1) },
                    { "irradianceMap",      assets->texture(MAP_IRRADIANCE)->sampler() },
                    { "radianceMap",        assets->texture(MAP_RADIANCE)->sampler() }//,
                });

                mesh->addComponent(Surface::create(sphereGeom, mat, assets->effect("effect/PBR.effect")));
                mesh->addComponent(Transform::create(math::translate(math::vec3(
                    (-(float)(numSpheres - 1) * .5f + (float)i) * 1.25f,
                    0.f,
                    (-(float)(numSpheres - 1) * .5f + (float)j) * 1.25f
                ))));
                meshes->addChild(mesh);
            }
        }
        root->addChild(meshes);

        auto yaw = 0.f;
        auto pitch = float(M_PI) * .5f;
        auto minPitch = 0.f + 0.1f;
        auto maxPitch = float(M_PI) * .5f - .1f;
        auto lookAt = math::vec3(0.f, 0.f, 0.f);
        auto distance = 10.f;

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

        auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
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

            light->component<DirectionalLight>()->computeShadowProjection(
                camera->component<PerspectiveCamera>()->viewMatrix(),
                camera->component<PerspectiveCamera>()->projectionMatrix()
            );

            sceneManager->nextFrame(time, deltaTime);
        });

    	canvas->run();
	});

    auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
	});

	sceneManager->assets()->loader()->load();

	return 0;
}
