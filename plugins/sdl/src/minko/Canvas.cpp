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

#include "minko/Canvas.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/input/Keyboard.hpp"

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

using namespace minko;

Canvas::Canvas(const std::string& name, const uint width, const uint height, bool useStencil) :
	_name(name),
	_width(width),
	_height(height),
	_useStencil(useStencil),
	_active(false),
	_framerate(0.f),
	_desiredFramerate(60.f),
	_enterFrame(Signal<Canvas::Ptr, uint, uint>::create()),
	_resized(Signal<Canvas::Ptr, uint, uint>::create())
{
}

void
Canvas::initialize()
{
	initializeContext(_name, _width, _height, _useStencil);
	initializeInputs();
}

void
Canvas::initializeInputs()
{
	_mouse		= Canvas::SDLMouse::create(shared_from_this());
    _keyboard	= input::Keyboard::create();

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        SDL_Joystick* joystick = SDL_JoystickOpen(i);

        if (!joystick)
            continue;
		else
			_joysticks.push_back(Canvas::SDLJoystick::create(shared_from_this()));
    }
}

void
Canvas::initializeContext(const std::string& windowTitle, unsigned int width, unsigned int height, bool useStencil)
{
#ifndef EMSCRIPTEN
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

	if (useStencil)
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

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

	if (useStencil)
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_WM_SetCaption(windowTitle.c_str(), NULL);
	SDL_Surface* screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL);

	_context = minko::render::WebGLContext::create();
#endif // EMSCRIPTEN
}

#ifdef MINKO_ANGLE
ESContext*
Canvas::initContext(SDL_Window* window, unsigned int width, unsigned int height)
{
	EGLint configAttribList[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_STENCIL_SIZE, 8,
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
	if (display == EGL_NO_DISPLAY)
	{
		return EGL_FALSE;
	}

	// Initialize EGL
	if (!eglInitialize(display, &majorVersion, &minorVersion))
	{
		return EGL_FALSE;
	}

	// Get configs
	if (!eglGetConfigs(display, NULL, 0, &numConfigs))
	{
		return EGL_FALSE;
	}

	// Choose config
	if (!eglChooseConfig(display, configAttribList, &config, 1, &numConfigs))
	{
		return EGL_FALSE;
	}

	// Create a surface
	surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hWnd, surfaceAttribList);
	if (surface == EGL_NO_SURFACE)
	{
		return EGL_FALSE;
	}

	// Create a GL context
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
	if (context == EGL_NO_CONTEXT)
	{
		return EGL_FALSE;
	}

	// Make the context current
	if (!eglMakeCurrent(display, surface, surface, context))
	{
		return EGL_FALSE;
	}
	es_context->eglDisplay = display;
	es_context->eglSurface = surface;
	es_context->eglContext = context;

	return es_context;
}
#endif

void
Canvas::step()
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

			//_keyDown->execute(shared_from_this(), keyboardState);
            _keyboard->keyDown()->execute(_keyboard, keyboardState);
			break;
		}

        case SDL_KEYUP:
        {
            const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

            //_keyDown->execute(shared_from_this(), keyboardState);
            _keyboard->keyUp()->execute(_keyboard, keyboardState);
            break;
        }

		case SDL_MOUSEMOTION:
		{
			auto oldX = mouse()->x();
			auto oldY = mouse()->y();

			_mouse->x(event.motion.x);
			_mouse->y(event.motion.y);
			_mouse->move()->execute(_mouse, event.motion.x - oldX, event.motion.y - oldY);
			//_mouseX = event.motion.x;
			//_mouseY = event.motion.y;
			//_mouseMove->execute(shared_from_this(), _mouseX, _mouseY);
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
			_mouse->leftButtonDown()->execute(_mouse);
			//_mouseLeftButtonDown->execute(shared_from_this(), event.motion.x, event.motion.y);
			break;

		case SDL_MOUSEBUTTONUP:
			_mouse->leftButtonUp()->execute(_mouse);
			//_mouseLeftButtonUp->execute(shared_from_this(), event.motion.x, event.motion.y);
			break;

		case SDL_MOUSEWHEEL:
			_mouse->wheel()->execute(_mouse, event.wheel.x, event.wheel.y);
			//_mouseWheel->execute(shared_from_this(), event.wheel.x, event.wheel.y);
			break;

		case SDL_JOYAXISMOTION:
			_joysticks[event.jaxis.which]->joystickAxisMotion()->execute(_joysticks[event.jaxis.which], event.jaxis.which, event.jaxis.axis, event.jaxis.value);
			break;

		case SDL_JOYBUTTONDOWN:
			_joysticks[event.jaxis.which]->joystickButtonDown()->execute(_joysticks[event.jaxis.which], event.button.which, event.jbutton.button);
			break;

		case SDL_JOYBUTTONUP:
			_joysticks[event.jaxis.which]->joystickButtonUp()->execute(_joysticks[event.jaxis.which], event.button.which, event.jbutton.button);
			break;

		case SDL_JOYHATMOTION:
			_joysticks[event.jaxis.which]->joystickHatMotion()->execute(_joysticks[event.jaxis.which], event.jhat.which, event.jhat.hat, event.jhat.value);
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				_width = event.window.data1;
				_height = event.window.data2;
				_context->configureViewport(0, 0, _width, _height);
				_resized->execute(shared_from_this(), _width, _height);
				break;
			default:
				break;
			}

			break;

		default:
			break;
		}
	}

	auto time = std::clock();
	auto frameTime = (1000.f * (time - stepStartTime) / CLOCKS_PER_SEC);

	_enterFrame->execute(shared_from_this(), (uint)time, (uint)frameTime);

	// swap buffers
#ifdef MINKO_ANGLE
	eglSwapBuffers(_angleContext->eglDisplay, _angleContext->eglSurface);
#elif defined EMSCRIPTEN
	SDL_GL_SwapBuffers();
#else
	SDL_GL_SwapWindow(_window);
#endif

	_framerate = 1000.f / frameTime;

	if (_framerate > _desiredFramerate)
		SDL_Delay((uint)((1000.f / _desiredFramerate) - frameTime));
}

void
Canvas::run()
{
	_active = true;
	_framerate = 0.f;

#ifdef EMSCRIPTEN
	_canvases.push_back(shared_from_this());
	if (_canvases.size() == 1)
		emscripten_set_main_loop(myUglyLoop, 0, 1);
#else
	while (_active)
		step();
#endif
}

#ifdef EMSCRIPTEN
void
Canvas::emscriptenMainLoop()
{
	for (auto& canvas : _canvases)
		canvas->step();
}
#endif

void
Canvas::quit()
{
	_active = false;
}
