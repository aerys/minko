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

#include "minko/dom/AbstractDOM.hpp"
#include "minko/MinkoLua.hpp"

namespace minko
{
	class LuaWrapper;

	namespace component
	{
		namespace overlay
		{
			class LuaAbstractDOM :
				public LuaWrapper
			{

			private:

			public:

				static
				void
				bind(LuaGlue& state)
				{
					auto& abstractDom = state.Class<dom::AbstractDOM>("AbstractDOM")
						.method("createElement", &dom::AbstractDOM::createElement)
						.method("getElementById", &dom::AbstractDOM::getElementById)
						.method("getElementsByClassName", &dom::AbstractDOM::getElementsByClassName)
						.method("getElementsByTagName", &dom::AbstractDOM::getElementsByTagName)
						.method("document", &dom::AbstractDOM::document)
						.method("body", &dom::AbstractDOM::body)
						.method("fileName", &dom::AbstractDOM::fileName)
						.method("fullUrl", &dom::AbstractDOM::fullUrl)
						.method("isMain", &dom::AbstractDOM::isMain)
						.method("sendMessage", &dom::AbstractDOM::sendMessage)
						.method("eval", &dom::AbstractDOM::eval);

						MINKO_LUAGLUE_BIND_SIGNAL(state, minko::dom::AbstractDOM::Ptr, std::string);

						abstractDom.property("onload", &dom::AbstractDOM::onload);
						//abstractDom.property("onmessage", &dom::AbstractDOM::onmessage);
					}

			};
		}
	}
}
