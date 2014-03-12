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
using namespace minko::component;
using namespace minko::math;

const uint WINDOW_WIDTH		= 800;
const uint WINDOW_HEIGHT	= 600;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Application", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = SceneManager::create(canvas->context());
	
	sceneManager->assets()
		->geometry("cube", geometry::CubeGeometry::create(sceneManager->assets()->context()))
		->queue("effect/Phong.effect");

	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		auto root = scene::Node::create("root")
			->addComponent(sceneManager);

		auto camera = scene::Node::create("camera")
			->addComponent(Renderer::create(0x7f7f7fff))
			->addComponent(Transform::create(
				Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f))
			))
			->addComponent(PerspectiveCamera::create(
				(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)PI * 0.25f, .1f, 1000.f)
			);
		root->addChild(camera);
		
		auto mesh = scene::Node::create("mesh")
			->addComponent(Transform::create())
			->addComponent(Surface::create(
				assets->geometry("cube"),
				material::Material::create()
					->set("diffuseColor",	Vector4::create(1.f, 1.f, 1.f, 1.f)),
				assets->effect("effect/Phong.effect")
			));
		root->addChild(mesh);

		auto light = scene::Node::create("light")
			->addComponent(AmbientLight::create())
			->addComponent(DirectionalLight::create())
			->addComponent(Transform::create(
				Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(-2.f, -1.f, -1.f))
			));
		root->addChild(light);

		auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
		{
			camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
		});

		auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint time, uint deltaTime)
		{
			mesh->component<Transform>()->matrix()->appendRotationY(.01f);

			sceneManager->nextFrame();
		});

		canvas->run();
	});

	sceneManager->assets()->load();

	return 0;
}
