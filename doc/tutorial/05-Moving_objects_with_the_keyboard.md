In this tutorial, we will see how to catch keyboard inputs and use them to move objects.

Step 1: Catching keyboards inputs
---------------------------------

```cpp
auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k) {

 std::cout << "key down!" << std::endl;

});
```


The argument passed to the callbacks of the `Canvas::keyDown()` signal is an array where each cell indicates whether a specific key is down or not.

```cpp
auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k) {

 std::cout << "The 'space' key is" << (k->keyIsDown(input::Keyboard::ScanCode::SPACE) ? "" : " not") << " down" << std::endl;

});
```


Step 2: Moving objects
----------------------

To move our object, we will simply use the `Matrix4x4::appendTranslation()` method with argument values depending on which key is actually down:

```cpp
auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k) {

   if (k->keyIsDown(input::Keyboard::ScanCode::LEFT))
       cube->component<Transform>()->matrix()->appendTranslation(-0.1f);
   if (k->keyIsDown(input::Keyboard::ScanCode::RIGHT))
       cube->component<Transform>()->matrix()->appendTranslation(0.1f);

});
```


Final code
----------

```cpp
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

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int	main(int argc, char** argv)
{
	auto canvas = Canvas::create("Tutorial - Moving objets with keyboard", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);
		sceneManager->assets()->loader()->queue("effect/Basic.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)M_PI * 0.25f, .1f, 1000.f));

	root->addChild(camera);

	auto cube = scene::Node::create("cube");

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto cubeEffect = sceneManager->assets()->effect("effect/Basic.effect");
		auto cubeMaterial = material::BasicMaterial::create();
		cubeMaterial->diffuseColor(vec4(0.f, 0.f, 1.f, 1.f));
		auto cubeGeometry = geometry::CubeGeometry::create(sceneManager->assets()->context());

		cube->addComponent(Surface::create(cubeGeometry, cubeMaterial, cubeEffect));
		cube->addComponent(Transform::create(translate(vec3(0.f, 0.f, -5.f))));

		root->addChild(cube);
	});

	sceneManager->assets()->loader()->load();

	auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k) {
		auto transform = cube->component<Transform>();

		if (k->keyIsDown(input::Keyboard::LEFT))
			transform->matrix(translate(vec3(-.1f, 0.f, 0.f)) * transform->matrix());
		if (k->keyIsDown(input::Keyboard::RIGHT))
			transform->matrix(translate(vec3(.1f, 0.f, 0.f)) * transform->matrix());
	});

	auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
	{
		auto transform = cube->component<Transform>();
		transform->matrix(transform->matrix() * rotate(.01f, vec3(0.f, 1.f, 0.f)));

		sceneManager->nextFrame(t, dt);
	});

	canvas->run();

	return 0;
}
```
