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
#include "minko/math/Vector4.hpp"
#include "minko/async/Worker.hpp"
#include "minko/file/Options.hpp"
#include "minko/SDLBackend.hpp"

#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
# include "minko/file/FileProtocolWorker.hpp"
#endif

#include "SDL.h"

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
# include "minko/MinkoWebGL.hpp"
#elif MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
# include "minko/MinkoAndroid.hpp"
#elif MINKO_PLATFORM == MINKO_PLATFORM_IOS
// # include "SDL_opengles.h"
// # include "SDL_syswm.h"
#elif defined(MINKO_PLUGIN_ANGLE)
# include "minko/MinkoAngle.hpp"
#elif defined(MINKO_PLUGIN_OFFSCREEN)
# include "minko/MinkoOffscreen.hpp"
#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
# include "Windows.h"
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
    _fileDropped(Signal<const std::string&>::create()),
    _joystickAdded(Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::create()),
    _joystickRemoved(Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::create()),
    _touchZoom(Signal<std::shared_ptr<input::Touch>, float>::create()),
    _width(width),
    _height(height),
    _x(0),
    _y(0)
{
    _data->set<math::Vector4::Ptr>("canvas.viewport", Vector4::create(0.0f, 0.0f, (float) width, (float) height));
}

void
Canvas::initialize()
{
#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    file::Options::defaultProtocolFunction("file", [](const std::string& filename)
    {
        return file::APKProtocol::create();
    });

    log::Logger::defaultLogger(
        log::Logger::create(log::Logger::Level::Debug, log::AndroidLogSink::create())
    );
#endif

    initializeWindow();
    initializeContext();
    initializeInputs();

#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
    registerWorker<file::FileProtocolWorker>("file-protocol");
#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
	SetConsoleCtrlHandler([](DWORD type) -> BOOL WINAPI { return type == CTRL_CLOSE_EVENT; }, true);
#endif
}

void
Canvas::initializeInputs()
{
    _mouse = SDLMouse::create(shared_from_this());
    _keyboard = SDLKeyboard::create();
    _touch = SDLTouch::create(shared_from_this());
    _touches = std::vector<std::shared_ptr<SDLTouch>>();

#if (MINKO_PLATFORM & (MINKO_PLATFORM_LINUX | MINKO_PLATFORM_OSX | MINKO_PLATFORM_WINDOWS))
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
Canvas::initializeWindow()
{
#if defined(MINKO_PLUGIN_OFFSCREEN)
    auto flags = 0;
#else
    auto flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#endif

    if (SDL_Init(flags) < 0)
        throw std::runtime_error(SDL_GetError());

    if (_useStencil)
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    SDL_WM_SetCaption(_name.c_str(), nullptr);

    SDL_SetVideoMode(_width, _height, 0, SDL_OPENGL | SDL_WINDOW_RESIZABLE);

    _window = nullptr;
#else
    auto sdlFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    if (_chromeless)
        sdlFlags |= SDL_WINDOW_BORDERLESS;

    _window = SDL_CreateWindow(
        _name.c_str(),
        SDL_WINDOWPOS_CENTERED, // SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_CENTERED, // SDL_WINDOWPOS_UNDEFINED,
        _width, _height,
        sdlFlags
    );

    // Reset window size after window creation because certain platforms (iOS, Android)
    // ignore passed arguments and use fullscreen window size. Not failing if the
    // window couln't be created (Offscreen support).
    if (_window)
    {
        int w, h;
        SDL_GetWindowSize(_window, &w, &h);
        width(w);
        height(h);
    }
#endif
}

void
Canvas::initializeContext()
{
#if (MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS) && defined(MINKO_PLUGIN_ANGLE)
    _backend = SDLAngleBackend::create();
#elif (MINKO_PLATFORM == MINKO_PLATFORM_LINUX) && defined(MINKO_PLUGIN_OFFSCREEN)
    _backend = SDLOffscreenBackend::create();
#elif MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    _backend = SDLWebGLBackend::create();
#else
    _backend = SDLBackend::create();
#endif

    _backend->initialize(shared_from_this());

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    _context = minko::render::WebGLContext::create();
#else
    _context = minko::render::OpenGLES2Context::create();
#endif

    if (!_context)
        throw std::runtime_error("Could not create context");
}

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
    _width = value;

    auto viewport = _data->get<math::Vector4::Ptr>("canvas.viewport");

    viewport->setTo(viewport->x(), viewport->y(), (float) value, viewport->w());
}

