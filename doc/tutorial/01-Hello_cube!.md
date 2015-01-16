Step 0: Bootstrap the application project
-----------------------------------------

The very first step is to follow the [Create a new application](../tutorial/Create_a_new_application.md) tutorial to bootstrap your appliation project/solution.

Step 1: Initialize the window
-----------------------------

We will use the SDL plugin to initialize a window. The following code is a simple C++ main function that initialize such window:

```cpp
#include "minko/Minko.hpp" 
#include "minko/MinkoSDL.hpp"

using namespace minko; 
using namespace minko::math; 
using namespace minko::component;

const uint WINDOW_WIDTH = 800; 
const uint WINDOW_HEIGHT = 600;

int main(int argc, char** argv) {

 auto canvas = Canvas::create("Hello cube!", WINDOW_WIDTH, WINDOW_HEIGHT);

 canvas->run();

 return 0;

} 
```


The call to `canvas->run()` will make the application loop until `canvas->quit()` is called or the window is closed.

Step 2: Load the assets
-----------------------

Before we can display anything, we need to load an `Effect`. An `Effect` is an external asset - a program actually - that will drive the rendering engine and tell it how things should be rendered. Everything you see on the screen, every final color of a pixel, has been rendered according to an `Effect`.

For now, we will just see how to load the `Basic.effect`, which is a very simple rendering program delivered by default with Minko's core framework. To use this `Effect` we first have to load it.

All the assets are loaded to and from an `AssetLibrary`. You can create an `AssetLibrary` yourself but it is a better practice to use the one provided with the `SceneManager` component. Indeed, this component will be added to the root `Node` of our scene, making all of the assets of its `SceneManager::assets()` property available to all descendants. As we will see later in this tutorial, the `SceneManager` is also responsible for stepping through frames rendering.

The following piece of code will create a `SceneManager` and use it's `SceneManager::assets()` property to load the `Basic.effect` from the filesystem:

```cpp
auto sceneManager = component::SceneManager::create(canvas);

sceneManager->assets()->queue("effect/Basic.effect"); auto complete = sceneManager->assets()->loader()->complete()->connect([&](file::Loader::Ptr loader) {

 // assets are loaded and ready

});

sceneManager->assets()->loader()->load(); 
```


The `AssetLibrary::queue()` method will enlist all the file names that you want to load: you can call `AssetLibrary::queue()` as many times as you have different files to load. Loading will eventually start when `AssetLibrary::load()` will be called. Then, the `AssetLibrary::complete()` signal will be called when all the required files have been a) successfully loaded or b) caused an error.

The `Basic.effect` file itself is not in your application: it's in Minko's core framework. Minko's build system is made in such a way that this kind of required files are copied automatically using post-build commands. Thus, you don't have to worry about the actual location of the `Basic.effect` file for now.

If you want to learn more about effects loading, you can read the [Loading effects](../tutorial/16-Loading_effects.md) tutorial.

Step 3: Initialize the scene
----------------------------

### Create the scene root node

The scene root is just a `Node` with the `SceneManager` component:

```cpp
auto root = scene::Node::create("root")

 ->addComponent(sceneManager);

```


### Add a camera

Our camera will be just a scene `Node` with two components:

-   a `Renderer`, that will take care of actual rendering operations
-   a `PerspectiveCamera` that will provide camera related data (world to view matrix, projection matrix...) for proper rendering

```cpp
auto camera = scene::Node::create("camera")

 ->addComponent(Renderer::create(0x7f7f7fff))
 ->addComponent(PerspectiveCamera::create(
   (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)M_PI * 0.25f, .1f, 1000.f)
 );

root->addChild(camera); 
```


Note how we initialize the `Renderer` with a custom background color with an RGBA formatted literal integer value.

The `PerspectiveCamera::create()` expects the following arguments:

-   `float aspectRatio`, the projection aspect ratio (width / height)
-   `float fieldOfView`, the angle around the y axis in radians
-   `float zNear`, the distance of the near clipping plane
-   `float zFar`, the distance of the far clipping plane

Step 4: Create the cube
-----------------------

### Create the cube scene node

To create the cube, we mainly have to:

