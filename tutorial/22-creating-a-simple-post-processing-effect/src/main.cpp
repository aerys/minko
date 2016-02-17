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

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

int
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Creating a simple post-processing effect", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()
		->queue("effect/Basic.effect")
		->queue("effect/Desaturate.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(PerspectiveCamera::create(canvas->aspectRatio()))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 0.f, 3.f), vec3(), vec3(0.f, 1.f, 0.f)))));

	root->addChild(camera);

	auto cube = scene::Node::create("cube");

	render::Effect::Ptr ppFx;
	auto ppRenderer = Renderer::create();
	auto ppTarget = render::Texture::create(sceneManager->assets()->context(), clp2(WINDOW_WIDTH), clp2(WINDOW_HEIGHT), false, true);
	ppTarget->upload();

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		cube
			->addComponent(Transform::create())
			->addComponent(Surface::create(
                        geometry::CubeGeometry::create(sceneManager->assets()->context()),
						material::BasicMaterial::create()->diffuseColor(vec4(0.f, 0.f, 1.f, 1.f)),
						sceneManager->assets()->effect("effect/Basic.effect")
					));
		
		root->addChild(cube);

		ppFx = sceneManager->assets()->effect("effect/Desaturate.effect");

		if (!ppFx)
			throw std::logic_error("The post-processing effect has not been loaded.");

		ppFx->data()->set("backBuffer", ppTarget->sampler());

		auto ppScene = scene::Node::create()
			->addComponent(ppRenderer)
			->addComponent(Surface::create(
						geometry::QuadGeometry::create(sceneManager->assets()->context()),
						material::Material::create(),
						ppFx
					));
	});

	auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, math::uint width, math::uint height)
	{
		camera->component<PerspectiveCamera>()->aspectRatio((float)width / (float)height);

		ppTarget = render::Texture::create(sceneManager->assets()->context(), clp2(width), clp2(height), false, true);
		ppTarget->upload();
		ppFx->data()->set("backBuffer", ppTarget->sampler());
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
	{
		auto transform = cube->component<Transform>();
		transform->matrix(transform->matrix() * rotate(.01f, vec3(0.f, 1.f, 0.f)));

		sceneManager->nextFrame(t, dt, ppTarget);
		ppRenderer->render(sceneManager->assets()->context());
	});

	sceneManager->assets()->loader()->load();
	
	canvas->run();

	return 0;
}