void
Canvas::height(uint value)
{
    _height = value;

    auto viewport = _data->get<math::Vector4::Ptr>("canvas.viewport");

    viewport->setTo(viewport->x(), viewport->y(), viewport->z(), (float) value);
}

void
Canvas::step()
{
    auto that = shared_from_this();

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
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
                    auto sdlJoystick = SDLJoystick::create(that, i, joystick);
                    _joysticks[i] = sdlJoystick;
                }

                _joystickAdded->execute(that, _joysticks[i]);

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

        _joystickRemoved->execute(that, _joysticks[joystickId]);

        SDL_JoystickClose(joystick);
        _joysticks.erase(joystickId);
    }
#endif

    SDL_Event event;

#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
#endif // MINKO_PLATFORM != MINKO_PLATFORM_HTML5

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        {
            quit();
            break;
        }
#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
        case SDL_DROPFILE:
        {
            _fileDropped->execute(std::string(event.drop.file));
            break;
        }
#endif // MINKO_PLATFORM != MINKO_PLATFORM_HTML5
        case SDL_TEXTINPUT:
        {
            int i = 0;

            while (event.text.text[i] != '\0' && event.text.text[i] != 0)
            {
                _keyboard->textInput()->execute(_keyboard, event.text.text[i++]);
            }
            break;
        }
        case SDL_TEXTEDITING:
        {
            //std::cout << "text editing" << std::endl;
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

                auto pair = input::KeyMap::keyToKeyCodeMap.find(code);

                if (pair != input::KeyMap::keyToKeyCodeMap.end() && pair->second == keyCode)
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

                auto pair = input::KeyMap::keyToKeyCodeMap.find(code);

                if (pair != input::KeyMap::keyToKeyCodeMap.end() && pair->second == keyCode)
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

            int windowW;
            int windowH;

            SDL_GetWindowSize(_window, &windowW, &windowH);

            auto x = event.motion.x;
            auto y = event.motion.y;

            if (windowW != _width || windowH != _height)
            {
                x = int(float(_width) * float(event.motion.x) / float(windowW));
                y = int(float(_height) * float(event.motion.y) / float(windowH));
            }

            _mouse->x(x);
            _mouse->y(y);

            _mouse->move()->execute(_mouse, x - oldX, y - oldY);
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
#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
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
            //_mouseWheel->execute(that, event.wheel.x, event.wheel.y);
            break;
        }

