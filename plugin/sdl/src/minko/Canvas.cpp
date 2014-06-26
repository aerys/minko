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

#include "minko/Canvas.hpp"
#include "minko/data/Provider.hpp"
#include "minko/async/Worker.hpp"
#include "minko/file/Options.hpp"

#if !defined(EMSCRIPTEN)
#include "minko/file/FileProtocolWorker.hpp"
#endif

#if defined(EMSCRIPTEN)
# include "minko/MinkoWebGL.hpp"
# include "SDL/SDL.h"
# include "emscripten/emscripten.h"
#elif defined(MINKO_ANGLE)
# include "SDL.h"
# include "SDL_syswm.h"
# include <EGL/egl.h>
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#elif defined(__ANDROID__)
# include "minko/MinkoAndroid.hpp"
# include "SDL.h"
#else
# include "SDL.h"
#endif

#if defined(__APPLE__)
# include <TargetConditionals.h>
# if TARGET_OS_IPHONE
#  include "SDL_opengles.h"
# endif
#endif

#if defined(_WIN32)
#include "Windows.h"
#endif

using namespace minko;
using namespace minko::math;
using namespace minko::async;

const float Canvas::SDLFinger::SWIPE_PRECISION = 0.05f;

Canvas::Canvas(const std::string& name, const uint width, const uint height, bool useStencil, bool chromeless) :
    _name(name),
    _useStencil(useStencil),
    _chromeless(chromeless),
    _data(data::Provider::create()),
    _active(false),
    _previousTime(std::chrono::high_resolution_clock::now()),
    _startTime(std::chrono::high_resolution_clock::now()),
    _framerate(0.f),
    _desiredFramerate(60.f),
    _enterFrame(Signal<Canvas::Ptr, float, float>::create()),
    _resized(Signal<AbstractCanvas::Ptr, uint, uint>::create()),
    _joystickAdded(Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::create()),
    _joystickRemoved(Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::create())
{
    _data->set("canvas.viewport", math::vec4(0.0f, 0.0f, (float) width, (float) height));
}

#if defined(_WIN32)
BOOL
WINAPI
ConsoleHandlerRoutine(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_CLOSE_EVENT)
        return true;
    return false;
}
#endif

void
Canvas::initialize()
{
#if defined(__ANDROID__)
    file::Options::defaultProtocolFunction("file", [](const std::string& filename)
    {
        return minko::file::APKProtocol::create();
    });

    // std::cout.rdbuf(new minko::log::AndroidStreambuf());
    // std::cerr.rdbuf(new minko::log::AndroidStreambuf());
#endif

    initializeContext(_name, width(), height(), _useStencil);
    initializeInputs();

#if !defined(EMSCRIPTEN)
    registerWorker<file::FileProtocolWorker>("file-protocol");
#endif

#if defined(_WIN32)
    SetConsoleCtrlHandler(ConsoleHandlerRoutine, true);
#endif
}

void
Canvas::initializeInputs()
{
    _mouse = SDLMouse::create(shared_from_this());
    _keyboard = SDLKeyboard::create();
    _finger = SDLFinger::create(shared_from_this());

#if !defined(EMSCRIPTEN) && !defined(__ANDROID__)
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        SDL_Joystick* joystick = SDL_JoystickOpen(i);

        if (!joystick)
            continue;
        else
            _joysticks[i] = SDLJoystick::create(shared_from_this(), SDL_JoystickInstanceID(joystick), joystick);
    }
#endif
}

void
Canvas::initializeContext(const std::string& windowTitle, unsigned int width, unsigned int height, bool useStencil)
{
#ifndef EMSCRIPTEN
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
        throw std::runtime_error(SDL_GetError());

    if (useStencil)
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    auto sdlFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;

    if (_chromeless)
        sdlFlags |= SDL_WINDOW_BORDERLESS;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    _window = SDL_CreateWindow(
        windowTitle.c_str(),
        SDL_WINDOWPOS_CENTERED, // SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_CENTERED, // SDL_WINDOWPOS_UNDEFINED,
        width, height,
        sdlFlags
    );

    if (!_window)
        throw std::runtime_error(SDL_GetError());

# if MINKO_ANGLE
    if (!(_angleContext = initContext(_window, width, height)))
        throw std::runtime_error("Could not create Angle context");
# elif TARGET_IPHONE_SIMULATOR
//    SDL_CreateRenderer(_window, -1, 0);
    SDL_GLContext glContext = SDL_GL_CreateContext(_window);
    if (!glContext)
        throw std::runtime_error("Could not create iOS context");
# else
    SDL_GLContext glContext = SDL_GL_CreateContext(_window);
    if (!glContext)
        throw std::runtime_error("Could not create a window context from SDL");
# endif // MINKO_ANGLE

    _context = minko::render::OpenGLES2Context::create();
#else // if defined(EMSCRIPTEN)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
        throw std::runtime_error(SDL_GetError());

    if (useStencil)
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_WM_SetCaption(windowTitle.c_str(), NULL);

    SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_WINDOW_RESIZABLE);

    _context = minko::render::WebGLContext::create();
