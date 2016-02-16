In this tutorial, we will see how to move objects programmatically. This tutorial will teach you both how to translate objects and how to set their position directly.

The code for this tutorial is based on the one described in the [Hello cube!](../tutorial/01-Hello_cube!.md) tutorial.

Step 1: Creating a 3D transform
-------------------------------

In order to move a scene `Node` in the 3D space, we must first make sure this very node has a 3D transform. Indeed: Minko is very modular and scene `Node` do not necessarily have a 3D transform. This is very important because it bring a much more modular and lightweight scene graph API.

Minko stores the 3D transform of a scene `Node` in a `Transform` component. Such component can easily be created using the `Transform::create()` static method:

```cpp
auto transform = component::Transform::create();
```


Step 2: Assigning the transform to a node
-----------------------------------------

We can then assign the `Transform` to any scene node using the `Node::addComponent()` method:

```cpp
auto transform = component::Transform::create();

node->addComponent(transform)
```


Note that you cannot add two `Transform` on the same component because it doesn't make sense. You might want to check the target node does not already have a `Transform` component before adding the new one. The `Node::hasComponent()` method will do exactly that:

```cpp
if (!node->hasComponent<Transform>())

 node->addComponent(component::Transform::create());

```


When it is assigned, our `Transform` component can also be accessed from the scene `Node` itself using the `Node::component()` method:

```cpp
auto transform = node->component<Transform>();
```


Step 3: Modifying the transform
-------------------------------

Now that our node has a `Transform`, we can modify it to translate the node in our 3D scene! This is done by using the `Transform::matrix()` property that returns a `math::mat4` object.From there we can do two different things: apply a translation - meaning add a translation operation to an existing 3D transform - or set the position of the node directly.

### Apply a 3D translation

We have to apply a translation matrix `math::translate(math::vec3)` on our matrix. The following code will translate our node on the x axis:

```cpp
auto transform = node->component<Transform>();
transform->matrix(translate(vec3(42.f, 0.f, 0.f)) * transform->matrix());
```

For instance, if `node` was located in (0, 0, 0), it should now be in (42, 0, 0). This code has the effect of an appending translation (it will apply a translation "after" the actual 3D transform currently held by the `mat4` object). To get the opposite effect like a prepending translation you should invert the getter and the `translate()` like that:

```cpp
auto transform = node->component<Transform>();
transform->matrix(transform->matrix() * translate(vec3(42.f, 0.f, 0.f)));
```

### Setting the 3D position

You can also "reset" the translation or set it to an absolute value using the `math::translate(math::vec3)` function. The following code will actually move our node directly to the (42, 0, 0) coordinates:

```cpp
node->component<Transform>()->matrix(translate(vec3(42.f, 0.f, 0.f)));
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
	auto canvas = Canvas::create("Tutorial - Moving objets", WINDOW_WIDTH, WINDOW_HEIGHT);
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

		// You can comment the following line in order to not enter in the following if case
		cube->addComponent(Transform::create(translate(vec3(0.f, 0.f, -5.f))));

		root->addChild(cube);

		// If the cube already has a transform component
		if (cube->hasComponent<Transform>())
		{
			// We translate the cube to the left
			auto transform = cube->component<Transform>();

			transform->matrix(translate(vec3(-1.f, 0.f, 0.f)) * transform->matrix());
		}
		else
		{
			// We create a new transform component
			auto transform = component::Transform::create();

			// We add it to the cube
			cube->addComponent(transform);

			// We translate the cube in front of the camera with a little lag to the right
			auto translation = cube->component<Transform>();

			translation->matrix(translate(vec3(1.f, 0.f, -5.f)) * transform->matrix());
		}
	});

	sceneManager->assets()->loader()->load();

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
