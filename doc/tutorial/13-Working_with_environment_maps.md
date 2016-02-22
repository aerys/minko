This tutorial will introduce environment map and how to use it in your scene.

Note: you can find some textures in the directory `/doc/tutorial/assets_for_tests/texture`

What is a environment map
-------------------------

Environment maps are textures that Minko uses to simulate reflection on a 3D object. The texture stores the image of an environment surrounding the rendered objects. There is three types of environment map corresponding to three methods in the shader to render an environment map:

-   2D Texture for Blinn Newell technique
-   2D Texture for Probe technique
-   Cube Texture

| Blinn Newell                                                               | Cube Texture                                                                                         |
|----------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------|
| ![](../../doc/image/Ditchriverii9.jpg "../../doc/image/Ditchriverii9.jpg") | ![](../../doc/image/F6-example_horizontalcross.jpg "../../doc/image/F6-example_horizontalcross.jpg") |

Prerequisites
-------------

To fully understand the rest of the tutorial, you need to know how to setup a scene with a phong material, and how to load textures:

-   [Working with the PhongMaterial](../tutorial/11-Working_with_the_PhongMaterial.md)
-   [Loading and using textures](../tutorial/15-Loading_and_using_textures.md)

Step 0: Setup a scene
---------------------

The setup will be the same that the one at the end of the [Working with the PhongMaterial](../tutorial/11-Working_with_the_PhongMaterial.md) tutorial:

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

const std::string MYTEXTURE = "texture/diffuseMap.jpg";

int	main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Working with the PhongMaterial", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()->options()
		->registerParser<file::JPEGParser>("jpg");

	sceneManager->assets()->loader()
		->queue("effect/Phong.effect")
		->queue(MYTEXTURE);

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x00000000))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 1.f, 1.3f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)M_PI * 0.25f, .1f, 1000.f));

	auto spotLight = scene::Node::create("spotLight")
		->addComponent(SpotLight::create(.6f, .78f, 20.f))
		->addComponent(Transform::create(inverse(lookAt(vec3(3.f, 5.f, 1.5f), vec3(), vec3(0.f, 1.f, 0.f)))));
	spotLight->component<SpotLight>()->diffuse(0.5f);

	auto ambientLight = scene::Node::create("ambientLight")
		->addComponent(AmbientLight::create(.2f));

	ambientLight->component<AmbientLight>()->color(vec3(1.f, 1.f, 1.f));

	root->addChild(ambientLight);
	root->addChild(spotLight);
	root->addChild(camera);

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto phongMaterial = material::PhongMaterial::create();

		phongMaterial->diffuseMap(sceneManager->assets()->texture(MYTEXTURE));
		phongMaterial->specularColor(vec4(.4f, .8f, 1.f, 1.f));
		phongMaterial->shininess(2.f);

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
```


Step 1: Setting an environment map
---------------------------------

Environment map are images just like diffuse maps, so their loading workflow are identical:

```cpp
sceneManager->assets()->loader()->queue("texture/envmap.jpg");
```

There is an exception for the cube maps. There is an option in the loader to force him to load the texture as a cube texture, so you have to clone your actual options and set the `isCubeTexture()` option at `true`:

```cpp
auto cubeTextureOptions = sceneManager->assets()->loader()->options()->clone();

cubeTextureOptions->isCubeTexture(true);

sceneManager->assets()->loader()->queue("textures/cubeMap.png", cubeTextureOptions);
```

Now, my cube map is accessible by `sceneManager->assets()->cubeTexture(textures/cubeMap.png)`.

To modulate the `environmentMap` another property of the `PhongMaterial` indicates the percentage of environment map that should be mixed with the computed color: EnvironmentAlpha.

-   `PhongMaterial::environmentAlpha(float alpha)` alpha must be between 0 and 1

The type of environment mapping must be set at the same time as the texture except for the cube maps (the type is set automatically).

```cpp
phongMaterial->environmentMap(assets->texture("texture/envmap.jpg"));
// or
phongMaterial->environmentMap(assets->texture("texture/cubeMap.jpg"));

// 20% of environment for 80% of computed color
phongMaterial->environmentAlpha(0.2f);
```

| Environment Alpha                                                          | 0.2                                                            | 0.5                                                            | 0.95                                                           |
|----------------------------------------------------------------------------|----------------------------------------------------------------|----------------------------------------------------------------|----------------------------------------------------------------|
| ![](../../doc/image/Ditchriverii9.jpg "../../doc/image/Ditchriverii9.jpg") | ![](../../doc/image/Envmap1.jpg "../../doc/image/Envmap1.jpg") | ![](../../doc/image/Envmap2.jpg "../../doc/image/Envmap2.jpg") | ![](../../doc/image/Envmap3.jpg "../../doc/image/Envmap3.jpg") |

Final Code
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

const std::string MYTEXTURE = "texture/diffuseMap.jpg";
const std::string ENVMAP = "texture/envMap.jpg";
int	main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Working with environment maps", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	sceneManager->assets()->loader()->options()
		->registerParser<file::JPEGParser>("jpg");

	auto cubeTextureOptions = sceneManager->assets()->loader()->options()->clone();

	cubeTextureOptions->isCubeTexture(true);

	sceneManager->assets()->loader()
		->queue("effect/Phong.effect")
		->queue(MYTEXTURE)
		->queue(ENVMAP)
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
	spotLight->component<SpotLight>()->diffuse(0.5f);

	auto ambientLight = scene::Node::create("ambientLight")
		->addComponent(AmbientLight::create(.2f));

	ambientLight->component<AmbientLight>()->color(vec3(1.f, 1.f, 1.f));

	root->addChild(ambientLight);
	root->addChild(camera);
	root->addChild(spotLight);

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		auto phongMaterial = material::PhongMaterial::create();

		phongMaterial->diffuseMap(sceneManager->assets()->texture(MYTEXTURE));
		phongMaterial->environmentMap(sceneManager->assets()->texture(ENVMAP));
		phongMaterial->environmentAlpha(.5f);

		auto mesh = scene::Node::create("mesh")
			->addComponent(Transform::create(scale(vec3(1.1f))))
			->addComponent(Surface::create(
				geometry::SphereGeometry::create(sceneManager->assets()->context()),
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
```


Where to go from here
---------------------

`environmentMap` can me mixed with `specularMap` ([Working with specular maps](../tutorial/14-Working_with_specular_maps.md)) and/or `normalMap` ([Working with normal maps ](../tutorial/12-Working_with_normal_maps.md)) to produce great effect on your 3D models: [ PhongMaterial reference full example](../article/PhongMaterial_reference.md#Full_Example).
