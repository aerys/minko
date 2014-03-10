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

#include "minko/file/LuaBatchLoader.hpp"

using namespace minko;
using namespace minko::file;

void
LuaBatchLoader::bind(LuaGlue& state)
{
    auto& batchLoader = state.Class<BatchLoader>("BatchLoader");

    MINKO_LUAGLUE_BIND_SIGNAL(state, AbstractLoader::Ptr);
    MINKO_LUAGLUE_BIND_SIGNAL(state, AbstractLoader::Ptr, float);

    batchLoader
        .method("create",       static_cast<BatchLoader::Ptr(*)(void)>(&BatchLoader::create))
        .method("createCopy",   static_cast<BatchLoader::Ptr(*)(BatchLoader::Ptr)>(&BatchLoader::create))
        .method("queue",        static_cast<BatchLoader::Ptr (BatchLoader::*)(const std::string&)>(&BatchLoader::queue))
        .method("load",         &BatchLoader::load)
        .property("complete",   &BatchLoader::complete)
        .property("progress",   &BatchLoader::progress)
        .property("error",      &BatchLoader::error);
}
