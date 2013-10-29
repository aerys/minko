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

#include "minko/Minko.hpp"
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

class MinkoSDL
{
private:
	typedef unsigned int    uint;

private:
	static bool                                 _active;
	static minko::render::AbstractContext::Ptr  _context;
#ifndef EMSCRIPTEN
	static SDL_Window*                          _window;
#endif
	static float                                _framerate;

	static minko::Signal<>::Ptr                 _enterFrame;
	static minko::Signal<const Uint8*>::Ptr     _keyDown;
	static minko::Signal<int, int, int>::Ptr    _joystickMotion;
	static minko::Signal<int>::Ptr              _joystickButtonDown;
	static minko::Signal<int>::Ptr              _joystickButtonUp;
	static minko::Signal<uint, uint>::Ptr       _mouseMove;
	static minko::Signal<uint, uint>::Ptr       _mouseLeftButtonDown;
	static minko::Signal<uint, uint>::Ptr       _mouseLeftButtonUp;
	static minko::Signal<uint, uint>::Ptr       _resized;

#ifdef MINKO_ANGLE
	typedef struct
	{
		/// Window width
		GLint               width;
		/// Window height
		GLint               height;
		/// Window handle
		EGLNativeWindowType  hWnd;
		/// EGL display
		EGLDisplay          eglDisplay;
		/// EGL context
		EGLContext          eglContext;
		/// EGL surface
		EGLSurface          eglSurface;
	} ESContext;

	static ESContext*   _angleContext;
#endif


public:
	inline static
	bool
	active()
	{
		return _active;
	}

	inline static
	minko::Signal<>::Ptr
	enterFrame()
	{
		return _enterFrame;
	}

	inline static
	minko::Signal<const Uint8*>::Ptr
	keyDown()
	{
		return _keyDown;
	}

	inline static
	minko::Signal<int, int, int>::Ptr
	joystickMotion()
	{
		return _joystickMotion;
	}

	inline static
	minko::Signal<int>::Ptr
	joystickButtonDown()
	{
		return _joystickButtonDown;
	}

	inline static
	minko::Signal<int>::Ptr
	joystickButtonUp()
	{
		return _joystickButtonUp;
	}

	inline static
	minko::Signal<uint, uint>::Ptr
	mouseMove()
	{
		return _mouseMove;
	}

	inline static
	minko::Signal<uint, uint>::Ptr
	mouseLeftButtonDown()
	{
		return _mouseLeftButtonDown;
	}

	inline static
	minko::Signal<uint, uint>::Ptr
	mouseLeftButtonUp()
	{
		return _mouseLeftButtonUp;
	}

	inline static
	minko::Signal<uint, uint>::Ptr
	resized()
	{
		return _resized;
	}

	inline static
	minko::render::AbstractContext::Ptr
	context()
	{
		return _context;
	}

	inline static
	float
	framerate()
	{
		return _framerate;
	}

	static
	void
	run()
	{
		_active = true;
		_framerate = 0.f;

#ifdef EMSCRIPTEN
		emscripten_set_main_loop(MinkoSDL::step, 0, 1);
#else
		while (_active)
			step();
#endif
	}

	static
	void
	initialize(const std::string& windowTitle, unsigned int width = 0, unsigned int height = 0)
	{
		_active = false;
		_framerate = 0.f;

		_enterFrame = minko::Signal<>::create();

		_keyDown = minko::Signal<const Uint8*>::create();

		_joystickMotion = minko::Signal<int, int, int>::create();
		_joystickButtonDown = minko::Signal<int>::create();
		_joystickButtonUp = minko::Signal<int>::create();

		_mouseMove = minko::Signal<uint, uint>::create();
		_mouseLeftButtonDown = minko::Signal<uint, uint>::create();
		_mouseLeftButtonUp = minko::Signal<uint, uint>::create();

		_resized = minko::Signal<uint, uint>::create();

		initializeContext(windowTitle, width, height);
		initializeJoysticks();
	}

private:
	static
	void
	step()
	{
		auto stepStartTime = std::clock();

		SDL_Event event;

		while (SDL_PollEvent(&event))
		{

			switch (event.type)
			{
			case SDL_QUIT:
				_active = false;
				break;

			case SDL_KEYDOWN:
			{
				const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

				_keyDown->execute(keyboardState);
				break;
			}

			case SDL_MOUSEMOTION:
				_mouseMove->execute(event.motion.x, event.motion.y);
				break;

			case SDL_MOUSEBUTTONDOWN:
				_mouseLeftButtonDown->execute(event.motion.x, event.motion.y);
				break;

			case SDL_MOUSEBUTTONUP:
				_mouseLeftButtonUp->execute(event.motion.x, event.motion.y);
				break;

			case SDL_JOYAXISMOTION:
				_joystickMotion->execute(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
				break;

			case SDL_JOYBUTTONDOWN:
				_joystickButtonDown->execute(event.jbutton.button);
				break;

			case SDL_JOYBUTTONUP:
				_joystickButtonUp->execute(event.jbutton.button);
				break;

			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
					_context->configureViewport(0, 0, event.window.data1, event.window.data2);
					_resized->execute(event.window.data1, event.window.data2);
					break;
				default:
					break;
				}

				break;

			default:
				break;
			}
		}


		_enterFrame->execute();

		// swap buffers
#ifdef MINKO_ANGLE
		eglSwapBuffers(_angleContext->eglDisplay, _angleContext->eglSurface);
#elif defined EMSCRIPTEN
		SDL_GL_SwapBuffers();
#else
		SDL_GL_SwapWindow(_window);
#endif

		_framerate = 1000.f / (1000.f * (std::clock() - stepStartTime) / CLOCKS_PER_SEC);
	}

