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
using namespace minko::math;

#define POST_PROCESSING 0
#define WINDOW_WIDTH  	800
#define WINDOW_HEIGHT 	600

scene::Node::Ptr camera = nullptr;

int main(int argc, char** argv)
{
	auto canvas		= Canvas::create("Minko Examples - Frustum", WINDOW_WIDTH, WINDOW_HEIGHT);
	
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

		for (uint i = 0; i < 25; ++i)
		{
			auto mesh = scene::Node::create("mesh")
				->addComponent(Transform::create(math::Matrix4x4::create()
					->appendTranslation(rand() % 200 - 100.f, rand() % 200 - 100.f, rand() % 200 - 100.f)
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
			->addComponent(Culling::create(math::Frustum::create(), "camera.worldToScreenMatrix"))
			->addComponent(Transform::create(
				Matrix4x4::create()->lookAt(
					Vector3::create(0.f, 0.f), 
					Vector3::create(rand() % 200 - 100.f, rand() % 200 - 100.f, rand() % 200 - 100.f))
			));

		root->addChild(camera);
		root->addChild(cubeGroup);
		
		auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, unsigned int width, unsigned int height)
		{
			camera->component<PerspectiveCamera>()->aspectRatio((float)width / (float)height);
		});

		auto enterFrame = canvas->enterFrame()->connect([&](AbstractCanvas::Ptr canvas, uint time, uint deltaTime)
		{
			camera->component<Transform>()->matrix()->lock()->appendRotationY(0.02f)->appendRotationZ(-0.014f)->unlock();
			sceneManager->nextFrame();
			std::cout << "Num drawCalls : " << camera->component<Renderer>()->numDrawCalls() << std::endl;
		});
		
		canvas->run();
	});

	sceneManager->assets()->load();

	exit(EXIT_SUCCESS);
}
