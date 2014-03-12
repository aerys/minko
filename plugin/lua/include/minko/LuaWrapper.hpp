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

#pragma once

#include "minko/Common.hpp"
#include "minko/Signal.hpp"

#include "LuaGlue/LuaGlue.h"

#define MINKO_LUAGLUE_BIND_SIGNAL(state, ...) \
    { \
        std::string argsStr(#__VA_ARGS__); \
        std::replace(argsStr.begin(), argsStr.end(), ':', '_'); \
        std::replace(argsStr.begin(), argsStr.end(), ',', '_'); \
        std::replace(argsStr.begin(), argsStr.end(), ' ', '_'); \
        std::replace(argsStr.begin(), argsStr.end(), '&', '_'); \
        state.Class<Signal<__VA_ARGS__>::Slot::element_type>("SignalSlot" + argsStr) \
            .method("disconnect", &Signal<__VA_ARGS__>::Slot::element_type::disconnect); \
        state.Class<Signal<__VA_ARGS__>>("Signal" + argsStr) \
            .methodWrapper("connect", &LuaWrapper::wrapSignalConnect<__VA_ARGS__>); \
    }

namespace minko
{
	class LuaWrapper
	{
	public:
		template <typename... Args>
		static
		typename Signal<Args...>::Slot
		wrapSignalConnect(typename Signal<Args...>::Ptr s, std::shared_ptr<LuaGlueFunctionRef> p)
		{
			return s->connect([=](Args... args) { p->invokeVoid(args...); });
		}
	};
}
