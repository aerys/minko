In this tutorial, we will see how to load textures and use them in materials. This tutorial focus on loading RGBA diffuse textures, but the very same technique is used to load just about any texture (normal maps, specular maps, etc...).

The code for this tutorial is based on the one described in the [Hello cube!](../tutorial/01-Hello_cube!.md) tutorial.

Just like any asset, texture are loaded through the `AssetLibrary`. Here, we will use the library available from our `SceneManager::assets()` property, but you can also create your own `AssetLibrary` objects to fit your needs.

Step 0: Enabling the required plugins
-------------------------------------

By default, no texture file formats are supported. This kind of features are provided by plugins. Regarding texture files, Minko provides the following plugins:

-   the "jpeg" plugin will handle JPEG textures parsing
-   the "png" plugin will handle PNG textures parsing

To enable one (or both) of those plugins, we will have to update our project configuration and regenerate our solution. To do this, open the `premake4.lua` file in the root folder of your project in your favorite text editor and uncomment the following lines:

```lua
minko.plugin.enable("jpeg")
minko.plugin.enable("png")
```


Uncommenting one of those lines will enable the corresponding plugin. Now we have to regenerate the project/solution files to take those changes into account. To do this, please refer to the [Step 3 of the "Create a new application" tutorial](../tutorial/Create_a_new_application.md#step-3-target-your-platform).

If you want to support more texture file formats, feel free to create a new plugin based on those already available. It's usually quite simple to integrate 3rd party parsing libraries to handle new file formats.

Step 1: Registering parsers
---------------------------

Before we can load anything, we have to make sure Minko will know how to handle the loaded data. To do this, we must register some data parsers to some specific file extensions. The parsers are registered on the `AssetLibrary` directly using the `AssetLibrary::registerParser()` method:

```cpp
sceneManager->assets()->loader()->options()->registerParser<file::JPEGParser>("jpg");
```


Note that you have to call `registerParser()` for each file extension you want to be able to load.

Step 1: Loading a texture
-------------------------

To load a texture, we call the `AssetLibrary::load()` method passing the file name of our texture as the single argument:

```cpp
sceneManager->assets()->loader()->queue("texture/my_texture.jpg");
```


Loading textures can be an asynchronous task depending on how the internal loader will actually work. To be notified when our loading operation is done, we listen to the `AssetLibrary::complete()` signal. The following code will output the loaded texture width and height in the console:

```cpp
auto _= sceneManager->assets()->loader()->complete()->connect([&](file::AssetLibrary assets)
{
	auto texture = sceneManager->assets()->texture("texture/my_texture.jpg");

	std::cout << "texture width: " << texture->width() << std::endl;
	std::cout << "texture height: " << texture->height() << std::endl;
});

sceneManager->assets()->loader()->load();
```


Note how the `AssetLibrary` is used as somekind of an application file system. It will not only perform load operations but also store their result and make them available to the rest of the program.

**Attention!** Make sure you listen to the `AssetLibrary::complete()` signal **before** you call `AssetLibrary::load()`!

**Attention!** Here, we assume the `texture/my_texture.jpg` file is located in the `asset` folder of your project.

Step 2: Setting the texture
---------------------------

To use our texture upon loading, we simply use the `BasicMaterial::diffuseMap()` method to set the right property:

```cpp
auto _= sceneManager->assets()->loader()->complete()->connect([&](file::AssetLibrary assets)
	auto cubeMaterial = material::BasicMaterial::create();

	cubeMaterial->diffuseMap(sceneManager->assets()->texture(MYTEXTURE));
});
```
You can also use the `BasicMaterial::set()` method:

```cpp
auto _= sceneManager->assets()->loader()->complete()->connect([&](file::AssetLibrary assets)
	auto cubeMaterial = material::BasicMaterial::create();

	cubeMaterial->set({ {"diffuseMap", sceneManager->assets()->texture(MYTEXTURE)} });
});
```

Both methods have the exact same behavior since `Material::set()` is actually called by `BasicMaterial::diffuseMap()` internally. The second method is a bit harder to write because its dynamic and code-hinting won't work, but it should be a bit faster since it avoids a dynamic pointer cast at runtime. Working with `Material::set()` is also more generic since it will work on any `Material` no matter its actual type and it will have the desired effect as long as you use the right string property name. Therfore, you might want to use `Material::set()` if you are writting your own scenes parser or some complex assets dynamic loading code. To learn more on this subject, please read the [The difference between the Material_set() method and setter methods](../article/The_difference_between_the_Material_set%27%28_method_and_setter_methods.md) article.

Of course, if you want to use your texture for something else that the diffuse map, you'll have to set the corresponding property. For example, if you want to set the normal map, you'll want to set the `normalMap` property instead. All the other operations are exactly the same.

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
#include "minko/MinkoJPEG.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

const std::string MYTEXTURE = "texture/my_texture.jpg";

int    main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Loading and using textures", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()->options()
		->registerParser<file::JPEGParser>("jpg");

	sceneManager->assets()->loader()
		->queue("effect/Basic.effect")
		->queue(MYTEXTURE);

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
		auto cubeMaterial = material::BasicMaterial::create();

		cubeMaterial->diffuseMap(sceneManager->assets()->texture(MYTEXTURE));

		cube->addComponent(Transform::create());
		cube->addComponent(Surface::create(
			geometry::CubeGeometry::create(sceneManager->assets()->context()),
			cubeMaterial,
			sceneManager->assets()->effect("effect/Basic.effect")
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
