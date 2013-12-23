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
#include "minko/math/OctTree.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

#define POST_PROCESSING 0
#define WINDOW_WIDTH  	800
#define WINDOW_HEIGHT 	600

scene::Node::Ptr camera = nullptr;

int main(int argc, char** argv)
{
	auto canvas		= Canvas::create("Minko Examples - Light", WINDOW_WIDTH, WINDOW_HEIGHT);
	
	canvas->context()->errorsEnabled(true);

	const clock_t startTime	= clock();

	auto sceneManager		= SceneManager::create(canvas->context());
	auto root				= scene::Node::create("root")->addComponent(sceneManager);
	
	auto quadTreeRoot		= scene::Node::create("quadTreeRoot");
	auto cubeGroup			= scene::Node::create("cubeGroup");

	cubeGroup->addComponent(component::Transform::create());

	// setup assets
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->queue("effect/Basic.effect");

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		std::shared_ptr<material::BasicMaterial> material = material::BasicMaterial::create()->diffuseColor(0xFF00FFFF);

		for (uint i = 0; i < 200; ++i)
		{
			auto mesh = scene::Node::create("mesh")
				->addComponent(Transform::create(math::Matrix4x4::create()
					->appendTranslation(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50)
					))
				->addComponent(Surface::create(
					assets->geometry("cube"),
					material,
					assets->effect("effect/Basic.effect")
				));
			mesh->layouts(mesh->layouts() | (1u << 17)); // static layout

			cubeGroup->addChild(mesh);
		}

		// camera init
		camera = scene::Node::create("camera")
			->addComponent(Renderer::create())
			->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT))
			->addComponent(FrustumCulling::create())
			->addComponent(Transform::create(
				Matrix4x4::create()->lookAt(Vector3::create(0.f, 2.f), Vector3::create(10.f, 10.f, 10.f))
			));

		root->addChild(camera);
		root->addChild(cubeGroup);
		
		auto resized = canvas->resized()->connect([&](Canvas::Ptr canvas, unsigned int width, unsigned int height)
		{
			camera->component<PerspectiveCamera>()->aspectRatio((float)width / (float)height);
		});

		auto yaw = 0.f;
		auto pitch = PI * .5f;
		auto roll = 0.f;
		auto minPitch = 0.f + 1e-5;
		auto maxPitch = (float)PI - 1e-5;
		auto lookAt = Vector3::create(0.f, 2.f, 0.f);
		auto distance = 150.f;

		// handle mouse signals
		auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
		{
			distance += (float)v ;
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

		auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint time, uint deltaTime)
		{
			yaw += cameraRotationYSpeed;
			cameraRotationYSpeed *= 0.9f;

			pitch += cameraRotationXSpeed;
			cameraRotationXSpeed *= 0.9f;
			if (pitch > maxPitch)
				pitch = maxPitch;
			else if (pitch < minPitch)
				pitch = minPitch;

			
			camera->component<Transform>()->transform()->lookAt(
				lookAt,
				Vector3::create(
					lookAt->x() + distance * cosf(yaw) * sinf(pitch),
					lookAt->y() + distance * cosf(pitch),
					lookAt->z() + distance * sinf(yaw) * sinf(pitch)
				)
			);

			sceneManager->nextFrame();
		});
		
		canvas->run();
	});

	sceneManager->assets()->load();

	exit(EXIT_SUCCESS);
}
