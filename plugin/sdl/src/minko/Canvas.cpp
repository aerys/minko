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
#include "minko/data/Provider.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/async/Worker.hpp"

#if !defined(EMSCRIPTEN)
#include "minko/async/FileLoaderWorker.hpp"
#endif

#if defined(EMSCRIPTEN)
# include "minko/MinkoWebGL.hpp"
# include "SDL/SDL.h"
# include "emscripten/emscripten.h"
#elif defined(MINKO_ANGLE)
# include "SDL2/SDL.h"
# include "SDL2/SDL_syswm.h"
# include <EGL/egl.h>
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#else
# include "SDL2/SDL.h"
#endif

#if defined(__APPLE__)
# include <TargetConditionals.h>
# if TARGET_OS_IPHONE
#  include "SDL2/SDL_opengles.h"
# endif
#endif

using namespace minko;
using namespace minko::math;
using namespace minko::async;

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
    _data->set<math::Vector4::Ptr>("canvas.viewport", Vector4::create(0.0f, 0.0f, (float) width, (float) height));
}

void
Canvas::initialize()
{
    initializeContext(_name, width(), height(), _useStencil);
    initializeInputs();

#if !defined(EMSCRIPTEN)
    registerWorker<async::FileLoaderWorker>("file-loader");
#endif
}

void
Canvas::initializeInputs()
{
    _mouse = Canvas::SDLMouse::create(shared_from_this());
    _keyboard = Canvas::SDLKeyboard::create();

#ifndef EMSCRIPTEN
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        SDL_Joystick* joystick = SDL_JoystickOpen(i);

        if (!joystick)
            continue;
        else
            _joysticks[i] = Canvas::SDLJoystick::create(shared_from_this(), SDL_JoystickInstanceID(joystick), joystick);
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
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width, height,
        sdlFlags
    );
    
    if (!_window)
        throw;

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

    SDL_SetVideoMode(width, height, 0, SDL_OPENGL);

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
    return (uint) _data->get<math::Vector4::Ptr>("canvas.viewport")->x();
}

uint
Canvas::y()
{
    return (uint) _data->get<math::Vector4::Ptr>("canvas.viewport")->y();
}

uint
Canvas::width()
{
    return (uint) _data->get<math::Vector4::Ptr>("canvas.viewport")->z();
}

uint
Canvas::height()
{
    return (uint) _data->get<math::Vector4::Ptr>("canvas.viewport")->w();
}

void
Canvas::x(uint value)
{
    auto viewport = _data->get<math::Vector4::Ptr>("canvas.viewport");

    viewport->setTo((float) value, viewport->y(), viewport->z(), viewport->w());
}

void
Canvas::y(uint value)
{
    auto viewport = _data->get<math::Vector4::Ptr>("canvas.viewport");

    viewport->setTo(viewport->x(), (float) value, viewport->z(), viewport->w());
}

void
Canvas::width(uint value)
{
    auto viewport = _data->get<math::Vector4::Ptr>("canvas.viewport");

    viewport->setTo(viewport->x(), viewport->y(), (float) value, viewport->w());
}

void
Canvas::height(uint value)
{
    auto viewport = _data->get<math::Vector4::Ptr>("canvas.viewport");

    viewport->setTo(viewport->x(), viewport->y(), viewport->z(), (float) value);
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
                    auto sdlJoystick = Canvas::SDLJoystick::create(shared_from_this(), i, joystick);
                    _joysticks[i] = sdlJoystick;
                }

                _joystickAdded->execute(shared_from_this(), _joysticks[i]);

#if defined(DEBUG)
                printf("New joystick found!\n");
                printf("Joystick %i\n", i);
                printf("Name: %s\n", SDL_JoystickName(i));
                printf("Number of Axes: %d\n", SDL_JoystickNumAxes(joystick));
                printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(joystick));
                printf("Number of Balls: %d\n", SDL_JoystickNumBalls(joystick));
