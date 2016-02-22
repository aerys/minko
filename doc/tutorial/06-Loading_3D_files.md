This tutorial explains how to use the file loading API. This API is designed to ease the loading of external files of any format. The goal is to give developers the possibility to **load files using the same API regardless of their actual format**.

A file can be loaded from a file or from a url.

Enable ASSIMP
-------------

Assimp is the asset import library used by Minko to load asset files, we have to enable it into the `premake5.lua` with these lines:

```lua
--the following plugin can be useful for assets that need to load jpeg files
minko.plugin.enable("assimp")
minko.plugin.enable("jpeg")
```


The next step is to include the correct header into your C++ application source code.

```cpp
#include "minko/MinkoASSIMP.hpp"
#include "minko/MinkoJPEG.hpp"
```


Setup file parsers
------------------

Every supported file format is available through a plugin. Each parser must be registered with its associated file format.

In order to enable a data parser, you have to tell Minko it exists using the `AssetLibrary::registerParse` method.

```cpp
sceneManager->assets()->loader()->options()
		->registerParser<file::OBJParser>("obj")
		->registerParser<file::ColladaParser>("dae")
		->registerParser<file::JPEGParser>("jpg");

```


Most common 3D file formats are supported by the ASSIMPParser ([Supported file formats](../article/Supported_file_formats_(Minko_Engine).md)). To load file with specific Minko extension (lighter, faster and modular), you can read the corresponding tutorial: [Loading .scene files](../tutorial/07-Loading_.scene_files.md). Learn how to export this format from the editor : [Exporting .scene files](../tutorial/Exporting_.scene_files.md).

After that, you can add your files to the loading queue and the right parser will be chosen automatically.

Access loaded files
-------------------

Once the loaded complete signal of the assetLibrary is triggered you can access the different model with the `AssetLibrary::symbol(name)` method

```cpp
auto objModel = sceneManager->assets()->symbol("model/model.obj");
auto daeModel = sceneManager->assets()->symbol("model/model.dae");
```


Also, all geometry, textures, material and effects needed by your model is automaticly added to the assetLibrary. So, if you know the name of one of them you can have a access throw the `AssetLibrary::texture(name)`, `AssetLibrary::geometry(name)`, `AssetLibrary::material(name)` and `AssetLibrary::effect(name)` methods.

Use default Effect
------------------

Most of the time, your model does not know the effect that its surfaces are supposed to be linked with. In order to indicate an effect, you must use the defaultOptions of the `AssetLibrary`.

```cpp
sceneManager->assets()->loader()->options()->effect(sceneManager->assets()->effect(DEFAULT_EFFECT));
```


You can also choose a default material

```cpp
sceneManager->assets()->loader()->options()->material()->set({{ "diffuseColor", vec4(.8f, .1f, .1f, 1.f) }});
```


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
#include "minko/MinkoASSIMP.hpp"
#include "minko/MinkoJPEG.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::component;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

const std::string OBJ_MODEL_FILENAME = "model/pirate.obj";
const std::string DAE_MODEL_FILENAME = "model/pirate.dae";

int	main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Tutorial - Loading 3D files", WINDOW_WIDTH, WINDOW_HEIGHT);
	auto sceneManager = component::SceneManager::create(canvas);

	canvas->context()->errorsEnabled(true);

	sceneManager->assets()->loader()->options()
		->registerParser<file::OBJParser>("obj")
		->registerParser<file::ColladaParser>("dae")
		->registerParser<file::JPEGParser>("jpg");

	sceneManager->assets()->loader()
		->queue("effect/Basic.effect")
		->queue("effect/Phong.effect")
		->queue(DAE_MODEL_FILENAME)
//		->queue(OBJ_MODEL_FILENAME)
		;

	sceneManager->assets()->loader()->options()
		->generateMipmaps(true);

	auto root = scene::Node::create("root")
		->addComponent(sceneManager);

	auto camera = scene::Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 0.f, 5.f), vec3(), vec3(0.f, 1.f, 0.f)))))
		->addComponent(PerspectiveCamera::create((float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)M_PI * 0.25f, .1f, 1000.f));

	root->addChild(camera);
		sceneManager->assets()->loader()->options()
			->effect(sceneManager->assets()->effect("effect/Basic.effect"));

	auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
	{
		// DAE model
		auto daeModel = sceneManager->assets()->symbol(DAE_MODEL_FILENAME);

		auto daeMove = daeModel->component<Transform>();
		daeMove->matrix(translate(vec3(1.f, -1.f, 0.f)) * daeMove->matrix());

		// add the scene
		root->addChild(daeModel);

	/*	// OBJ Model
		auto objModel = sceneManager->assets()->symbol(OBJ_MODEL_FILENAME);

		// change scale for the object file
		auto objScale = objModel->component<Transform>();
		objScale->matrix(scale(vec3(.01f)) * objScale->matrix());

		// change position for the object file
		auto objMove = objModel->component<Transform>();
		objMove->matrix(translate(vec3(-1.f, -1.f, 0.f)) * objMove->matrix());

		// add the scene
		root->addChild(objModel);
	*/
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
