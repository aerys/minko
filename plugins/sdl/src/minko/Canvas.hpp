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
#include "minko/AbstractCanvas.hpp"
#include "minko/input/Mouse.hpp"

#if defined(EMSCRIPTEN)
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
		public AbstractCanvas,
		public std::enable_shared_from_this<Canvas>
	{
	public:
		typedef std::shared_ptr<Canvas>	Ptr;

	private:
		class SDLMouse :
			public input::Mouse
		{
			friend class Canvas;

		public:
			static inline
			std::shared_ptr<SDLMouse>
			create(Canvas::Ptr canvas)
			{
				return std::shared_ptr<SDLMouse>(new SDLMouse(canvas));
			}

		private:
			SDLMouse(Canvas::Ptr canvas) :
				input::Mouse(canvas)
			{
			}

			void
			x(uint x)
			{
				_x = x;
			}

			void
			y(uint y)
			{
				_y = y;
			}
		};

	private:
#ifdef EMSCRIPTEN
		static std::list<Ptr>				_canvases;
#endif

		std::string							_name;
		uint								_x;
		uint								_y;
		uint								_width;
		uint								_height;
		bool								_useStencil;

		bool								_active;
		render::AbstractContext::Ptr		_context;
#ifndef EMSCRIPTEN
		SDL_Window*							_window;
#endif
		float								_framerate;
		float								_desiredFramerate;

		Signal<Ptr, uint, uint>::Ptr		_enterFrame;
		Signal<Ptr, const Uint8*>::Ptr		_keyDown;
		Signal<Ptr, int, int, int>::Ptr		_joystickMotion;
		Signal<Ptr, int>::Ptr				_joystickButtonDown;
		Signal<Ptr, int>::Ptr				_joystickButtonUp;
		Signal<Ptr, uint, uint>::Ptr		_resized;
		std::shared_ptr<SDLMouse>			_mouse;

	public:
		static inline
		Ptr
		create(const std::string& name, const uint width, const uint height, bool useStencil = false)
		{
			auto canvas = std::shared_ptr<Canvas>(new Canvas(name, width, height, useStencil));

			canvas->initialize();

			return canvas;
		}

		inline
		const std::string&
		name() const
		{
			return _name;
		}

		inline
		uint
		x()
		{
			return _x;
		}

		inline
		uint
		y()
		{
			return _y;
		}

		inline
		uint
		width()
		{
			return _width;
		}

		inline
		uint
		height()
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
		std::shared_ptr<input::Mouse>
		mouse()
		{
			return _mouse;
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
		float
		normalizedMouseX() const
		{
			return 2.f * ((float)_x / _width - .5f);
		}

		inline
		float
		normalizedMouseY() const
		{
			return 2.f * ((float)_y / _height - .5f);
		}

		void
		run();

		void
		quit();

	private:
		Canvas(const std::string& name, const uint width, const uint height, bool useStencil = false);

		void
		initialize();

		void
		initializeMouse();

		void
		initializeContext(const std::string& windowTitle, unsigned int width, unsigned int height, bool useStencil);

		void
		initializeJoysticks();

#ifdef MINKO_ANGLE
		ESContext*
		initContext(SDL_Window* window, unsigned int width, unsigned int height);
#endif

#ifdef EMSCRIPTEN
	public:
		static
		void
		emscriptenMainLoop();
#endif

		void
		step();
	};
}