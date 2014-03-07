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
using namespace minko::math;
using namespace minko::component;

const uint WINDOW_WIDTH = 800;
const uint WINDOW_HEIGHT = 600;

int
main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Using external GLSL code in effect files", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas->context());

	sceneManager->assets()
		->queue("effect/MyCustomEffect.effect");
	auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
	{
		auto myCustomEffect = assets->effect("effect/MyCustomEffect.effect");

		auto root = scene::Node::create("root")
			->addComponent(sceneManager)
			->addComponent(Renderer::create(0x7f7f7fff));

		auto cube = scene::Node::create("cube")
			->addComponent(Surface::create(
			geometry::CubeGeometry::create(assets->context()),
			material::BasicMaterial::create()->diffuseColor(Vector4::create(0.f, 0.f, 1.f, 1.f)),
			myCustomEffect
			));
		root->addChild(cube);

		auto modelToWorldMatrix = Matrix4x4::create()->translation(0.f, 0.f, -5.f);

		myCustomEffect->setUniform("uModelToWorldMatrix", modelToWorldMatrix);
		myCustomEffect->setUniform("uViewMatrix", Matrix4x4::create());
		myCustomEffect->setUniform("uProjectionMatrix", Matrix4x4::create()->perspective((float)PI * 0.25f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, .1f, 1000.f));

		myCustomEffect->setUniform("uColor", Vector4::create(0.f, 0.f, 1.f, 1.f));

		auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint t, float dt)
		{
			modelToWorldMatrix->prependRotationY(0.01f);
			myCustomEffect->setUniform("uModelToWorldMatrix", modelToWorldMatrix);

			sceneManager->nextFrame();
		});

		canvas->run();
	});

	sceneManager->assets()->load();

	return 0;
}