#endif // EMSCRIPTEN

    this->width(width);
    this->height(height);
}

#ifdef MINKO_ANGLE
ESContext*
Canvas::initContext(SDL_Window* window, unsigned int width, unsigned int height)
{
    this->width(0);
    this->height(0);

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

    this->width(width);
    this->height(height);

    return es_context;
}
#endif

uint
Canvas::x()
{
    return (uint) _data->get<math::vec4>("canvas.viewport").x;
}

uint
Canvas::y()
{
    return (uint) _data->get<math::vec4>("canvas.viewport").y;
}

uint
Canvas::width()
{
    return (uint) _data->get<math::vec4>("canvas.viewport").z;
}

uint
Canvas::height()
{
    return (uint) _data->get<math::vec4>("canvas.viewport").w;
}

void
Canvas::x(uint value)
{
    auto viewport = _data->get<math::vec4>("canvas.viewport");

     _data->set("canvas.viewport", math::vec4((float) value, viewport.y, viewport.z, viewport.w));
}

void
Canvas::y(uint value)
{
    auto viewport = _data->get<math::vec4>("canvas.viewport");

     _data->set("canvas.viewport", math::vec4(viewport.x, (float) value, viewport.z, viewport.w));
}

void
Canvas::width(uint value)
{
    auto viewport = _data->get<math::vec4>("canvas.viewport");

    _data->set("canvas.viewport", math::vec4(viewport.x, viewport.y, (float) value, viewport.w));
}

void
Canvas::height(uint value)
{
    auto viewport = _data->get<math::vec4>("canvas.viewport");

    _data->set("canvas.viewport", math::vec4(viewport.x, viewport.y, viewport.z, (float) value));
}

void
Canvas::step()
{
#if defined(EMSCRIPTEN)
    // Detect new joystick
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        if (!SDL_JoystickOpened(i))
        {
            auto joystick = SDL_JoystickOpen(i);
            if (joystick)
            {
                if (_joysticks.find(i) == _joysticks.end())
                {
                    auto sdlJoystick = SDLJoystick::create(shared_from_this(), i, joystick);
                    _joysticks[i] = sdlJoystick;
                }

                _joystickAdded->execute(shared_from_this(), _joysticks[i]);

# if defined(DEBUG)
                printf("New joystick found!\n");
                printf("Joystick %i\n", i);
                printf("Name: %s\n", SDL_JoystickName(i));
                printf("Number of Axes: %d\n", SDL_JoystickNumAxes(joystick));
                printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(joystick));
                printf("Number of Balls: %d\n", SDL_JoystickNumBalls(joystick));
# endif // DEBUG
            }
        }
    }

    // A gamepad has been removed ?
    if (_joysticks.size() != SDL_NumJoysticks())
    {
        // We looking for the missing gamepad
        int joystickId = 0;
        for (auto it = _joysticks.begin(); it != _joysticks.end(); ++it)
        {
            if (!SDL_JoystickOpen(it->first))
            {
                joystickId = it->first;
                break;
            }
        }

        auto joystick = _joysticks[joystickId]->_joystick;

        _joystickRemoved->execute(shared_from_this(), _joysticks[joystickId]);

        SDL_JoystickClose(joystick);
        _joysticks.erase(joystickId);
    }