#endif // DEBUG
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
            _active = false;
            break;

        case SDL_KEYDOWN:
        {
            _keyboard->keyDown()->execute(_keyboard);
            for (uint i = 0; i < input::Keyboard::NUM_SCANCODES; ++i)
            {
                auto code = static_cast<input::Keyboard::ScanCode>(i);
                if (_keyboard->_keyboardState[i] && _keyboard->hasKeyDownSignal(code))
                    _keyboard->keyDown(code)->execute(_keyboard, i);
            }
            break;
        }

        case SDL_KEYUP:
        {
            _keyboard->keyUp()->execute(_keyboard);
            for (uint i = 0; i < input::Keyboard::NUM_SCANCODES; ++i)
            {
                auto code = static_cast<input::Keyboard::ScanCode>(i);
                if (_keyboard->_keyboardState[i] && _keyboard->hasKeyUpSignal(code))
                    _keyboard->keyUp(code)->execute(_keyboard, i);
            }
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
        {
            switch( event.button.button ) 
            {
            case SDL_BUTTON_LEFT:
                _mouse->leftButtonDown()->execute(_mouse);
                break;
            case SDL_BUTTON_RIGHT:
                _mouse->rightButtonDown()->execute(_mouse);
                break;
	    }
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            switch( event.button.button ) 
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
            _mouse->wheel()->execute(_mouse, event.wheel.x, event.wheel.y);
            //_mouseWheel->execute(shared_from_this(), event.wheel.x, event.wheel.y);
            break;

        case SDL_JOYAXISMOTION:
# if defined(DEBUG)
            printf("Joystick %d axis %d value: %d\n",
                event.jaxis.which,
                event.jaxis.axis,
                event.jaxis.value);
#endif // DEBUG
            _joysticks[event.jaxis.which]->joystickAxisMotion()->execute(
                _joysticks[event.jaxis.which], event.jaxis.which, event.jaxis.axis, event.jaxis.value
                );
            break;

        case SDL_JOYHATMOTION:
# if defined(DEBUG)
            printf("Joystick %d hat %d value:",
                event.jhat.which,
                event.jhat.hat);
            if (event.jhat.value == SDL_HAT_CENTERED)
                printf(" centered");
            if (event.jhat.value & SDL_HAT_UP)
                printf(" up");
            if (event.jhat.value & SDL_HAT_RIGHT)
                printf(" right");
            if (event.jhat.value & SDL_HAT_DOWN)
                printf(" down");
            if (event.jhat.value & SDL_HAT_LEFT)
                printf(" left");
            printf("\n");
#endif // DEBUG
            _joysticks[event.jhat.which]->joystickHatMotion()->execute(
                _joysticks[event.jhat.which], event.jhat.which, event.jhat.hat, event.jhat.value
                );
            break;

        case SDL_JOYBUTTONDOWN:
# if defined(DEBUG)
            printf("Joystick %d button %d down\n",
                event.jbutton.which,
                event.jbutton.button);
#endif
            _joysticks[event.jbutton.which]->joystickButtonDown()->execute(
                _joysticks[event.jbutton.which], event.jbutton.which, event.jbutton.button
                );
            break;

        case SDL_JOYBUTTONUP:
# if defined(DEBUG)
            printf("Joystick %d button %d up\n",
                event.jbutton.which,
                event.jbutton.button);
#endif
            _joysticks[event.jbutton.which]->joystickButtonUp()->execute(
                _joysticks[event.jbutton.which], event.jbutton.which, event.jbutton.button
                );
            break;

#ifndef EMSCRIPTEN
        case SDL_JOYDEVICEADDED:
        {
            int				device = event.cdevice.which;
            auto			joystick = SDL_JoystickOpen(device);
            SDL_JoystickID  instance_id = SDL_JoystickInstanceID(joystick);

            if (_joysticks.find(instance_id) == _joysticks.end())
            {
                auto sdlJoystick = Canvas::SDLJoystick::create(shared_from_this(), instance_id, joystick);
                _joysticks[instance_id] = sdlJoystick;
            }

            _joystickAdded->execute(shared_from_this(), _joysticks[instance_id]);

# if defined(DEBUG)
            std::cout << "Is Gamecontroller : " << SDL_IsGameController(device) << std::endl;
            std::cout << "Num joystick : " << SDL_NumJoysticks() << std::endl;
            std::cout << "Name : " << SDL_JoystickName(joystick) << std::endl;
            std::cout << "Num axes : " << SDL_JoystickNumAxes(joystick) << std::endl;
            std::cout << "Num buttons : " << SDL_JoystickNumButtons(joystick) << std::endl;
            std::cout << "Num balls : " << SDL_JoystickNumBalls(joystick) << std::endl;
            std::cout << "Num hat : " << SDL_JoystickNumHats(joystick) << std::endl;
            std::cout << "instance_id : " << instance_id << std::endl;
# endif // DEBUG
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
            width(event.resize.w);
            height(event.resize.h);

            delete _screen;
            _screen = SDL_SetVideoMode(width(), height(), 0, SDL_OPENGL | SDL_RESIZABLE);
            _context->configureViewport(x(), y(), width(), height());
            _resized->execute(shared_from_this(), width(), height());
            break;
#else
        case SDL_WINDOWEVENT:
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
#endif // EMSCRIPTEN
        default:
            break;
        }
    }

#if !defined(EMSCRIPTEN)
    for (auto worker : _activeWorkers)
        worker->update();
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

Canvas::SDLKeyboard::SDLKeyboard()
{
    _keyboardState = SDL_GetKeyboardState(NULL);
}

input::Keyboard::KeyCode
Canvas::SDLKeyboard::getKeyCodeFromScanCode(input::Keyboard::ScanCode scanCode)
{
    return static_cast<input::Keyboard::KeyCode>(SDL_SCANCODE_TO_KEYCODE(static_cast<int>(scanCode)));
}

input::Keyboard::ScanCode
Canvas::SDLKeyboard::getScanCodeFromKeyCode(input::Keyboard::KeyCode keyCode)
{
    return static_cast<input::Keyboard::ScanCode>(SDL_GetScancodeFromKey(static_cast<int>(keyCode)));
}

Canvas::WorkerPtr
Canvas::getWorker(const std::string& name)
{
    if (!_workers.count(name))
        return nullptr;

    auto worker = _workers[name]();

    _activeWorkers.push_back(worker);

    _workerCompleteSlots.push_back(worker->complete()->connect([worker, this](Worker::MessagePtr) {
        std::cout << "Canvas::getWorker(): " << "remove worker" << std::endl;
        //_activeWorkers.remove(worker);
    }));

    return worker;
}
