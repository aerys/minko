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

 std::cout << "The 'up' key is" << (k->keyIsDown(input::Keyboard::ScanCode::SPACE) ? "" : " not") << " down" << std::endl;

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
#include "minko/Minko.hpp" 
#include "minko/MinkoSDL.hpp"

using namespace minko; 
using namespace minko::math; 
using namespace minko::component;

const uint WINDOW_WIDTH = 800; 
const uint WINDOW_HEIGHT = 600;

int main(int argc, char** argv) {

   auto canvas = Canvas::create("Tutorial - Moving objects with the keyboard", WINDOW_WIDTH, WINDOW_HEIGHT);
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

       auto keyDown = canvas->keyboard()->keyDown()->connect([&](input::Keyboard::Ptr k)
       {
           if (k->keyIsDown(input::Keyboard::ScanCode::LEFT))
               cube->component<Transform>()->matrix()->appendTranslation(-0.1f);
           if (k->keyIsDown(input::Keyboard::ScanCode::RIGHT))
               cube->component<Transform>()->matrix()->appendTranslation(0.1f);
       });

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