#endif
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        {
            quit();
            break;
        }

        case SDL_KEYDOWN:
        {
            _keyboard->keyDown()->execute(_keyboard);

			auto keyCode = static_cast<input::Keyboard::KeyCode>(event.key.keysym.sym);

            for (uint i = 0; i < input::Keyboard::NUM_KEYS; ++i)
            {
                auto code = static_cast<input::Keyboard::Key>(i);

				if (!_keyboard->hasKeyDownSignal(code))
					continue;

				auto pair = _keyboard->keyToKeyCodeMap.find(code);

				if (pair != _keyboard->keyToKeyCodeMap.end() && pair->second == keyCode)
                    _keyboard->keyDown(code)->execute(_keyboard, i);
            }
            break;
        }

        case SDL_KEYUP:
        {
            _keyboard->keyUp()->execute(_keyboard);

			auto keyCode = static_cast<input::Keyboard::KeyCode>(event.key.keysym.sym);

			for (uint i = 0; i < input::Keyboard::NUM_KEYS; ++i)
			{
				auto code = static_cast<input::Keyboard::Key>(i);

				if (!_keyboard->hasKeyUpSignal(code))
					continue;

				auto pair = _keyboard->keyToKeyCodeMap.find(code);

				if (pair != _keyboard->keyToKeyCodeMap.end() && pair->second == keyCode)
					_keyboard->keyUp(code)->execute(_keyboard, i);
			}

            for (uint i = 0; i < input::Keyboard::NUM_KEYS; ++i)
            {
                auto code = static_cast<input::Keyboard::Key>(i);
                if (_keyboard->_keyboardState[i] && _keyboard->hasKeyUpSignal(code))
                    _keyboard->keyUp(code)->execute(_keyboard, i);
            }
        }

        case SDL_MOUSEMOTION:
        {
            auto oldX = _mouse->input::Mouse::x();
            auto oldY = _mouse->input::Mouse::y();

            _mouse->x(event.motion.x);
            _mouse->y(event.motion.y);
            _mouse->move()->execute(_mouse, event.motion.x - oldX, event.motion.y - oldY);
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                _mouse->leftButtonDown()->execute(_mouse);
                break;
            case SDL_BUTTON_RIGHT:
                _mouse->rightButtonDown()->execute(_mouse);
                break;
#ifdef EMSCRIPTEN
            case SDL_BUTTON_X1:
                _mouse->wheel()->execute(_mouse, 0, 1);
                break;
            case SDL_BUTTON_X2:
                _mouse->wheel()->execute(_mouse, 0, -1);
                break;
#endif
            }
            break;
        }

        case SDL_MOUSEBUTTONUP:
        {
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                _mouse->leftButtonUp()->execute(_mouse);
                break;
            case SDL_BUTTON_RIGHT:
                _mouse->rightButtonUp()->execute(_mouse);
                break;
            }
            break;
        }

        case SDL_MOUSEWHEEL:
        {
            _mouse->wheel()->execute(_mouse, event.wheel.x, event.wheel.y);
            //_mouseWheel->execute(shared_from_this(), event.wheel.x, event.wheel.y);
            break;
        }

#ifndef EMSCRIPTEN
            // Touch events
        case SDL_FINGERDOWN:
        {
            _finger->x(uint(event.tfinger.x));
            _finger->y(uint(event.tfinger.y));

            _finger->fingerDown()->execute(_finger, event.tfinger.x, event.tfinger.y);

            break;
        }

        case SDL_FINGERUP:
        {
            _finger->x(uint(event.tfinger.x));
            _finger->y(uint(event.tfinger.y));

            _finger->fingerUp()->execute(_finger, event.tfinger.x, event.tfinger.y);

            break;
        }

        case SDL_FINGERMOTION:
        {
            _finger->x(uint(event.tfinger.x));
            _finger->y(uint(event.tfinger.y));
            _finger->dx(uint(event.tfinger.dx));
            _finger->dy(uint(event.tfinger.dy));

            _finger->fingerMotion()->execute(_finger, event.tfinger.dx, event.tfinger.dy);

            // Gestures
            if (event.tfinger.dx > SDLFinger::SWIPE_PRECISION)
                _finger->swipeRight()->execute(_finger);

            if (-event.tfinger.dx > SDLFinger::SWIPE_PRECISION)
                _finger->swipeLeft()->execute(_finger);

            if (event.tfinger.dy > SDLFinger::SWIPE_PRECISION)
                _finger->swipeDown()->execute(_finger);

            if (-event.tfinger.dy > SDLFinger::SWIPE_PRECISION)
                _finger->swipeUp()->execute(_finger);

            break;
        }
#endif
        case SDL_JOYAXISMOTION:
        {
            _joysticks[event.jaxis.which]->joystickAxisMotion()->execute(
                _joysticks[event.jaxis.which], event.jaxis.which, event.jaxis.axis, event.jaxis.value
            );
            break;
        }

        case SDL_JOYHATMOTION:
        {
            _joysticks[event.jhat.which]->joystickHatMotion()->execute(
                _joysticks[event.jhat.which], event.jhat.which, event.jhat.hat, event.jhat.value
            );
            break;
        }

        case SDL_JOYBUTTONDOWN:
        {
            int button = event.jbutton.button;

#if defined(EMSCRIPTEN)
            auto htmlButton = static_cast<SDLJoystick::Button>(button);
            auto nativeButton = SDLJoystick::GetNativeButton(htmlButton);

            if (nativeButton != SDLJoystick::Button::Nothing)
                button = static_cast<int>(nativeButton);
#endif // EMSCRIPTEN

            _joysticks[event.jbutton.which]->joystickButtonDown()->execute(
                _joysticks[event.jbutton.which], event.jbutton.which, button
            );
            break;
        }

        case SDL_JOYBUTTONUP:
        {
            auto button = event.jbutton.button;

# if defined(EMSCRIPTEN)
            auto htmlButton = static_cast<SDLJoystick::Button>(button);
            auto nativeButton = SDLJoystick::GetNativeButton(htmlButton);

            if (nativeButton != SDLJoystick::Button::Nothing)
                button = static_cast<int>(nativeButton);
#endif // EMSCRIPTEN

            _joysticks[event.jbutton.which]->joystickButtonUp()->execute(
                _joysticks[event.jbutton.which], event.jbutton.which, button
            );
            break;
        }

