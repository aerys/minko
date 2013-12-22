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
#include "minko/component/AbstractScript.hpp"

#include "LuaGlue/LuaGlue.h"

#include "minko/Signal.hpp"
#include "minko/input/Keyboard.hpp"

#include <chrono>

class LuaGlue;
class lua_State;

namespace minko
{
	namespace component
	{
		class LuaScriptManager :
			public AbstractScript
		{
			friend class LuaScript;

		public:
			typedef std::shared_ptr<LuaScriptManager> 		Ptr;

		private:
			typedef std::shared_ptr<file::AbstractLoader>			AbsLoaderPtr;
			typedef std::chrono::high_resolution_clock::time_point	time_point;

		private:
			class LuaGlobalStub
			{
			public:
				static std::shared_ptr<scene::Node>		_root;
				static std::shared_ptr<AbstractCanvas>	_canvas;

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
			std::shared_ptr<AbstractCanvas>			_canvas;
			LuaGlue									_state;
			time_point								_previousTime;

			bool									_ready;
			uint									_numDependencies;
			uint									_numLoadedDependencies;
			std::list<Signal<AbsLoaderPtr>::Slot>	_dependencySlots;

		public:
			inline static
			Ptr
			create(std::shared_ptr<AbstractCanvas> canvas)
			{
				auto sm = std::shared_ptr<LuaScriptManager>(new LuaScriptManager(canvas));

				sm->initialize();

				return sm;
			}

			inline
			bool
			ready(std::shared_ptr<scene::Node> node)
			{
				return _numDependencies == _numLoadedDependencies;
			}

			void
			update(std::shared_ptr<scene::Node> target);

		private:
			LuaScriptManager(std::shared_ptr<AbstractCanvas> canvas) :
				_canvas(canvas),
				_state(),
				_ready(false),
				_numDependencies(0),
				_numLoadedDependencies(0)
			{

			}

			void
			targetAddedHandler(AbstractComponent::Ptr cmp, std::shared_ptr<scene::Node> target);

			void
			initialize();

			void
			initializeBindings();

			void
			loadStandardLibrary();

			void
			dependencyLoadedHandler(AbsLoaderPtr loader);

			static
			Signal<input::Keyboard::Ptr, uint>::Ptr
			wrapKeyboardKeyDown(input::Keyboard::Ptr k, uint s);

			static
			Signal<input::Keyboard::Ptr, uint>::Ptr
			wrapKeyboardKeyUp(input::Keyboard::Ptr k, uint s);
		};
	}
}
