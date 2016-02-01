In this tutorial we will see how to load external `*.effect` files. Loading effects is a mandatory procedure since any pixel rendered in our application is computed using those precious effects. They are handled as external assets for the following reasons: # They are easily redistributable. # They can easily be swapped at runtime. # The rendering engine is "data driven": the effect files provide all the required data to setup the rendering process, making it very modular and flexible.

**Attention!** To make sure your `*.effect` are easily accessible at runtime on all platforms, we recommend storing them in the `asset/effect` directory located in the root folder of your application project. The content of the `asset` directory is automatically copied after a successful build.

Step 1: Loading a single effect
-------------------------------

To load a single effect, we will use the `AssetLibrary::load()` method:

```cpp
auto _= sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets) {

	std::cout << "my effect has been loaded!" << std::endl;

});

sceneManager->assets()->loader()->load("MyEffect.effect");
```


This code snippet should output:

```
my effect has been loaded!
```


when all the file passed to the `AssetLibrary::load()` method has been successfully loaded and is available in the `AssetLibrary`.

Step 1 (alternative): Loading multiple effects/assets
-----------------------------------------------------

To load multiple effects - or if the `*.effect` file(s) you want to load is just one asset among others - we can use the `AssetLibrary::queue()` method. This method expects the same arguments as the `AssetLibrary::load()` method except it will enlist all the files to load. It will then eventually load then in a batch when `AssetLibrary::load()` is actually called:

```cpp
sceneManager->assets()->loader()
	->queue("MyEffect1.effect")
	->queue("MyEffect2.effect")
	->queue("MyEffect3.effect");

auto _= sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets) {

	std::cout << "all assets have been loaded!" << std::endl;

});

sceneManager->assets()->loader()->load();
```


This code snippet should output:

```
 all assets have been loaded!
```


when all the assets listed by the successive calls to the `AssetLibrary::queue()` method have been successfully loaded and are available in the `AssetLibrary`.

Step 2: Fetching the Effect object
----------------------------------

When our `*.effect` is loaded, it is automatically made available in the `AssetLibrary` and can be fetched using the `AssetLibrary::effect()` method. This method expects a single `const std::string&` argument that will point to the loaded `Effect` object using:

-   the value of the "name" field in the `*.effect` file, if any;
-   or the actual file name used upon loading (the `std::string` passed to either `AssetLibrary::load()` or `AssetLibrary::queue()`).

```cpp
auto _= sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets) {

 auto fx = assets()->effect("MyEffect.effect");

}
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
#include "minko/MinkoPNG.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

int	main(int argc, char** argv)
{
	auto canvas = Canvas::create("Tutorial - Loading effects", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()->options()
		->registerParser<file::PNGParser>("png");

	sceneManager->assets()->loader()
		->queue("effect/MyEffect.effect");

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x00000000))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 1.f, 1.3f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)M_PI * 0.25f, .1f, 1000.f));

	root->addChild(camera);

	auto cube = scene::Node::create("cube");

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		cube->addComponent(Transform::create());
		cube->addComponent(Surface::create(
				geometry::CubeGeometry::create(sceneManager->assets()->context()),
				material::BasicMaterial::create()->diffuseColor(vec4(1.f, 0.f, 0.f, 1.f)),
				sceneManager->assets()->effect("effect/MyEffect.effect")
				));
		root->addChild(cube);
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


Where to go from there...
-------------------------

Now that you know how to load effects, you should read one of the following tutorials:

-   [Create your first custom effect](../tutorial/17-Creating_a_custom_effect.md)
-   [Creating a simple post-processing effect](../tutorial/22-Creating_a_simple_post-processing_effect.md)
