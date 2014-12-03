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
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"
#include "minko/MinkoLua.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

int
main(int argc, char** argv)
{
    auto canvas = Canvas::create("Minko Example - Lua Scripts");
    auto sceneManager = SceneManager::create(canvas);
    auto root = scene::Node::create("root")
        ->addComponent(sceneManager)
        ->addComponent(MouseManager::create(canvas->mouse()));

    auto loader = sceneManager->assets()->loader();
    loader->options()
        ->generateMipmaps(true)
        ->registerParser<file::PNGParser>("png")
        ->registerParser<file::LuaScriptParser>("lua");

    // init. lua
    LuaContext::initialize(argc, argv, root, canvas);
    root->addComponent(LuaScriptManager::create());

    // setup assets
    loader->queue("script/main.lua");

    Signal<Canvas::Ptr, float, float>::Slot nextFrame;
    Signal<file::Loader::Ptr>::Slot loaded = loader->complete()->connect([&](file::Loader::Ptr assets)
    {
        loaded = nullptr;

        nextFrame = canvas->enterFrame()->connect([&](Canvas::Ptr, float, float)
        {
            nextFrame = nullptr;
            root->addComponent(sceneManager->assets()->script("script/main.lua"));
        });
    });

    loader->load();

    auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, float time, float deltaTime)
    {
        sceneManager->nextFrame(time, deltaTime);
    });

    canvas->run();
}