#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
            // Touch events
        case SDL_FINGERDOWN:
        {
#if defined(DEBUG)
            //std::cout << "Finger down! (x: " << event.tfinger.x << ", y: " << event.tfinger.y << ")" << std::endl;
#endif // DEBUG
            _touch->fingerId(event.tfinger.fingerId);
            _touch->x(uint(event.tfinger.x));
            _touch->y(uint(event.tfinger.y));

            _touch->touchDown()->execute(_touch, event.tfinger.x, event.tfinger.y);

            // Create a new touch
            auto touch = SDLTouch::create(that);

            touch->fingerId(event.tfinger.fingerId);
            touch->x(event.tfinger.x);
            touch->y(event.tfinger.y);

            touch->touchDown()->execute(touch, event.tfinger.x, event.tfinger.y);

            // Add the touch to the touch list
            _touches.push_back(touch);
            break;
        }

        case SDL_FINGERUP:
        {
#if defined(DEBUG)
            //std::cout << "Finger up! (x: " << event.tfinger.x << ", y: " << event.tfinger.y << ")" << std::endl;
#endif // DEBUG
            _touch->x(uint(event.tfinger.x));
            _touch->y(uint(event.tfinger.y));

            _touch->touchUp()->execute(_touch, event.tfinger.x, event.tfinger.y);

            // Get the real touch up
            auto fingerUp = std::find_if(_touches.begin(), _touches.end(), [&] (SDLTouch::Ptr f) { return f->Touch::fingerId() == event.tfinger.fingerId; } );

            // If this touch exists
            if (fingerUp != _touches.end())
            {
                auto touch = *fingerUp;

                touch->x(event.tfinger.x);
                touch->y(event.tfinger.y);

                touch->touchUp()->execute(touch, event.tfinger.x, event.tfinger.y);

                // Remove it from the list
                _touches.erase(fingerUp);
            }
            break;
        }

        case SDL_FINGERMOTION:
        {
#if defined(DEBUG)
            /*
            std::cout << "Finger motion! "
            << "("
            << "x: " << event.tfinger.x << ", y: " << event.tfinger.y
            << "|"
            << "dx: " << event.tfinger.dx << ", dy: " << event.tfinger.dy
            << ")" << std::endl;
            */
#endif // DEBUG
            _touch->x(uint(event.tfinger.x));
            _touch->y(uint(event.tfinger.y));
            _touch->dx(uint(event.tfinger.dx));
            _touch->dy(uint(event.tfinger.dy));

            _touch->touchMotion()->execute(_touch, event.tfinger.dx, event.tfinger.dy);

            // Get the real touch in motion
            auto fingerMotion = std::find_if(_touches.begin(), _touches.end(), [&] (SDLTouch::Ptr f) { return f->Touch::fingerId() == event.tfinger.fingerId; } );

            if (fingerMotion != _touches.end())
            {
                auto touch = *fingerMotion;

                // Store event data
                touch->x(event.tfinger.x);
                touch->y(event.tfinger.y);
                touch->dx(event.tfinger.dx);
                touch->dy(event.tfinger.dy);

                touch->touchMotion()->execute(touch, event.tfinger.dx, event.tfinger.dy);

                // Gestures
                if (event.tfinger.dx > SDLTouch::SWIPE_PRECISION)
                {
# if defined(DEBUG)
                std::cout << "Swipe right! (" << event.tfinger.dx << ")" << std::endl;
#endif // DEBUG
                    _touch->swipeRight()->execute(_touch);
                    touch->swipeRight()->execute(touch);
                }

                if (-event.tfinger.dx > SDLTouch::SWIPE_PRECISION)
                {

# if defined(DEBUG)
                    std::cout << "Swipe left! (" << event.tfinger.dx << ")" << std::endl;
#endif // DEBUG
                    _touch->swipeLeft()->execute(_touch);
                    touch->swipeLeft()->execute(touch);
                }

                if (event.tfinger.dy > SDLTouch::SWIPE_PRECISION)
                {

# if defined(DEBUG)
                    std::cout << "Swipe down! (" << event.tfinger.dy << ")" << std::endl;
#endif // DEBUG
                    _touch->swipeDown()->execute(_touch);
                    touch->swipeDown()->execute(touch);
                }

                if (-event.tfinger.dy > SDLTouch::SWIPE_PRECISION)
                {
#if defined(DEBUG)
                std::cout << "Swipe up! (" << event.tfinger.dy << ")" << std::endl;
#endif // DEBUG
                    _touch->swipeUp()->execute(_touch);
                    touch->swipeUp()->execute(touch);
                }

                // If it's the second touch
                if (_touches.size() > 1 && _touches.at(1)->Touch::fingerId() == event.tfinger.fingerId)
                {
                    // Get the first touch
                    auto firstFinger = _touches[0];

                    // Compute distance between first touch and second touch
                    auto distance = sqrt(pow(event.tfinger.x - firstFinger->Touch::x(), 2) + pow(event.tfinger.y - firstFinger->Touch::y(), 2));
                    auto deltaDistance = sqrt(
                                              pow((event.tfinger.x + event.tfinger.dx) - firstFinger->Touch::x(), 2) +
                                              pow((event.tfinger.y + event.tfinger.dy) - firstFinger->Touch::y(), 2));

                    if (deltaDistance != distance)
                    {
                        //std::cout << "[Canvas] Zoom value: " << distance - deltaDistance << "(dx: " << event.tfinger.dx << ", dy: " << event.tfinger.dy << ")" << std::endl;
                        touchZoom()->execute(touch, distance - deltaDistance);
                    }
                }
            }

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
            SDLJoystick::Button button = SDLJoystick::button(event.jbutton.button);

            _joysticks[event.jbutton.which]->joystickButtonDown()->execute(
                _joysticks[event.jbutton.which], event.jbutton.which, SDLJoystick::buttonId(button)
            );
            break;
        }

        case SDL_JOYBUTTONUP:
        {
            SDLJoystick::Button button = SDLJoystick::button(event.jbutton.button);

            _joysticks[event.jbutton.which]->joystickButtonUp()->execute(
                _joysticks[event.jbutton.which], event.jbutton.which, SDLJoystick::buttonId(button)
            );
            break;
        }

