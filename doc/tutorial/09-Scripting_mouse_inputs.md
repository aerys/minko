Introduction to mouse inputs
----------------------------

The following section introduces the two different ways to deal with mouse inputs (or any kind of interactions based on signals for that matter): signals callbacks and coroutines. You are free to choose one or the other according to the situation.

### Listening to mouse signals

```lua
-- my_mouse_script.lua function my_mouse_script:start(node)

 local mouse = getCanvas().mouse

 mouse.leftButtonDown:connect(function(m) print('left button down!') end)
 mouse.leftButtonUp:connect(function(m) print('left button up!') end)
 mouse.move:connect(function(m, dx, dy) print('mouse move!') end)

end 
```


### Alternative: Waiting for mouse signals

Coroutines are a great way to work with an asynchronous API using a synchronous syntax. Instead of listening to a specific signal and act accordingly in the corresponding callback, we will simply pause the script and wait for "something to happen" before resuming.

The following code will "wait" for the `mouse.leftButtonDown` signal before continuing:

```lua
function my_mouse_script:start(node)

 self.co = coroutine.create(my_mouse_script.handleMouseDown)
 coroutine.resume(self.co, self)

end

function my_mouse_script:handleMouseDown()

 print("please click...")
 -- execution of this specific script will pause when it hits the call to wait()
 wait(getCanvas().mouse.leftButtonDown)
 -- execution will resume here when the mouse.leftButtonDown has been executed
 print("left button down!")

end 
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

   auto canvas = Canvas::create("Minko Tutorial - Scripting mouse inputs", WINDOW_WIDTH, WINDOW_HEIGHT);
   auto sceneManager = component::SceneManager::create(canvas);
   auto root = scene::Node::create("root")->addComponent(sceneManager);

   // initialization of Lua context
   LuaContext::initialize(argc, argv, root, canvas);

   // add a LuaScriptManager to the root node
   root->addComponent(LuaScriptManager::create());

   sceneManager->assets()->registerParser<[file::LuaScriptParser>](file::LuaScriptParser>)("lua");
   sceneManager->assets()->queue("script/my_mouse_script.lua");

   auto complete = sceneManager->assets()->complete()->connect([&](file::AssetLibrary::Ptr assets)
   {
       root->addComponent(assets->script("script/my_mouse_script.lua"));

       auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr c, float t, float dt)
       {
           sceneManager->nextFrame(t, dt);
       });

       canvas->run();
   });

   // actually begin loading operations
   sceneManager->assets()->load();

   return 0;

} 
```


