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
#include "minko/component/AbstractComponent.hpp"

#include "LuaGlue/LuaGlue.h"

#include "minko/Signal.hpp"

class LuaGlue;
class lua_State;

namespace minko
{
	namespace component
	{
		class LuaScriptManager :
			public AbstractComponent,
			public std::enable_shared_from_this<LuaScriptManager>
		{
			friend class LuaScript;

		public:
			typedef std::shared_ptr<LuaScriptManager> 	Ptr;

		private:
			class LuaGlobalStub
			{
			public:
				static std::shared_ptr<scene::Node>	_root;

				static
				std::shared_ptr<SceneManager>
				getSceneManager();

				static
				std::shared_ptr<input::Mouse>
				getMouse();

				static
				std::shared_ptr<input::Keyboard>
				getKeyboard();

				static
				std::shared_ptr<math::Matrix4x4>
				getModelToWorldMatrix(std::shared_ptr<scene::Node> n);
			};

		private:
			LuaGlue*															_state;
			Signal<AbstractComponent::Ptr, std::shared_ptr<scene::Node>>::Slot 	_targetAddedSlot;

		public:
			inline static
			Ptr
			create()
			{
				auto sm = std::shared_ptr<LuaScriptManager>(new LuaScriptManager());

				sm->initialize();

				return sm;
			}

		private:
			void
			targetAddedHandler(AbstractComponent::Ptr cmp, std::shared_ptr<scene::Node> target);

			void
			initialize();

			void
			initializeBindings();

			static
			int
			stubSelfTest(Signal<std::shared_ptr<file::AssetLibrary>>::Ptr s, std::shared_ptr<LuaGlueFunctionRef> p);

			template <typename... Args>
			static
			typename Signal<Args...>::Slot
			wrapSignalConnect(typename Signal<Args...>::Ptr s, std::shared_ptr<LuaGlueFunctionRef> p)
			{
				return s->connect([=](Args... args) { p->invokeVoid(args...); });
			}
		};
	}
}
