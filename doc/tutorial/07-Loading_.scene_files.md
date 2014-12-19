Step 0: Exporting .scene files from the Minko editor
====================================================

You can export your mks files to the new .scene file for Minko 3. See the [Export .scene files](../tutorial/Exporting_.scene_files.md) tutorial.

Step 1: Enable the serializer plugin
====================================

In order to use .scene files, you will have to enable the `serializer` plugin. Make sure the `premake5.lua` file of your project includes the following line :

```lua
minko.plugin.enable("serializer") 
```


Next, the `AssetManager` of your scene should register the `minko::[file::SceneParser`](file::SceneParser`) for .scene files : 
```cpp
#include "minko/MinkoSerializer.hpp"

sceneManager->assets()->registerParser<minko::[file::SceneParser>](file::SceneParser>)("scene"); 
```


Step 2: Loading the file
========================

You then have to add your .scene file to the `AssetManager`. You only need to import the .scene file, all eventual dependency will be loaded. 
```cpp
sceneManager->assets()->queue("model/myScene/myScene.scene"); 
```


Step 3: Adding the loaded asset to the scene
============================================

Once the assets have been loaded, your then deserialized symbol can be added to the scene. 
```cpp
auto root = scene::Node::create("root")->addComponent(sceneManager);

auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets) {

   root->addChild(assets->symbol("model/myScene/myScene.scene"));

});

sceneManager->assets()->load(); 
```


You can look at the serializer example for a more detailed example of importing a .scene file.

Final Code
==========

```cpp
#include "minko/Minko.hpp" 
#include "minko/MinkoSDL.hpp" 
#include "minko/MinkoSerializer.hpp"

using namespace minko; 
using namespace minko::component; 
using namespace minko::math;

const uint WINDOW_WIDTH = 800; 
const uint WINDOW_HEIGHT = 600; std::string SCENE_FILENAME = "model/myScene/myScene.scene";

int main(int argc, char** argv) {

   auto canvas = Canvas::create("Minko Tutorial - Loading .scene files", WINDOW_WIDTH, WINDOW_HEIGHT);
   auto sceneManager = SceneManager::create(canvas->context());

   sceneManager->assets()->registerParser<minko::[file::SceneParser>](file::SceneParser>)("scene");
   sceneManager->assets()->queue("effect/Phong.effect");
   sceneManager->assets()->queue(SCENE_FILENAME);

   auto root = scene::Node::create("root")->addComponent(sceneManager);

   auto camera = scene::Node::create("camera")
       ->addComponent(Renderer::create(0x7f7f7fff))
       ->addComponent(Transform::create(
       Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0., 3., -5.f))
       ))
       ->addComponent(PerspectiveCamera::create(
       (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, (float) PI * 0.25f, .1f, 1000.f)
       );
   root->addChild(camera);

   auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
   {
       root->addChild(assets->symbol(SCENE_FILENAME));

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


