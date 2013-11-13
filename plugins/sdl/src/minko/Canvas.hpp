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

#include <ctime>

#include "minko/Common.hpp"
#include "minko/Signal.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/OpenGLES2Context.hpp"

#ifdef EMSCRIPTEN
# include "minko/MinkoWebGL.hpp"
# include "SDL/SDL.h"
# include "emscripten.h"
#elif defined(MINKO_ANGLE)
# include "SDL2/SDL.h"
# include "SDL2/SDL_syswm.h"
# include <EGL/egl.h>
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#else
# include "SDL2/SDL.h"
#endif

namespace minko
{
	class Canvas :
		public std::enable_shared_from_this<Canvas>
	{
	public:
		typedef std::shared_ptr<Canvas>	Ptr;

	private:
#ifdef EMSCRIPTEN
		static std::list<Ptr>			_canvases;
#endif

		std::string						_name;
		uint							_width;
		uint							_height;

		bool							_active;
		render::AbstractContext::Ptr	_context;
#ifndef EMSCRIPTEN
		SDL_Window*                     _window;
#endif
		float							_framerate;
		float							_desiredFramerate;

		uint							_mouseX;
		uint							_mouseY;

		Signal<Ptr, uint, uint>::Ptr	_enterFrame;
		Signal<Ptr, const Uint8*>::Ptr	_keyDown;
		Signal<Ptr, int, int, int>::Ptr	_joystickMotion;
		Signal<Ptr, int>::Ptr			_joystickButtonDown;
		Signal<Ptr, int>::Ptr			_joystickButtonUp;
		Signal<Ptr, uint, uint>::Ptr	_mouseMove;
		Signal<Ptr, uint, uint>::Ptr	_mouseLeftButtonDown;
		Signal<Ptr, uint, uint>::Ptr	_mouseLeftButtonUp;
		Signal<Ptr, int, int>::Ptr		_mouseWheel;
		Signal<Ptr, uint, uint>::Ptr	_resized;

	public:
		static inline
		Ptr
		create(const std::string& name, const uint width, const uint height, bool useStencil = false)
		{
			return std::shared_ptr<Canvas>(new Canvas(name, width, height, useStencil));
		}

		inline
		const std::string&
		name() const
		{
			return _name;
		}

		inline
		uint
		width() const
		{
			return _width;
		}

		inline
		uint
		height() const
		{
			return _height;
		}

		inline
		bool
		active() const
		{
			return _active;
		}

		inline
		Signal<Ptr, uint, uint>::Ptr
		enterFrame() const
		{
			return _enterFrame;
		}

		inline
		Signal<Ptr, const Uint8*>::Ptr
		keyDown() const
		{
			return _keyDown;
		}

		inline
		Signal<Ptr, int, int, int>::Ptr
		joystickMotion() const
		{
			return _joystickMotion;
		}

		inline
		Signal<Ptr, int>::Ptr
		joystickButtonDown() const
		{
			return _joystickButtonDown;
		}

		inline
		Signal<Ptr, int>::Ptr
		joystickButtonUp() const
		{
			return _joystickButtonUp;
		}

		inline
		Signal<Ptr, uint, uint>::Ptr
		mouseMove() const
		{
			return _mouseMove;
		}

		inline
		Signal<Ptr, uint, uint>::Ptr
		mouseLeftButtonDown() const
		{
			return _mouseLeftButtonDown;
		}

		inline
		Signal<Ptr, uint, uint>::Ptr
		mouseLeftButtonUp() const
		{
			return _mouseLeftButtonUp;
		}

		inline
		Signal<Ptr, int, int>::Ptr
		mouseWheel() const
		{
			return _mouseWheel;
		}

		inline
		Signal<Ptr, uint, uint>::Ptr
		resized() const
		{
			return _resized;
		}

		inline
		minko::render::AbstractContext::Ptr
		context() const
		{
			return _context;
		}

		inline
		float
		framerate() const
		{
			return _framerate;
		}

		inline
		float
		desiredFramerate() const
		{
			return _desiredFramerate;
		}

		inline
		void
		desiredFramerate(float desiredFramerate)
		{
			_desiredFramerate = desiredFramerate;
		}

		inline
		uint
		mouseX() const
		{
			return _mouseX;
		}

		inline
		uint
		mouseY() const
		{
			return _mouseY;
		}

		inline
		float
		normalizedMouseX() const
		{
			return 2.f * ((float)_mouseX / _width - .5f);
		}

		inline
		float
		normalizedMouseY() const
		{
			return 2.f * ((float)_mouseY / _height - .5f);
		}

		void
		run();

		void
		quit();

	private:
		Canvas(const std::string& name, const uint width, const uint height, bool useStencil = false);

		void
		initializeContext(const std::string& windowTitle, unsigned int width, unsigned int height, bool useStencil);

		void
		initializeJoysticks();

#ifdef MINKO_ANGLE
		ESContext*
		initContext(SDL_Window* window, unsigned int width, unsigned int height);
#endif

#ifdef EMSCRIPTEN
		static
		void
		emscriptenMainLoop();
#endif

		void
		step();
	};
}