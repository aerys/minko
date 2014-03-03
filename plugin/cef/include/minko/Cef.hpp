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
#include "minko/render/AbstractContext.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/AbstractCanvas.hpp"

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

namespace minko
{
	class CefPimpl;

	class Cef
	{
	public:
		Cef();
		
		bool
		load(int argc, char** argv);

		void
		unload();
		
		void
		initialize(std::shared_ptr<AbstractCanvas>, std::shared_ptr<component::SceneManager>);

		void
		setURL(std::string);

		void
		loadLocal(std::string);

		void
		executeJavascript(std::string);
		
		void
		setHTML(std::string);


		int cefProcessResult;

		Signal<std::string>::Ptr messageReceived();

	private:

		void
		enterFrame();

		std::string
		getWorkingDirectory()
		{
			if (!GetCurrentDir(_currentPath, sizeof(_currentPath)))
			{
				return "";
			}
			_currentPath[sizeof(_currentPath)-1] = '\0'; /* not really required */

			return std::string(_currentPath);
		}


		std::shared_ptr<AbstractCanvas> _canvas;
		std::shared_ptr<render::AbstractContext> _context;
		std::shared_ptr<component::SceneManager> _sceneManager;

		Signal<std::shared_ptr<AbstractCanvas>, uint, uint>::Slot _canvasResizedSlot;
		Signal<std::shared_ptr<component::SceneManager>>::Slot _enterFrameSlot;

		CefPimpl* _impl;
		std::shared_ptr<material::BasicMaterial> _overlayMaterial;

		char _currentPath[FILENAME_MAX];
	};
}