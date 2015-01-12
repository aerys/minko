In this tutorial, we will see the basic structure of a script file, how to load a script and assign it to a scene node. To do this, we will use the "lua" plugin that provides all you need to work with Minko's C++ engine using the Lua scripting language.

Scripting in Lua is very cool because you get the best of both worlds:

-   the native performances of C++ when invoking native functions;
-   the simplicity of the Lua scripting language with good overall performances;
-   the possibility to update your application code without re-compiling anything;
-   the possibility to load and assign scripts at runtime; even scripts loaded from a remote location.

Before we start, you should read the following tutorials:

-   [Create a new application](../tutorial/Create_a_new_application.md)
-   [Hello cube!](../tutorial/01-Hello_cube!.md)
-   [How to enable a plugin](../tutorial/How_to_enable_a_plugin.md)

Step 0: Enabling the Lua plugin
-------------------------------

The very first step is to make sure we can use Lua scripts in our applications. To do this, we're going to follow the [Create a new application](../tutorial/Create_a_new_application.md) tutorial and then the [How to enable a plugin](../tutorial/How_to_enable_a_plugin.md) tutorial to enable the "lua" plugin. Your `premake5.lua` file should look like this:

```lua
dofile(os.getenv("MINKO_HOME") .. "/sdk.lua")

PROJECT_NAME = path.getname(os.getcwd())

minko.project.solution(PROJECT_NAME)

   minko.project.application(PROJECT_NAME)

       language "c++"
       kind "ConsoleApp"

       files { "src/**.cpp", "src/**.hpp" }
       includedirs { "src" }

       -- plugins
       minko.plugin.enable("sdl")
       minko.plugin.enable("lua")

```


Don't forget to [regenerate your solution file](../tutorial/Create_a_new_application.md#step-3-target-your-platform) when you're done.

Step 1: My first script file
----------------------------

Lua scripts are an implementation of the `AbstractScript` C++ class that will leverage the Lua virtual machine to execute Lua code. As such, they are implemented as Lua classes providing three main methods:

-   the `start` method will be called each and everytime the script is added to a target scene node;
-   the `update` method will be called at each frame as long as the script is running;
-   the `stop` method will be called when the script is removed from one of its target scene nodes.

Keep in mind that **each script can be added to more than one target scene node**. This is why all of the 3 methods above take that **target node as an argument**.

You should also keep in mind that:

-   you do not have to defined all of those 3 methods, you should only defines the ones you find useful for your script;
-   you can have a script that will define none of the 3 methods but will only declare global values used by other scripts;
-   you are free to create more methods (or even classes) in your scripts as long as they don't conflict;
-   scripts should be stored in/loading from the `/asset/script` directory of your application;
-   the name of the script class must be the same as the name of the script file (ex: the "foo" script must be defined in "foo.lua".

Here is an example of script that will simply output debug messages in the console using the `print()` global Lua function:

```lua
-- /asset/script/my_script.lua function my_script:start(node)

 print('start')

end

function my_script:update(node)

 print('update')

end

function my_script:stop(node)

 print('stop')

end 
```


Step 2: Initialization of LUA
-----------------------------

The first thing to do in our C++ application code is to load the main header for the "lua" plugin:

```cpp
#include "minko/MinkoLua.hpp" 
```


Now, we have to initialize the Lua context to give it access to our canvas and root node without forget to add a `LuaScriptManager` thereto:

```cpp
auto canvas = Canvas::create("Minko Tutorial - My first script", 800, 600); auto sceneManager = component::SceneManager::create(canvas); auto root = scene::Node::create("root")->addComponent(sceneManager);

// initialization of Lua context LuaContext::initialize(argc, argv, root, canvas);

// add a LuaScriptManager to the root node root->addComponent(LuaScriptManager::create()); 
```


Step 3: Loading a script
------------------------

We can then use all the classes related to Lua scripting. Next we need to actually load our script. This should not be a surprise by now: we are going to use the `AssetLibrary`. Just like for any kind of assets, we need to:

-   make sure the `LuaScriptParser` is registered for the "lua" file extension;
-   actually load our script(s) using `AssetLibrary::queue()` and/or `AssetLibrary::load()`.

```cpp
// register the LuaScriptParser sceneManager->assets()->registerParser<[file::LuaScriptParser>](file::LuaScriptParser>)("lua");

// queue the "script/my_script.lua" script file sceneManager->assets()->queue("script/my_script.lua");

if (assets->script("script/my_script.lua"))

   std::cout << "script loaded!" << std::endl;

// actually begin loading operations sceneManager->assets()->load(); 
```


Step 4: Assigning a script
--------------------------

Scripts are components. When loaded, each script file will be available as an `AbstractScript` component in the `AssetLibrary`. Thus, we can assign scripts using the `Node::addComponent()` method on the node that is supposed to be the target of that very script:

In this case, we will simply add our script to the root of our scene:

```cpp
auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets) {

   root->addComponent(assets->script("script/my_script.lua"));

   auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float t, float dt)
   {
       sceneManager->nextFrame(t, dt);
   });

   canvas->run();

}); 
```


You can define, load and assigns as many scripts as you want on any kind of scene nodes. If the script has been successfully added to our node, you should see this in the console:

```
 start update update update update update update ... 
```


In our case, `my_script:stop()` will never be called because it is never removed from its target node. You can make sure the method works by removing the script after a few frames:

```cpp
auto numFrames = 0; auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr c, float t, float dt) {

   ++numFrames;
   if (numFrames == 5)
       root->removeComponent(sceneManager->assets()->script("script/my_script.lua"));

   sceneManager->nextFrame(t, dt);

}); 
```


So you should get this in the console:

```
 start update update update update stop 
```


Final code
----------

```cpp
#include "minko/Minko.hpp" 
#include "minko/MinkoSDL.hpp" 
#include "minko/MinkoLua.hpp"

using namespace minko; 
using namespace minko::math; 
using namespace minko::component;

const uint WINDOW_WIDTH = 800; 
const uint WINDOW_HEIGHT = 600;

int main(int argc, char** argv) {

   auto canvas = Canvas::create("Minko Tutorial - My first script", WINDOW_WIDTH, WINDOW_HEIGHT);
   auto sceneManager = component::SceneManager::create(canvas);
   auto root = scene::Node::create("root")->addComponent(sceneManager);

   // initialization of Lua context
   LuaContext::initialize(argc, argv, root, canvas);

   // add a LuaScriptManager to the root node
   root->addComponent(LuaScriptManager::create());

   sceneManager->assets()->registerParser<[file::LuaScriptParser>](file::LuaScriptParser>)("lua");
   sceneManager->assets()->queue("script/my_script.lua");

   auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
   {
       root->addComponent(assets->script("script/my_script.lua"));

       auto numFrames = 0;
       auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr c, float t, float dt)
       {
           ++numFrames;
           if (numFrames == 5)
               root->removeComponent(sceneManager->assets()->script("script/my_script.lua"));

           sceneManager->nextFrame(t, dt);
       });

       canvas->run();
   });

   // actually begin loading operations
   sceneManager->assets()->load();

   return 0;

} 
```


