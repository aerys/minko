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

#include "minko/file/LuaLoader.hpp"

using namespace minko;
using namespace minko::file;

void
LuaLoader::bind(LuaGlue& state)
{
    auto& batchLoader = state.Class<Loader>("Loader");

    MINKO_LUAGLUE_BIND_SIGNAL(state, Loader::Ptr);
    MINKO_LUAGLUE_BIND_SIGNAL(state, Loader::Ptr, const Error&);
    MINKO_LUAGLUE_BIND_SIGNAL(state, Loader::Ptr, float);

    batchLoader
        .method("create",           static_cast<Loader::Ptr(*)(void)>(&Loader::create))
        .method("createCopy",       static_cast<Loader::Ptr(*)(Loader::Ptr)>(&Loader::create))
        .method("queue",            static_cast<Loader::Ptr (Loader::*)(const std::string&)>(&Loader::queue))
        .method("load",             &Loader::load)
        .property("complete",       &Loader::complete)
        .property("progress",       &Loader::progress)
        .property("error",          &Loader::error);
}
