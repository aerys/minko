/*
Copyright (c) 2014 Aerys

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

#include <chrono>

#include "minko/Common.hpp"
#include "minko/SDLKeyboard.hpp"
#include "minko/SDLMouse.hpp"
#include "minko/SDLJoystick.hpp"
#include "minko/Signal.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/OpenGLES2Context.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/async/Worker.hpp"

#if defined(__APPLE__)
# include <TargetConditionals.h>
# if TARGET_OS_IPHONE
#  include "SDL2/SDL_main.h"
# endif
#endif

struct SDL_Window;
struct SDL_Surface;
struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;
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
        typedef std::chrono::high_resolution_clock::time_point	time_point;
		typedef std::shared_ptr<async::Worker>			        WorkerPtr;

		std::string										_name;
		uint											_x;
		uint											_y;
		uint											_width;
		uint											_height;
		std::shared_ptr<data::Provider>					_data;
		bool											_useStencil;
		bool											_chromeless;

		bool											_active;
		render::AbstractContext::Ptr					_context;
#ifdef EMSCRIPTEN
		SDL_Surface*											_screen;
#else
		SDL_Window*												_window;
#endif
		float													_relativeTime;
		float													_frameDuration;
        time_point                                              _previousTime;
        time_point                                              _startTime;
		float													_framerate;
		float													_desiredFramerate;

		std::shared_ptr<SDLMouse>								_mouse;
		std::unordered_map<int, std::shared_ptr<SDLJoystick>>	_joysticks;
        std::shared_ptr<SDLKeyboard>    						_keyboard;

		Signal<Ptr, float, float>::Ptr											_enterFrame;
		Signal<AbstractCanvas::Ptr, uint, uint>::Ptr							_resized;
		Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::Ptr		_joystickAdded;
		Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::Ptr		_joystickRemoved;
		std::list<std::shared_ptr<async::Worker>>								_activeWorkers;
		std::list<Any>															_workerCompleteSlots;


	public:
		static inline
		Ptr
		create(const std::string&	name, 
			   const uint			width, 
			   const uint			height, 
			   bool					useStencil = false,
			   bool					chromeless = false)
		{
			auto canvas = std::shared_ptr<Canvas>(new Canvas(name, width, height, useStencil, chromeless));

			canvas->initialize();

			if (_defaultCanvas == nullptr)
				_defaultCanvas = canvas;

			return canvas;
		}

		inline
		const std::string&
		name() const
		{
			return _name;
		}

		uint
		x();

		uint
		y();

		uint
		width();

		uint
		height();

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
		Signal<Ptr, float, float>::Ptr
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
		joystick(uint id)
		{
			return id < numJoysticks() ? _joysticks[id] : nullptr;
		}

		inline
		uint
		numJoysticks()
		{
			return _joysticks.size();
		}

		inline
		Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::Ptr
		joystickAdded()
		{
			return _joystickAdded;
		}

		inline
		Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::Ptr
		joystickRemoved()
		{
			return _joystickRemoved;
		}

		inline
		Signal<AbstractCanvas::Ptr, uint, uint>::Ptr
		resized()
		{
			return _resized;
		}

		inline
		minko::render::AbstractContext::Ptr
		context()
		{
			return _context;
		}

		inline
		float
		framerate()
		{
			return _framerate;
		}

		inline
		float
		desiredFramerate()
		{
			return _desiredFramerate;
		}

		inline
		void
		desiredFramerate(float desiredFramerate)
		{
			_desiredFramerate = desiredFramerate;
		}

		// Current frame execution time in milliseconds.
		inline
		float
		frameDuration() const
		{
			return _frameDuration;
		}

		// Time in milliseconds since application started.
		inline
		float
		relativeTime() const
		{
			return _relativeTime;
		}

		WorkerPtr
		getWorker(const std::string& name);

		bool
		isWorkerRegistered(const std::string& name)
		{
			if (_workers.count(name))
				return true;
			else
				return false;
		};

		void
		run();

		void
		quit();

	private:
		Canvas(const std::string&	name, 
			   const uint			width, 
			   const uint			height, 
			   bool					useStencil = false,
			   bool					chromeless = false);

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

	public:
		void
		step();
	};
}