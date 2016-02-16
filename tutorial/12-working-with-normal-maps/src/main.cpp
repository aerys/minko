/*
Copyright (c) 2016 Aerys

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
#include "minko/MinkoJPEG.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

const std::string DIFFUSEMAP = "texture/diffuseMap.jpg";
const std::string NORMALMAP = "texture/normalMap.jpg";

int	main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Working with the NormalMap", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()->options()
		->registerParser<file::JPEGParser>("jpg");

	sceneManager->assets()->loader()
		->queue(DIFFUSEMAP)
		->queue(NORMALMAP)
		->queue("effect/Phong.effect")
		;

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x00000000))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 3.f, 3.3f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)M_PI * 0.25f, .1f, 1000.f));

	auto spotLight = scene::Node::create("spotLight")
		->addComponent(SpotLight::create(.6f, .78f, 20.f))
		->addComponent(Transform::create(inverse(lookAt(vec3(4.f, 6.f, 2.5f), vec3(), vec3(0.f, 1.f, 0.f)))));
	spotLight->component<SpotLight>()->diffuse(0.4f);

	auto ambientLight = scene::Node::create("ambientLight")
		->addComponent(AmbientLight::create(0.25f));
	ambientLight->component<AmbientLight>()->color(vec3(1.f, 1.f, 1.f));

	root->addChild(camera);
	root->addChild(spotLight);
	root->addChild(ambientLight);

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto phongMaterial = material::PhongMaterial::create();

		phongMaterial->diffuseMap(sceneManager->assets()->texture(DIFFUSEMAP));
		//phongMaterial->normalMap(sceneManager->assets()->texture(NORMALMAP));

		auto mesh = scene::Node::create("mesh")
			->addComponent(Transform::create(scale(vec3(1.1f))))
			->addComponent(Surface::create(
				geometry::SphereGeometry::create(sceneManager->assets()->context(), 20U),
				phongMaterial,
				sceneManager->assets()->effect("effect/Phong.effect")
				));

		root->addChild(mesh);
	});

	sceneManager->assets()->loader()->load();

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
	{
		sceneManager->nextFrame(t, dt);
	});

	canvas->run();

	return 0;
}
