/*
Copyright (c) 2013 Aerys

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

#include "LUAScriptParser.hpp"

#include "minko/component/LUAScript.hpp"
#include "minko/file/AssetLibrary.hpp"

#include "lua.hpp"
#include "lualib.h"
#include "lauxlib.h"

#define constexpr const
#define uint unsigned int
#include "LuaGlue/LuaGlue.h"
#undef constexpr
#undef uint

#include "minko/scene/Node.hpp"

using namespace minko;
using namespace minko::file;

void
LUAScriptParser::parse(const std::string&				    filename,
                       const std::string&                   resolvedFilename,
                       std::shared_ptr<Options>             options,
                       const std::vector<unsigned char>&	data,
                       std::shared_ptr<AssetLibrary>	    assetLibrary)
{
    static LuaGlue state;

    state
        .Class<scene::Node>("Node")
//            .method<const std::string&, void>("getName", &scene::Node::name)
//            .method<const std::string&>("setName", &scene::Node::name)
//              .method("setName", &scene::Node::name)
        .end()
        .open()
        .glue();

    if (luaL_loadstring(state.state(), std::string((char*)&data[0], data.size()).c_str()))
    {
        auto error = lua_tostring(state.state(), -1);

        std::cerr << error << std::endl;
        throw std::runtime_error(error);
    }
    if (lua_pcall(state.state(), 0, 0, 0))
    {
        auto error = lua_tostring(state.state(), -1);

        std::cerr << error << std::endl;
        throw std::runtime_error(error);
    }

    auto sepPos = resolvedFilename.find_last_of("/\\");
    auto dotPos = resolvedFilename.find_last_of('.');
    auto start = sepPos == std::string::npos ? 0 : sepPos + 1;
    auto length = dotPos - start;
    auto scriptName = resolvedFilename.substr(start, length);

    auto script = component::LUAScript::create(state.state(), scriptName);

    assetLibrary->script(filename, script);

    _complete->execute(shared_from_this());
}
