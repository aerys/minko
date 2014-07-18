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
using namespace minko::math;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Working with normal maps", 800, 600);

	auto sceneManager = SceneManager::create(canvas->context());

	// add the jpeg parser to load textures
	// add the Phong effect
	sceneManager->assets()->loader()		
		->queue("texture/diffuseMap.jpg")
		->queue("texture/normalMap.jpg")
		->queue("effect/Phong.effect")
		->options()->registerParser<file::JPEGParser>("jpg");

	auto _ = sceneManager->assets()->loader()->complete()->connect([=](file::Loader::Ptr loader)
	{
		auto root = scene::Node::create("root")
			->addComponent(sceneManager);

		auto phongMaterial = material::PhongMaterial::create();

		phongMaterial->diffuseMap(sceneManager->assets()->texture("texture/diffuseMap.jpg"));
		phongMaterial->normalMap(sceneManager->assets()->texture("texture/normalMap.jpg"));

		auto mesh = scene::Node::create("mesh")
			->addComponent(Transform::create(Matrix4x4::create()->prependScale(1.1)))
			->addComponent(Surface::create(
			geometry::SphereGeometry::create(sceneManager->assets()->context(), 20U),
			phongMaterial,
			sceneManager->assets()->effect("effect/Phong.effect")
			));

		auto camera = scene::Node::create("camera")
			->addComponent(Renderer::create(0x00000000))
			->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::create(), Vector3::create(0.0f, 3.f, 3.3f))
			))
			->addComponent(PerspectiveCamera::create(800.f / 600.f, float(M_PI) * 0.25f, .1f, 1000.f));

		auto ambientLight = scene::Node::create("ambientLight")
			->addComponent(AmbientLight::create(0.25f));
		ambientLight->component<AmbientLight>()->color(Vector4::create(1.0f, 1.0f, 1.0f, 1.0f));
		root->addChild(ambientLight);

		auto spotLight = scene::Node::create("SpotLight")
			->addComponent(SpotLight::create(0.6f, 0.78f, 20.f))
			->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(4.f, 6.f, 2.5f))));
		spotLight->component<SpotLight>()->diffuse(0.4f);

		root->addChild(camera);
		root->addChild(mesh);
		root->addChild(spotLight);

		auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
		{
			sceneManager->nextFrame(t, dt);
		});

		canvas->run();
	});
	sceneManager->assets()->loader()->load();
	return 0;
}
