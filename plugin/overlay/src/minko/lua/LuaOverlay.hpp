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

#include "minko/component/Overlay.hpp"
#include "minko/MinkoLua.hpp"

#include "minko/scene/Node.hpp"

namespace minko
{
	class LuaWrapper;

	namespace component
	{
		namespace overlay
		{
			class LuaOverlay :
				public LuaWrapper
			{

			private:

			public:

				static
				std::shared_ptr<Overlay>
				extractOverlayFromNode(std::shared_ptr<scene::Node> node)
				{
					return node->component<Overlay>(0);
				}

				static
				void
				bind(LuaGlue& state)
				{
					auto scene_node = (LuaGlueClass<scene::Node>*)state.lookupClass("Node");

					scene_node->methodWrapper("getOverlay", &LuaOverlay::extractOverlayFromNode);

					auto& overlay = state.Class<Overlay>("Overlay")
						.method("create", &Overlay::create)
						.method("load", &Overlay::load)
						.method("clear", &Overlay::clear);

					MINKO_LUAGLUE_BIND_SIGNAL(state, minko::dom::AbstractDOM::Ptr, std::string);

					overlay.property("onload", &Overlay::onload);
					overlay.property("onmessage", &Overlay::onmessage);
				}

			};
		}
	}
}