#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
        case SDL_JOYDEVICEADDED:
        {
            int             device = event.cdevice.which;
            auto            joystick = SDL_JoystickOpen(device);
            SDL_JoystickID  instance_id = SDL_JoystickInstanceID(joystick);

            if (_joysticks.find(instance_id) == _joysticks.end())
            {
                auto sdlJoystick = SDLJoystick::create(that, instance_id, joystick);
                _joysticks[instance_id] = sdlJoystick;
            }

            _joystickAdded->execute(that, _joysticks[instance_id]);
            break;
        }

        case SDL_JOYDEVICEREMOVED:
        {
            auto joystick = _joysticks[event.cdevice.which]->_joystick;

            _joystickRemoved->execute(that, _joysticks[event.cdevice.which]);

            SDL_JoystickClose(joystick);
            _joysticks.erase(event.cdevice.which);

            break;
        }
#endif // MINKO_PLATFORM_HTML5

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
        case SDL_VIDEORESIZE:
        {
            width(event.resize.w);
            height(event.resize.h);

            _screen = SDL_SetVideoMode(width(), height(), 0, SDL_OPENGL | SDL_WINDOW_RESIZABLE);
            _context->configureViewport(x(), y(), width(), height());
            _resized->execute(that, width(), height());
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
                    _resized->execute(that, width(), height());
                    break;

                default:
                    _resized->execute(that, width(), height());
                    break;
            }
            break;
        }
#endif // MINKO_PLATFORM_HTML5

        default:
            break;
        }
    }

#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
    for (auto worker : _activeWorkers)
        worker->poll();
#endif

    auto absoluteTime = std::chrono::high_resolution_clock::now();
    _relativeTime   = 1e-6f * std::chrono::duration_cast<std::chrono::nanoseconds>(absoluteTime - _startTime).count(); // in milliseconds
    _frameDuration  = 1e-6f * std::chrono::duration_cast<std::chrono::nanoseconds>(absoluteTime - _previousTime).count(); // in milliseconds

    _enterFrame->execute(that, _relativeTime, _frameDuration);
    _previousTime = absoluteTime;

    _backend->swapBuffers(that);

    // framerate in seconds
    _framerate = 1000.f / _frameDuration;

    if (_framerate > _desiredFramerate)
    {
        _backend->wait(that, (1000.f / _desiredFramerate) - _frameDuration);

        _framerate = _desiredFramerate;
    }
}

void
Canvas::run()
{
    _active = true;
    _framerate = 0.f;

    _backend->run(shared_from_this());
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
