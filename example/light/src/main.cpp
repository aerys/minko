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
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;

#define WINDOW_WIDTH  	800
#define WINDOW_HEIGHT 	600

scene::Node::Ptr camera = nullptr;

Signal<input::Keyboard::Ptr>::Slot keyDown;

scene::Node::Ptr
createPointLight(math::vec3 color, math::vec3 position, file::AssetLibrary::Ptr assets)
{
	static int lightId = 0;

	auto pointLight = scene::Node::create("pointLight")
		->addComponent(PointLight::create(.3f))
		->addComponent(Transform::create(math::translate(math::mat4(1.f), position)))
		->addComponent(Surface::create(
			assets->geometry("quad"),
			material::Material::create()
				->set("diffuseMap",		assets->texture("texture/sprite-pointlight.png"))
				->set("diffuseTint",	math::vec4(color, 1.f)),
			assets->effect("effect/Sprite.effect")
		));

	pointLight->component<PointLight>()->color(color);
	pointLight->component<PointLight>()->diffuse(.1f);
	pointLight->component<PointLight>()->layoutMask(lightId % 2 == 0 ? 1<<2 : 1);

	return pointLight;
}

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Light", WINDOW_WIDTH, WINDOW_HEIGHT);

	canvas->context()->errorsEnabled(true);

	auto sceneManager		= SceneManager::create(canvas->context());
	auto root				= scene::Node::create("root")->addComponent(sceneManager);
	auto assets				= sceneManager->assets();
	auto lights 			= scene::Node::create("lights");
	auto sphereGeometry		= geometry::SphereGeometry::create(assets->context(), 32, 32, true);
	auto sphereMaterial		= material::PhongMaterial::create()
		->shininess(16.f)
		->specularColor({ 1.0f, 1.0f, 1.0f, 1.0f })
		->diffuseColor({ 1.f, 1.f, 1.f, 1.f });

	std::cout << "Press [SPACE]\tto toogle normal mapping\nPress [A]\tto add random light\nPress [R]\tto remove random light" << std::endl;

	sphereGeometry->computeTangentSpace(false);

	// setup assets
	assets
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->geometry("quad", geometry::QuadGeometry::create(sceneManager->assets()->context()))
		->geometry("sphere", sphereGeometry);

	assets->loader()->options()
		->generateMipmaps(true)
		->registerParser<file::PNGParser>("png");

	assets->loader()
		->queue("texture/normalmap-cells.png")
		->queue("texture/sprite-pointlight.png")
		->queue("effect/VertexNormal.effect")
		->queue("effect/Basic.effect")
		->queue("effect/Sprite.effect")
		->queue("effect/Phong.effect");

	auto _ = assets->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		// ground
		auto ground = scene::Node::create("ground")
			->layouts(1 << 2 | 1)
			->addComponent(Surface::create(
				sceneManager->assets()->geometry("quad"),
				material::Material::create()
					->set("diffuseColor", math::vec4(1.f)),
				sceneManager->assets()->effect("effect/Phong.effect")
			))
			->addComponent(Transform::create(
				math::rotate(math::scale(math::mat4(1.f), math::vec3(300.f)), -1.57f, math::vec3(1.f, 0.f, 0.f))
			));
		//root->addChild(ground);

		// sphere
		auto sphere = scene::Node::create("sphere")
			->addComponent(Surface::create(
				assets->geometry("sphere"),
				sphereMaterial,
				assets->effect("effect/Phong.effect")
			))
			->addComponent(Transform::create(
				math::scale(math::mat4(1.f), math::vec3(4.f))
			));
		std::cout << std::to_string(sphere->component<Transform>()->matrix() * math::vec4(0.f, 0.f, 0.f, 1.f)) << std::endl;
		root->addChild(sphere);

		// spotLight
		auto spotLight = scene::Node::create("spotLight")
			->addComponent(SpotLight::create(.2f, .6f))
			->addComponent(Transform::create(math::inverse(math::lookAt(
				math::vec3(20.f, 30.f, 20.f),
				math::vec3(0.f),
				math::vec3(0.f, 1.f, 0.f)
			))));
		spotLight->component<SpotLight>()->diffuse(.2f);
		lights->addChild(spotLight);

		lights->addComponent(Transform::create());
		//root->addChild(lights);

		// std::cout << "test" << std::to_string(
		// 	math::vec3(0.f, 0.f, 1.f)
		// 	* math::mat3(math::rotate(math::mat4(1.f), (float)PI * .5f, math::vec3(0.f, 1.f, 0.f)))
		// ) << std::endl;

		// handle keyboard signals
		keyDown = canvas->keyboard()->keyDown()->connect([=](input::Keyboard::Ptr k)
		{
			if (k->keyIsDown(input::Keyboard::A))
			{
				const auto MAX_NUM_LIGHTS = 400;

				if (lights->children().size() == MAX_NUM_LIGHTS)
				{
					std::cout << "cannot add more lights" << std::endl;
					return;
				}

				auto r = rand() / (float)RAND_MAX;
				auto theta = 2.0f * (float)PI *  r;
				auto color = math::rgbColor(math::vec3((theta / ((float)PI * 2.f)) * 360.f, 1.f, .5f));
				auto pos = math::vec3(
					cosf(theta) * 5.f + rand() / ((float)RAND_MAX * 3.f),
					2.5f + rand() / (float)RAND_MAX,
					sinf(theta) * 5.f + rand() / ((float)RAND_MAX * 3.f)
				);

				lights->addChild(createPointLight(color, pos, sceneManager->assets()));

				std::cout << lights->children().size() << " lights" << std::endl;
			}
			if (k->keyIsDown(input::Keyboard::R))
			{
				if (lights->children().size() == 0)
					return;

				lights->removeChild(lights->children().back());
				std::cout << lights->children().size() << " lights" << std::endl;
			}

			if (k->keyIsDown(input::Keyboard::S))
			{
				auto sphereLayout = sphere->layouts();
				sphere->layouts(sphereLayout == 1 ? 1 << 2 | 1 : 1);
			}

			if (k->keyIsDown(input::Keyboard::D))
			{
				auto light = lights->children()[0];
				auto mask = light->component<PointLight>()->layoutMask();

				light->component<PointLight>()->layoutMask(mask == 1 ? 1 << 2 : 1);
			}

			if (k->keyIsDown(input::Keyboard::SPACE))
			{
				auto data = sphere->component<Surface>()->material();
				bool hasNormalMap = data->hasProperty("normalMap");

				std::cout << "mesh does" << (!hasNormalMap ? " not " : " ")
					<< "have a normal map:\t" << (hasNormalMap ? "remove" : "add")
					<< " it" << std::endl;

				if (hasNormalMap)
					data->unset("normalMap");
				else
					data->set("normalMap", assets->texture("texture/normalmap-cells.png"));
			}
			if (k->keyIsDown(input::Keyboard::UP))
				camera->component<Transform>()->matrix(math::translate(
					camera->component<Transform>()->matrix(),
					{ 0.f, 0.f, -1.f }
				));
			if (k->keyIsDown(input::Keyboard::DOWN))
				camera->component<Transform>()->matrix(math::translate(
					camera->component<Transform>()->matrix(),
					{ 0.f, 0.f, 1.f }
				));
		});
	});

	// camera init
	camera = scene::Node::create("camera")
		->addComponent(Renderer::create())
		->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT))
		->addComponent(Transform::create(math::inverse(math::lookAt(
			math::vec3(10.f),
			math::vec3(0.f, 2.f, 0.f),
			math::vec3(0.f, 1.f, 0.f)
		))));
	root->addChild(camera);

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, unsigned int width, unsigned int height)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)width / (float)height);
	});

	auto yaw = 0.f;
	auto pitch = (float)PI * .5f;
	auto minPitch = 0.f + 1e-5;
	auto maxPitch = (float)PI * .5f;//(float)PI - 1e-5;
	auto lookAt = math::vec3(0.f, 2.f, 0.f);
	auto distance = 20.f;

	// handle mouse signals
	auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
	{
		distance += (float)v / 10.f;
	});

	Signal<input::Mouse::Ptr, int, int>::Slot mouseMove;
	auto cameraRotationXSpeed = 0.f;
	auto cameraRotationYSpeed = 0.f;

	auto mouseDown = canvas->mouse()->leftButtonDown()->connect([&](input::Mouse::Ptr m)
	{
		mouseMove = canvas->mouse()->move()->connect([&](input::Mouse::Ptr, int dx, int dy)
		{
			cameraRotationYSpeed = (float)dx * .01f;
			cameraRotationXSpeed = (float)dy * -.01f;
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
			{
				lookAt.x + distance * cosf(yaw) * sinf(pitch),
				lookAt.y + distance * cosf(pitch),
				lookAt.z + distance * sinf(yaw) * sinf(pitch)
			},
			lookAt,
			{ 0.f, 1.f, 0.f }
		)));

		//std::cout << std::to_string(camera->component<Transform>()->modelToWorldMatrix()) << std::endl;

		/*
		lights->component<Transform>()->matrix(
			math::rotate(lights->component<Transform>()->matrix(), .005f, math::vec3(0.f, 1.f, 0.f))
		);
		*/

		sceneManager->nextFrame(time, deltaTime);
	});

	assets->loader()->load();

	canvas->run();
}
