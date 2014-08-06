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
#include "minko/MinkoFX.hpp"
#include "minko/MinkoSerializer.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const std::string MODEL_FILENAME = "model/pirate.scene";
const float WIDTH = 1024;
const float HEIGHT = 1024;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Clone", WIDTH, HEIGHT);

	auto sceneManager = SceneManager::create(canvas->context());

	// setup assets 
	//sceneManager->assets()->loader()->options()->resizeSmoothly(true);
	sceneManager->assets()->loader()->options()->generateMipmaps(true);
	//sceneManager->assets()->loader()->options()->disposeVertexBufferAfterLoading(true);
	//sceneManager->assets()->loader()->options()->disposeTextureAfterLoading(true);
	//sceneManager->assets()->loader()->options()->disposeIndexBufferAfterLoading(true);
	sceneManager->assets()->loader()->options()
		->registerParser<file::SceneParser>("scene")
		->registerParser<file::PNGParser>("png");

	auto fxLoader = file::Loader::create(sceneManager->assets()->loader())
		->queue("effect/Phong.effect")
		//->queue("effect/FXAA/FXAA.effect")
		->queue("effect/Basic.effect");
		//->queue("effect/VertexNormal.effect")
		//->queue("texture/noise.png")
		//->queue("effect/Depth/Depth.effect");

	auto fxComplete = fxLoader->complete()->connect([&](file::Loader::Ptr l)
	{
		sceneManager->assets()->loader()->options()->effect(sceneManager->assets()->effect("effect/Phong.effect"));
		sceneManager->assets()->loader()->options()->disposeTextureAfterLoading(false);
		sceneManager->assets()->loader()->queue(MODEL_FILENAME);
		sceneManager->assets()->loader()->load();
	});

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	/*auto mesh = scene::Node::create("mesh")
		->addComponent(Transform::create());

	auto mesh2 = scene::Node::create("mesh2")
		->addComponent(Transform::create());*/

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
		Matrix4x4::create()->lookAt(Vector3::create(0.f, 0.f, 0.f), Vector3::create(0.f, .0f, 20.f))
		))
		->addComponent(PerspectiveCamera::create(WIDTH / HEIGHT, (float)PI * 0.25f, .1f, 1000.f));
	root->addChild(camera);


	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		auto cubeGeometry = geometry::CubeGeometry::create(sceneManager->assets()->context());
		auto sphereGeometry = geometry::SphereGeometry::create(sceneManager->assets()->context(), 30, 30);

		

		sceneManager->assets()->geometry("cubeGeometry", cubeGeometry);
		sceneManager->assets()->geometry("sphereGeometry", sphereGeometry);

	/*	mesh->addComponent(Surface::create(
			sceneManager->assets()->geometry("cubeGeometry"),
			material::Material::create(),
			sceneManager->assets()->effect("effect/Basic.effect")
			));

		mesh2->addComponent(Surface::create(
			sceneManager->assets()->geometry("sphereGeometry"),
			material::Material::create(),
			sceneManager->assets()->effect("effect/Basic.effect")
			));

		mesh->component<Transform>()->matrix()->appendTranslation(2.5f, 0.f, 0.f);
		mesh2->component<Transform>()->matrix()->appendTranslation(-2.5f, 0.f, 0.f);*/


		//root->addChild(mesh);
		//root->addChild(mesh2);
		auto armor = sceneManager->assets()->symbol(MODEL_FILENAME);
		//auto armor2 = armor->clone(CloneOption::DEEP);
		//mesh->component<Transform>()->matrix()->prependTranslation(0, 3, 0);
		//armor2->component<Transform>()->matrix()->prependTranslation(2, 0, 0);
		root->addChild(armor);

		if (!armor->hasComponent<Transform>())
			armor->addComponent(Transform::create());
		//root->addChild(armor2);
		/*int c = 0;
		for (int x = 0; x < 6; x=x+2)
		{
			for (int z = 0; z < 5; z++)
			{
				auto clone = armor->clone(CloneOption::DEEP);
				clone->component<Transform>()->matrix()->prependTranslation(x, 0, z);
				root->addChild(clone);
				c++;
				std::cout << "bot no : " << c << std::endl;
			}
		}*/

		auto meshes = scene::NodeSet::create(sceneManager->assets()->symbol(MODEL_FILENAME))->descendants(false, false)->where([=](scene::Node::Ptr node)
		{
			return node->hasComponent<Surface>();
		});


	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
		auto width = math::clp2(w);
		auto height = math::clp2(h);
	
	});

	auto yaw = 0.f;
	auto pitch = (float)PI * .5f;
	auto minPitch = 0.f + 1e-5;
	auto maxPitch = (float)PI - 1e-5;
	auto lookAt = Vector3::create(0.f, .8f, 0.f);
	auto distance = 10.f;

	// handle mouse signals
	auto mouseWheel = canvas->mouse()->wheel()->connect([&](input::Mouse::Ptr m, int h, int v)
	{
		distance += (float)v / 2.f;
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
		auto test = root;

		yaw += cameraRotationYSpeed;
		cameraRotationYSpeed *= 0.9f;

		pitch += cameraRotationXSpeed;
		cameraRotationXSpeed *= 0.9f;
		if (pitch > maxPitch)
			pitch = maxPitch;
		else if (pitch < minPitch)
			pitch = minPitch;

		camera->component<Transform>()->matrix()->lookAt(
			lookAt,
			Vector3::create(
			lookAt->x() + distance * cosf(yaw) * sinf(pitch),
			lookAt->y() + distance * cosf(pitch),
			lookAt->z() + distance * sinf(yaw) * sinf(pitch)
			)
			);

		//mesh->component<Transform>()->matrix()->prependRotationY(0.0005f * deltaTime);
		//mesh2->component<Transform>()->matrix()->prependRotationY(0.0005f * deltaTime);
		//sceneManager->assets()->symbol(MODEL_FILENAME)->component<Transform>()->matrix()->prependRotationY(0.0005f * deltaTime);

		sceneManager->nextFrame(time, deltaTime);		
	});

	fxLoader->load();
	canvas->run();

	return 0;
}