#ifndef EMSCRIPTEN
        case SDL_JOYDEVICEADDED:
        {
            int             device = event.cdevice.which;
            auto            joystick = SDL_JoystickOpen(device);
            SDL_JoystickID  instance_id = SDL_JoystickInstanceID(joystick);

            if (_joysticks.find(instance_id) == _joysticks.end())
            {
                auto sdlJoystick = SDLJoystick::create(shared_from_this(), instance_id, joystick);
                _joysticks[instance_id] = sdlJoystick;
            }

            _joystickAdded->execute(shared_from_this(), _joysticks[instance_id]);
            break;
        }

        case SDL_JOYDEVICEREMOVED:
        {
            auto joystick = _joysticks[event.cdevice.which]->_joystick;

            _joystickRemoved->execute(shared_from_this(), _joysticks[event.cdevice.which]);

            SDL_JoystickClose(joystick);
            _joysticks.erase(event.cdevice.which);

            break;
        }
#endif // EMSCRIPTEN

#ifdef EMSCRIPTEN
        case SDL_VIDEORESIZE:
        {
            width(event.resize.w);
            height(event.resize.h);

            _screen = SDL_SetVideoMode(width(), height(), 0, SDL_OPENGL | SDL_WINDOW_RESIZABLE);
            _context->configureViewport(x(), y(), width(), height());
            _resized->execute(shared_from_this(), width(), height());
            break;
        }
#else
        case SDL_WINDOWEVENT:
        {
            switch (event.window.event)
            {
                case SDL_WINDOWEVENT_RESIZED:
                    width(event.window.data1);
                    height(event.window.data2);

                    _context->configureViewport(x(), y(), width(), height());
                    _resized->execute(shared_from_this(), width(), height());
                    break;

                default:
                    break;
            }
            break;
        }
#endif // EMSCRIPTEN

        default:
            break;
        }
    }

#if !defined(EMSCRIPTEN)
    for (auto worker : _activeWorkers)
        worker->poll();
#endif
    auto absoluteTime = std::chrono::high_resolution_clock::now();
    _relativeTime   = 1e-6f * std::chrono::duration_cast<std::chrono::nanoseconds>(absoluteTime - _startTime).count(); // in milliseconds
    _frameDuration  = 1e-6f * std::chrono::duration_cast<std::chrono::nanoseconds>(absoluteTime - _previousTime).count(); // in milliseconds

    _enterFrame->execute(shared_from_this(), _relativeTime, _frameDuration);
    _previousTime = absoluteTime;

    // swap buffers
#if defined(MINKO_ANGLE)
    eglSwapBuffers(_angleContext->eglDisplay, _angleContext->eglSurface);
#elif defined(EMSCRIPTEN)
    SDL_GL_SwapBuffers();
#else
    SDL_GL_SwapWindow(_window);
#endif

    // framerate in seconds
    _framerate = 1000.f / _frameDuration;

#if !defined(EMSCRIPTEN)
    if (_framerate > _desiredFramerate)
    {
        SDL_Delay((uint) ((1000.f / _desiredFramerate) - _frameDuration));

        _framerate = _desiredFramerate;
    }
#endif
}

#if defined(EMSCRIPTEN)
namespace
{
    Canvas::Ptr currentCanvas;

    void
    emscriptenMainLoop()
    {
        currentCanvas->step();
    }
}
#endif

void
Canvas::run()
{
    _active = true;
    _framerate = 0.f;

#if defined(EMSCRIPTEN)
    currentCanvas = shared_from_this();
    emscripten_set_main_loop(emscriptenMainLoop, 0, 1);
#else
    while (_active)
    {
        step();
        // usleep(1000000 / TARGET_FPS);
    }
#endif
}

void
Canvas::quit()
{
    _active = false;
}

Canvas::WorkerPtr
Canvas::getWorker(const std::string& name)
{
    if (!_workers.count(name))
        return nullptr;

    auto worker = _workers[name]();

    _activeWorkers.push_back(worker);

    return worker;
}

int
Canvas::getJoystickAxis(input::Joystick::Ptr joy, int axis)
{
    int id = joy->joystickId();

    if (_joysticks.find(id) == _joysticks.end())
        return -1;

    return SDL_JoystickGetAxis(_joysticks[id]->joystick(), axis);
}
