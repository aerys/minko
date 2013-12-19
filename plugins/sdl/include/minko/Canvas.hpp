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
#include "minko/input/Joystick.hpp"

struct SDL_Window;
typedef unsigned char Uint8;

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

		class SDLJoystick : 
			public input::Joystick
		{
			friend class Canvas;

		public :
			static inline
			std::shared_ptr<SDLJoystick>
			create(Canvas::Ptr canvas)
			{
				return std::shared_ptr<SDLJoystick>(new SDLJoystick(canvas));
			}
		private:
			SDLJoystick(Canvas::Ptr canvas) :
				input::Joystick(canvas)
			{
			}
		};

	private:
#ifdef EMSCRIPTEN
		static std::list<Ptr>						_canvases;
#endif
		
		std::string									_name;
		std::shared_ptr<data::Provider>				_data;
		bool										_useStencil;


		bool										_active;
		render::AbstractContext::Ptr				_context;
#ifndef EMSCRIPTEN
		SDL_Window*									_window;
#endif
		float										_framerate;
		float										_desiredFramerate;

		Signal<Ptr, uint, uint>::Ptr				_enterFrame;
		Signal<Ptr, uint, uint>::Ptr				_resized;
		std::shared_ptr<SDLMouse>					_mouse;
		std::vector<std::shared_ptr<SDLJoystick>>	_joysticks;
        std::shared_ptr<input::Keyboard>			_keyboard;

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
		x() const;

		inline
		uint
		y() const;

		inline
		uint
		width() const;

		inline
		uint
		height() const;

		inline
		std::shared_ptr<data::Provider>
		data() const
		{
			return _data;
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
		std::shared_ptr<input::Mouse>
		mouse()
		{
			return _mouse;
		}

        inline
        std::shared_ptr<input::Keyboard>
        keyboard()
        {
            return _keyboard;
        }
		
		inline
		std::shared_ptr<input::Joystick>
        joystick(int id)
        {
			return _joysticks[id];
        }

		inline
		uint
		numJoysticks()
		{
			return _joysticks.size();
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
			return 2.f * ((float)x() / width() - .5f);
		}

		inline
		float
		normalizedMouseY() const
		{
			return 2.f * ((float)y() / height() - .5f);
		}

		void
		run();

		void
		quit();

	private:
		Canvas(const std::string& name, const uint width, const uint height, bool useStencil = false);

		void
		x(uint);

		void
		y(uint);

		void
		width(uint);

		void 
		height(uint);

		void
		initialize();

		void
		initializeInputs();

		void
		initializeContext(const std::string& windowTitle, unsigned int width, unsigned int height, bool useStencil);

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