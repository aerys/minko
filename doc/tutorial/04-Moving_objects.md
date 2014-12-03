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

Now that our node has a `Transform`, we can modify it to translate the node in our 3D scene! This is done by using the `Transform::matrix()` property that returns a `Matrix4x4` object.From there we can do two different things: apply a translation - meaning add a translation operation to an existing 3D transform - or set the position of the node directly.

### Apply a 3D translation

We can then use the `Matrix4x4::appendTranslation()` method to translate our object. The following code will translate ou node on the x axis:

```cpp
node->component<Transform>()->matrix()->appendTranslation(42.f, 0.f, 0.f); 
```


For instance, if `node` was located in (0, 0, 0), it should now be in (42, 0, 0). The `Matrix4x4::appendTranslation()` will apply a translation "after" the actual 3D transform currently held by the `Matrix4x4` object. To get the opposite effect, you should use The `Matrix4x4::prependTranslation()`.

### Setting the 3D position

You can also "reset" the translation or set it to an absolute value using the `Matrix4x4::translation()` method. The following code will actually move our node directly to the (42, 0, 0) coordinates:

```cpp
node->component<Transform>()->matrix()->translation(42.f, 0.f, 0.f); 
```


Final code
----------

```cpp
#include "minko/Minko.hpp" 
#include "minko/MinkoSDL.hpp"

using namespace minko; 
using namespace minko::math; 
using namespace minko::component;

const uint WINDOW_WIDTH = 800; 
const uint WINDOW_HEIGHT = 600;

int main(int argc, char** argv) {

   auto canvas = Canvas::create("Tutorial - Moving objets", WINDOW_WIDTH, WINDOW_HEIGHT);
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

       // If the cube already has a transform component
       if (cube->hasComponent<Transform>())
       {
           // We translate the cube to the left
           cube->component<Transform>()->matrix()->appendTranslation(-1.f, 0.f, 0.f);
       }
       else
       {
           // We create a new transform component
           auto transform = component::Transform::create();
           // We add it to the cube
           cube->addComponent(transform);

           // We translate the cube in front of the camera with a little lag to the right
           cube->component<Transform>()->matrix()->appendTranslation(1.f, 0.f, -5.f);
       }
       
       auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
       {
           sceneManager->nextFrame(t, dt);
       });

       canvas->run();
   });

   sceneManager->assets()->load();

   return 0;

} 
```


