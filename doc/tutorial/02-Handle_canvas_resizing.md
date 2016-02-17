Whether the `Canvas` represents a simple drawing area or a complete window, you'll have to deal with the eventuality of its resizal. In this tutorial, we will learn how to catch the resizal of the `Canvas` and how to adapt the projection of our camera(s) to fit its new aspect ratio.

The code for this tutorial is based on the one described in the [Hello cube!](../tutorial/01-Hello_cube!.md) tutorial.

Step 1: Catch the resize Signal
-------------------------------

Once our `Canvas` is created, we can listen to its `Canvas::resized()` signal:

```cpp
auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint width, uint height) {

 // do something here...

});
```


The `Canvas::resized()` signal callbacks expect three arguments:

-   `AbstractCanvas::Ptr canvas`, the actual `Canvas` object that executed the signal
-   `uint width`, the new width of the `Canvas`
-   `uint height`, the new height of the `Canvas`

Step 2: Adapting the projection
-------------------------------

Assuming we have a direct access to our camera scene `Node`, we can adapt it's projection by accessing its `PerspectiveCamera` component and setting its `aspectRatio()` property:

```cpp
auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint width, uint height) {

 camera->component<PerspectiveCamera>()->aspectRatio((float)width / (float)height);

});
```


If we only have access to the root `Node` of our scene, we can fetch all the nodes with a `PerspectiveCamera` component to update them:

```cpp
auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint width, uint height) {

 auto cameras = scene::NodeSet::create(root)->descendants(true)->where([](scene::Node::Ptr node)
 {
   return node->hasComponent<PerspectiveCamera>();
 });

 for (auto& camera : cameras->nodes())
   camera->component<PerspectiveCamera>()->aspectRatio((float)width / (float)height);

});
```


The `NodeSet` class and its `descendants()` and `where()` operators help us fetching all the descendants of `root` and filter them using a custom function.

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

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

int	main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Handle canvas resizing", WINDOW_WIDTH, WINDOW_HEIGHT);
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

	auto resize = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, math::uint width, math::uint height)
	{
		auto cameras = scene::NodeSet::create(root)->descendants(true)->where([](scene::Node::Ptr node)
		{
			return node->hasComponent<PerspectiveCamera>();
		});

		for (auto& camera : cameras->nodes())
			camera->component<PerspectiveCamera>()->aspectRatio((float)width / (float)height);

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
