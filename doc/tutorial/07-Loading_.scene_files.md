Step 0: Exporting .scene files from the Minko editor
====================================================

You can export your mks files to the new .scene file for Minko 3. See the [Export .scene files](../tutorial/Exporting_.scene_files.md) tutorial.

Step 1: Enable the serializer plugin
====================================

In order to use .scene files, you will have to enable the `serializer` plugin. Make sure the `premake5.lua` file of your project includes the following line :

```lua
minko.plugin.enable("serializer")
```


Next, the `AssetManager` of your scene should register the `file::SceneParser` for `.scene` files :

```cpp
#include "minko/MinkoSerializer.hpp"

sceneManager->assets()->loader()->options()->registerParser<file::SceneParser>("scene");
```


Step 2: Loading the file
========================

You then have to add your `.scene` file to the `AssetManager`. You only need to import the `.scene` file, all eventual dependency will be loaded.

```cpp
sceneManager->assets()->loader()->queue(model/myScene/myScene.scene);
```


Step 3: Adding the loaded asset to the scene
============================================

Once the assets have been loaded, your then deserialized symbol can be added to the scene.

```cpp
auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
{
	root->addChild(sceneManager->assets()->symbol(SCENE_FILENAME));
});

sceneManager->assets()->loader()->load();
```


You can look at the serializer example for a more detailed example of importing a `.scene` file.

Final Code
==========

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
#include "minko/MinkoSerializer.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

const math::uint WINDOW_WIDTH = 800;
const math::uint WINDOW_HEIGHT = 600;

std::string SCENE_FILENAME = "model/myScene/myScene.scene";

int main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Tutorial - Loading .scene files", WINDOW_WIDTH, WINDOW_HEIGHT);
    auto sceneManager = SceneManager::create(canvas);

    sceneManager->assets()->loader()->options()
		->registerParser<minko::file::SceneParser>("scene");

	sceneManager->assets()->loader()
		->queue("effect/Phong.effect")
		->queue(SCENE_FILENAME)
		;

    auto root = scene::Node::create("root")
		->addComponent(sceneManager);

    auto camera = scene::Node::create("camera")
        ->addComponent(Renderer::create(0x7f7f7fff))
        ->addComponent(Transform::create(inverse(lookAt(vec3(0.f, 3.f, -5.f), vec3(), vec3(0.f, 1.f, 0.f)))
        ))
        ->addComponent(PerspectiveCamera::create(
        (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, float(M_PI) * 0.25f, .1f, 1000.f)
        );

	root->addChild(camera);

    auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader)
    {
		root->addChild(sceneManager->assets()->symbol(SCENE_FILENAME));
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
