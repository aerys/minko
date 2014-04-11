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

#if defined(CHROMIUM)
#pragma once

#include "minko/Common.hpp"
#include "minko/Signal.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/dom/AbstractDOMEngine.hpp"
#include "ChromiumDOM.hpp"
#include "include/cef_render_process_handler.h"

#include <stdio.h>  /* defines FILENAME_MAX */
#if defined(_WIN32)
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

namespace chromium
{
	class ChromiumPimpl;
	namespace dom
	{
		class ChromiumDOMEngine : public minko::dom::AbstractDOMEngine,
			public std::enable_shared_from_this<ChromiumDOMEngine>
		{
		public:
			typedef std::shared_ptr<ChromiumDOMEngine> Ptr;

		private:
			ChromiumDOMEngine();

		public:
			~ChromiumDOMEngine();
			
			static
			Ptr
			create(int argc, char** argv);

			void
			clear();
			
			void
			initialize(std::shared_ptr<minko::AbstractCanvas>, std::shared_ptr<minko::component::SceneManager>, std::shared_ptr<minko::scene::Node>);
			
			minko::dom::AbstractDOM::Ptr
			load(std::string uri);

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onload()
			{
				return _onload;
			}

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr
			onmessage()
			{
				return _onmessage;
			}

			void
			registerDom(ChromiumDOM::Ptr);

		private:			
			void
			start(int argc, char** argv);

			void
			start();

			void
			enterFrame();

			void
			loadHttp(std::string);

			void
			loadLocal(std::string);
			
		private:

			std::list<chromium::dom::ChromiumDOM::Ptr> _doms;

			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onload;
			minko::Signal<minko::dom::AbstractDOM::Ptr, std::string>::Ptr _onmessage;

			minko::AbstractCanvas::Ptr _canvas;
			minko::component::SceneManager::Ptr _sceneManager;

			minko::Signal<minko::AbstractCanvas::Ptr, minko::uint, minko::uint>::Slot _canvasResizedSlot;
			minko::Signal<minko::component::SceneManager::Ptr, float, float>::Slot _enterFrameSlot;
			minko::Signal<minko::component::SceneManager::Ptr, float, float>::Slot _endFrameSlot;

			ChromiumPimpl* _impl;
			std::shared_ptr<minko::material::BasicMaterial> _overlayMaterial;

			bool _cleared;
		};
	}
}
#endif