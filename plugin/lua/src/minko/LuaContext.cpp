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

#include "minko/LuaContext.hpp"

#include "minko/component/SceneManager.hpp"
#include "minko/input/Keyboard.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/scene/Node.hpp"

using namespace minko;

int LuaContext::_argc = 0;
char** LuaContext::_argv = NULL;
std::shared_ptr<scene::Node> LuaContext::_root = nullptr;
std::shared_ptr<AbstractCanvas> LuaContext::_canvas = nullptr;

void
LuaContext::initialize(int argc, char** argv, std::shared_ptr<scene::Node> root, std::shared_ptr<AbstractCanvas> canvas)
{
    _argc = argc;
    _argv = argv;
    _root = root;
    _canvas = canvas;
}

std::shared_ptr<AbstractCanvas>
LuaContext::getCanvas()
{
    return _canvas;
}

std::shared_ptr<component::SceneManager>
LuaContext::getSceneManager()
{
    return _root->component<component::SceneManager>();
}

bool
LuaContext::getOption(const std::string& optionName)
{
    for (auto i = 0; i < _argc; ++i)
        if (optionName == std::string(_argv[i]))
            return true;

    return false;
}