-   create a scene `Node`
-   create a new `BasicMaterial`
-   create a new `CubeGeometry`
-   retrieve the loaded effect from the `AssetLibrary`

The following code will do those 4 steps:

```cpp
auto cube = scene::Node::create("cube"); auto cubeMaterial = material::BasicMaterial::create(); auto cubeGeometry = geometry::CubeGeometry(assets->context()); auto cubeEffect = assets->effect("effect/Basic.effect");

cube->addComponent(Surface::create(cubeGeometry, cubeMaterial, cubeEffect); 
```


### Set the cube color

The `Basic.effect` expects the `diffuseColor` material value to be set to an RGBA `Vector4`. The following code will set the color of our cube to blue (R: 0, G: 0, B: 1, A: 1):

```cpp
cubeMaterial->diffuseColor(Vector4::create(0.f, 0.f, 1.f, 1.f)); 
```


You can learn more about the `BasicMaterial` in the [Working with the BasicMaterial](../tutorial/10-Working_with_the_BasicMaterial.md) tutorial.

### Move the cube

By default, our `PerspectiveCamera` will be in (0, 0, 0) looking down the -Z axis. To make sure our cube is visible, we must translate it down the the -Z axis to make sure our camera is not inside the cube. To do this, we simply add a `Transform` component to our cube scene node:

```cpp
cube->addComponent(Transform::create(Matrix4x4::create()->translation(0.f, 0.f, -5.f))); 
```


Note that we initialize the `Transform` with a `Matrix4x4` holding a (0, 0, -5) translation. You can learn more about the `Transform` component in the [Moving objects](../tutorial/04-Moving_objects.md) tutorial

### Add the cube to the scene

We can then add our cube directly to the scene root using the `Node::addChild()` method:

```cpp
root->addChild(cube); 
```


Step 5: Render the scene
------------------------

To render our scene, we will use the `SceneManager::nextFrame()` method. This method will tell all `Renderer` components to process their respective list of draw calls. Calling `SceneManager::nextFrame()` will process and render a single frame. To render frames whenever our window is ready, we will listen to the `canvas->enterFrame()` signal:

```cpp
auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt) {

 sceneManager->nextFrame(t, dt);

}); 
```


Step 6: Make the cube rotate
----------------------------

To rotate our cube, we just have to access its `Transform` component and apply the rotation we want. Here, we will use `Matrix4x4::prependRotation()` because we want our rotation to be done "before" the translation applied in step 4:

```cpp


 cube->component<Transform>()->matrix()->prependRotationY(.01f);

```


To make our cube rotate a bit more at each frame, we simply add this line to our "enter frame" callback:

```cpp
auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt) {

 cube->component<Transform>()->matrix()->prependRotationY(.01f);
 sceneManager->nextFrame(t, dt);

}); 
```


Final code
----------

```cpp
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
    auto canvas = Canvas::create("Minko Tutorial - Hello cube!", WINDOW_WIDTH, WINDOW_HEIGHT);
    auto sceneManager = component::SceneManager::create(canvas);

    sceneManager->assets()->queue("effect/Basic.effect");
    auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
    {
        auto root = scene::Node::create("root")
            ->addComponent(sceneManager);

        auto camera = scene::Node::create("camera")
            ->addComponent(Renderer::create(0x7f7f7fff))
            ->addComponent(PerspectiveCamera::create(
            (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, (float) PI * 0.25f, .1f, 1000.f)
            );
        root->addChild(camera);

        auto cube = scene::Node::create("cube")
            ->addComponent(Transform::create(Matrix4x4::create()->translation(0.f, 0.f, -5.f)))
            ->addComponent(Surface::create(
            geometry::CubeGeometry::create(assets->context()),
            material::BasicMaterial::create()->diffuseColor(Vector4::create(0.f, 0.f, 1.f, 1.f)),
            assets->effect("effect/Basic.effect")
            ));
        root->addChild(cube);

        auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
        {
            cube->component<Transform>()->matrix()->prependRotationY(.01f);
            sceneManager->nextFrame(t, dt);
        });

        canvas->run();
    });

    sceneManager->assets()->load();

    return 0;
}
```