	static
	void
	initializeJoysticks()
	{
		for (int i = 0; i < SDL_NumJoysticks(); ++i)
		{
			SDL_Joystick* joystick = SDL_JoystickOpen(i);

			if (!joystick)
				continue;
		}

	}

	static
	void
	initializeContext(const std::string& windowTitle, unsigned int width, unsigned int height)
	{
#ifndef EMSCRIPTEN
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

		_window = SDL_CreateWindow(
		              windowTitle.c_str(),
		              SDL_WINDOWPOS_UNDEFINED,
		              SDL_WINDOWPOS_UNDEFINED,
		              width, height,
		              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
		          );

# ifdef MINKO_ANGLE
		if (!(_angleContext = initContext(_window, width, height)))
			throw std::runtime_error("Could not create eglContext");
# else
		SDL_GLContext glcontext = SDL_GL_CreateContext(_window);
# endif // MINKO_ANGLE

		_context = minko::render::OpenGLES2Context::create();
#else
		//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		SDL_Init(SDL_INIT_VIDEO);
		SDL_WM_SetCaption(windowTitle.c_str(), NULL);
		SDL_Surface* screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL);

		_context = minko::render::WebGLContext::create();
#endif // EMSCRIPTEN
	}

#ifdef MINKO_ANGLE
	static
	ESContext*
	initContext(SDL_Window* window, unsigned int width, unsigned int height)
	{
		EGLint configAttribList[] =
		{
			EGL_RED_SIZE,       8,
			EGL_GREEN_SIZE,     8,
			EGL_BLUE_SIZE,      8,
			EGL_ALPHA_SIZE,     8,
			EGL_DEPTH_SIZE,     16,
			EGL_STENCIL_SIZE,   8,
			EGL_SAMPLE_BUFFERS, 0,
			EGL_NONE
		};
		EGLint surfaceAttribList[] =
		{
			EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
			EGL_NONE, EGL_NONE
		};

		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		if (!SDL_GetWindowWMInfo(window, &info))
			return GL_FALSE;
		EGLNativeWindowType hWnd = info.info.win.window;

		ESContext* es_context = new ESContext();
		es_context->width = width;
		es_context->height = height;
		es_context->hWnd = hWnd;

		EGLDisplay display;
		EGLint numConfigs;
		EGLint majorVersion;
		EGLint minorVersion;
		EGLContext context;
		EGLSurface surface;
		EGLConfig config;
		EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

		display = eglGetDisplay(GetDC(hWnd)); // EGL_DEFAULT_DISPLAY
		if ( display == EGL_NO_DISPLAY )
		{
			return EGL_FALSE;
		}

		// Initialize EGL
		if ( !eglInitialize(display, &majorVersion, &minorVersion) )
		{
			return EGL_FALSE;
		}

		// Get configs
		if ( !eglGetConfigs(display, NULL, 0, &numConfigs) )
		{
			return EGL_FALSE;
		}

		// Choose config
		if ( !eglChooseConfig(display, configAttribList, &config, 1, &numConfigs) )
		{
			return EGL_FALSE;
		}

		// Create a surface
		surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hWnd, surfaceAttribList);
		if ( surface == EGL_NO_SURFACE )
		{
			return EGL_FALSE;
		}

		// Create a GL context
		context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
		if ( context == EGL_NO_CONTEXT )
		{
			return EGL_FALSE;
		}

		// Make the context current
		if ( !eglMakeCurrent(display, surface, surface, context) )
		{
			return EGL_FALSE;
		}
		es_context->eglDisplay = display;
		es_context->eglSurface = surface;
		es_context->eglContext = context;

		return es_context;
	}
#endif
};
