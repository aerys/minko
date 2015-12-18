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
#include "minko/log/Logger.hpp"
#include "minko/SDLBackend.hpp"
#include "minko/scene/Node.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/PerspectiveCamera.hpp"

#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5 && MINKO_PLATFORM != MINKO_PLATFORM_ANDROID
# include "minko/file/FileProtocolWorker.hpp"
#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
# include "minko/file/APKProtocolWorker.hpp"
#endif

#include "SDL.h"
#include "SDL_syswm.h"

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
# include "minko/SDLWebGLBackend.hpp"
#elif MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
# include "minko/file/APKProtocol.hpp"
#elif MINKO_PLATFORM == MINKO_PLATFORM_IOS
// # include "SDL_opengles.h"
// # include "SDL_syswm.h"
#elif defined(MINKO_PLUGIN_ANGLE)
# include "minko/MinkoAngle.hpp"
#elif defined(MINKO_PLUGIN_OFFSCREEN)
# include "minko/MinkoOffscreen.hpp"
#endif

#include "minko/audio/SDLAudio.hpp"

#if MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
# include "Windows.h"
#endif

using namespace minko;
using namespace minko::component;
using namespace minko::scene;
using namespace minko::async;

Canvas::Canvas(const std::string& name, const uint width, const uint height, int flags) :
    _name(name),
    _flags(flags),
    _data(data::Provider::create()),
    _active(false),
    _previousTime(std::chrono::high_resolution_clock::now()),
    _startTime(std::chrono::high_resolution_clock::now()),
    _framerate(0.f),
    _desiredFramerate(60.f),
	_swapBuffersAtEnterFrame(true),
    _enterFrame(Signal<Canvas::Ptr, float, float>::create()),
    _resized(Signal<AbstractCanvas::Ptr, uint, uint>::create()),
    _fileDropped(Signal<const std::string&>::create()),
    _joystickAdded(Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::create()),
    _joystickRemoved(Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::create()),
    _suspended(Signal<AbstractCanvas::Ptr>::create()),
    _resumed(Signal<AbstractCanvas::Ptr>::create()),
    _width(width),
    _height(height),
    _x(0),
    _y(0),
    _onWindow(false),
    _enableRendering(true)
{
    _data->set("viewport", math::vec4(0.f, 0.f, (float)width, (float)height));
}

void
Canvas::initialize()
{
#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    file::Options::defaultProtocolFunction("file", [](const std::string& filename)
    {
        return file::APKProtocol::create();
    });
#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS
    // Exclude Library and Document folders from iCloud backup system
    NSString* appLibraryFolder = [NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString* appDocumentFolder = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSArray* backupFolders = [NSArray arrayWithObjects: appLibraryFolder, appDocumentFolder, nil];

    NSURL * url;
    for (NSString* folder in backupFolders)
    {
        url = [NSURL fileURLWithPath:folder];

        assert([[NSFileManager defaultManager] fileExistsAtPath: [url path]]);

        NSLog(@"Final URL: %@", url);

        NSError *error = nil;
        BOOL success = [url setResourceValue: [NSNumber numberWithBool: YES]
                            forKey: NSURLIsExcludedFromBackupKey error: &error];
        if(!success)
            NSLog(@"Error excluding %@ from backup %@", [url lastPathComponent], error);
    }
#endif

    initializeWindow();
    initializeContext();
    initializeInputs();

#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5 && MINKO_PLATFORM != MINKO_PLATFORM_ANDROID
    registerWorker<file::FileProtocolWorker>("file-protocol");
#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    registerWorker<file::APKProtocolWorker>("apk-protocol");
#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
	SetConsoleCtrlHandler([](DWORD type) -> BOOL { return type == CTRL_CLOSE_EVENT; }, true);
#endif
}

void
Canvas::initializeInputs()
{
    _mouse = input::SDLMouse::create(shared_from_this());
    _keyboard = input::SDLKeyboard::create();
    _touch = input::SDLTouch::create(shared_from_this());

#if (MINKO_PLATFORM & (MINKO_PLATFORM_LINUX | MINKO_PLATFORM_OSX | MINKO_PLATFORM_WINDOWS))
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        SDL_Joystick* joystick = SDL_JoystickOpen(i);

        if (!joystick)
            continue;
        else
            _joysticks[i] = input::SDLJoystick::create(shared_from_this(), SDL_JoystickInstanceID(joystick), joystick);
    }
#endif
}

void
Canvas::initializeWindow()
{
    int initFlags = 0;

#if !defined(MINKO_PLUGIN_OFFSCREEN)
    initFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK;
#endif

    if (SDL_Init(initFlags) < 0)
        throw std::runtime_error(SDL_GetError());

    if (_flags & STENCIL)
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    // SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);
    SDL_GL_SetSwapInterval(0);

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    SDL_WM_SetCaption(_name.c_str(), nullptr);

    SDL_SetVideoMode(_width, _height, 0, SDL_OPENGL | SDL_WINDOW_RESIZABLE);

    _window = nullptr;
#else
    int windowFlags = SDL_WINDOW_OPENGL;

    if (_flags & RESIZABLE)
        windowFlags |= SDL_WINDOW_RESIZABLE;

    if (_flags & CHROMELESS)
        windowFlags |= SDL_WINDOW_BORDERLESS;

    if (_flags & FULLSCREEN)
        windowFlags |= SDL_WINDOW_FULLSCREEN;

    if (_flags & HIDDEN)
        windowFlags |= SDL_WINDOW_HIDDEN;

# if !defined(MINKO_PLUGIN_OFFSCREEN)
    _window = SDL_CreateWindow(
        _name.c_str(),
        SDL_WINDOWPOS_CENTERED, // SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_CENTERED, // SDL_WINDOWPOS_UNDEFINED,
        _width, _height,
        windowFlags
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
    else
    {
        const auto error = SDL_GetError();

        LOG_ERROR(error);
    }
# endif

    _audio = audio::SDLAudio::create(shared_from_this());
#endif
}

void*
Canvas::systemWindow() const
{
    SDL_Window* sdlWindow = _window;
    SDL_SysWMinfo info;

    SDL_VERSION(&info.version);

    if (SDL_GetWindowWMInfo(sdlWindow, &info))
    {
#if MINKO_PLATFORM == MINKO_PLATFORM_IOS
        return info.info.uikit.window;
#elif MINKO_PLATFORM == MINKO_PLATFORM_OSX
        return info.info.cocoa.window;
#elif MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS
        return info.info.win.window;
#endif
    }

    return nullptr;
}

void
Canvas::initializeContext()
{
#if (MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS) && defined(MINKO_PLUGIN_ANGLE)
    _backend = SDLAngleBackend::create();
#elif (defined(MINKO_PLUGIN_OFFSCREEN))
    _backend = SDLOffscreenBackend::create();
#elif MINKO_PLATFORM == MINKO_PLATFORM_HTML5
    _backend = SDLWebGLBackend::create();
#else
    _backend = SDLBackend::create();
#endif

    _backend->initialize(shared_from_this());

    _context = minko::render::OpenGLES2Context::create();

    if (!_context)
        throw std::runtime_error("Could not create context");
}

Canvas::NodePtr
Canvas::createScene()
{
    auto sceneManager = SceneManager::create(shared_from_this());
    auto root = Node::create("root")
        ->addComponent(sceneManager);

    _camera = Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
			math::inverse(math::lookAt(math::vec3(0.f, 0.f, 3.f), math::vec3(), math::vec3(0.f, 1.f, 0.f)))
		))
		->addComponent(PerspectiveCamera::create(shared_from_this()->aspectRatio()));

    root->addChild(_camera);

    _resizedSlot = _resized->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
	{
		_camera->component<PerspectiveCamera>()->aspectRatio(float(w) / float(h));
	});

    return root;
}

uint
Canvas::x()
{
    return _x;
}

uint
Canvas::y()
{
    return _y;
}

uint
Canvas::width()
{
    return _width;
}

uint
Canvas::height()
{
    return _height;
}

void
Canvas::x(uint value)
{
    if (value != _x)
    {
        auto viewport = _data->get<math::vec4>("viewport");

        _x = value;
        viewport.x = (float)value;
        _data->set<math::vec4>("viewport", viewport);
    }
}

void
Canvas::y(uint value)
{
    if (value != _y)
    {
        auto viewport = _data->get<math::vec4>("viewport");

        _y = value;
        viewport.y = (float)value;
        _data->set<math::vec4>("viewport", viewport);
    }
}

void
Canvas::width(uint value)
{
    if (value != _width)
    {
        auto viewport = _data->get<math::vec4>("viewport");

        _width = value;
        viewport.z = (float)value;
        _data->set<math::vec4>("viewport", viewport);
    }
}

void
Canvas::height(uint value)
{
    if (value != _height)
    {
        auto viewport = _data->get<math::vec4>("viewport");

        _height = value;
        viewport.w = (float)value;
        _data->set<math::vec4>("viewport", viewport);
    }
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
                    auto sdlJoystick = input::SDLJoystick::create(that, i, joystick);
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

    auto enteredOrLeftThisFrame = false;

    std::vector<char16_t> chars;

    _mouse->dX(0);
    _mouse->dY(0);

    _touch->resetDeltas();

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

            char16_t c = 0;

            while (event.text.text[i] != '\0' && event.text.text[i] != 0)
                i++;

            if (i == 1)
            {
                c = event.text.text[0] & 0x00FF;
            }
            else if (i == 2)
            {
                int c1 = event.text.text[0] & 0xFF;
                int c2 = event.text.text[1] & 0xFF;

                if (c1 == 0xC3)
                    c2 += 0x40;
                else if (c1 == 0xC4)
                    c2 += 0x80;
                else if (c1 == 0xC5)
                    c2 += 0xC0;

                c = c2;
            }
            
            if (c != 0)
            {
                bool found = false;

                for (size_t i = 0; i < chars.size(); ++i)
                {
                    if (chars[i] == c)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    chars.push_back(c);
                    _keyboard->textInput()->execute(_keyboard, c);
                }
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
            break;
        }

        case SDL_MOUSEMOTION:
        {
            int windowW;
            int windowH;

            SDL_GetWindowSize(_window, &windowW, &windowH);

            auto x = event.motion.x;
            auto y = event.motion.y;

            auto dX = event.motion.xrel;
            auto dY = event.motion.yrel;

            if (windowW != _width || windowH != _height)
            {
                x = int(float(_width) * float(x) / float(windowW));
                y = int(float(_height) * float(y) / float(windowH));

                dX = int(float(_width) * float(dX) / float(windowW));
                dY = int(float(_height) * float(dY) / float(windowH));
            }

            _mouse->x(x);
            _mouse->y(y);

            _mouse->dX(_mouse->dX() + dX);
            _mouse->dY(_mouse->dY() + dY);

            _mouse->move()->execute(_mouse, dX, dY);
            break;
        }

        case SDL_MOUSEBUTTONDOWN:
        {
            if (enteredOrLeftThisFrame)
                break;

            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                _mouse->leftButtonDown()->execute(_mouse);
                break;
            case SDL_BUTTON_RIGHT:
                _mouse->rightButtonDown()->execute(_mouse);
                break;
            case SDL_BUTTON_MIDDLE:
                _mouse->middleButtonDown()->execute(_mouse);
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
            case SDL_BUTTON_MIDDLE:
                _mouse->middleButtonUp()->execute(_mouse);
                break;
            }
            break;
        }

        case SDL_MOUSEWHEEL:
        {
            _mouse->wheel()->execute(_mouse, event.wheel.x, event.wheel.y);
            break;
        }

        // Touch events
        case SDL_FINGERDOWN:
        {
            auto x = event.tfinger.x * _width;
            auto y = event.tfinger.y * _height;
            auto id = (int)(event.tfinger.fingerId);

            _touch->addTouch(id, x, y, 0.f, 0.f);

            _mouse->x((int)_touch->averageX());
            _mouse->y((int)_touch->averageY());

            _touch->touchDown()->execute(
                _touch,
                id,
                x,
                y
            );

            if (_touch->numTouches() == 1)
            {
                _touch->lastTouchDownTime(_relativeTime);
                _touch->lastTouchDownX(x);
                _touch->lastTouchDownY(y);
            }
            else
            {
                _touch->lastTouchDownTime(-1.f);
                _touch->lastTapTime(-1.f);
            }

            break;
        }

        case SDL_FINGERUP:
        {
            auto x = event.tfinger.x * _width;
            auto y = event.tfinger.y * _height;
            auto id = (int)(event.tfinger.fingerId);

            _mouse->x((int)_touch->averageX());
            _mouse->y((int)_touch->averageY());

            _touch->removeTouch(id);

            _touch->touchUp()->execute(
                _touch,
                id,
                x,
                y
            );

            if (_touch->numTouches() == 0 && _touch->lastTouchDownTime() != -1.0f )
            {
                auto dX = std::abs(x - _touch->lastTouchDownX());
                auto dY = std::abs(y - _touch->lastTouchDownY());
                auto dT = _relativeTime - _touch->lastTouchDownTime();

                if (dT < input::SDLTouch::TAP_DELAY_THRESHOLD &&
                    dX < input::SDLTouch::TAP_MOVE_THRESHOLD &&
                    dY < input::SDLTouch::TAP_MOVE_THRESHOLD)
                {
                    _touch->tap()->execute(_touch, x, y);

                    dX = std::abs(x - _touch->lastTapX()) * 0.75f;
                    dY = std::abs(y - _touch->lastTapY()) * 0.75f;
                    dT = _relativeTime - _touch->lastTapTime();

                    if (_touch->lastTapTime() != -1.0f &&
                        dT < input::SDLTouch::DOUBLE_TAP_DELAY_THRESHOLD &&
                        dX < input::SDLTouch::TAP_MOVE_THRESHOLD &&
                        dY < input::SDLTouch::TAP_MOVE_THRESHOLD)
                    {
                        _touch->doubleTap()->execute(_touch, x, y);
                        _touch->lastTapTime(-1.0f);
                    }
                    else
                    {
                        _touch->lastTapTime(_relativeTime);
                    }

                    _touch->lastTapX(x);
                    _touch->lastTapY(y);
                }
            }

            _touch->lastTouchDownTime(-1.0f);

            break;
        }

        case SDL_FINGERMOTION:
        {
            auto id = (int)(event.tfinger.fingerId);
            auto x = event.tfinger.x * _width;
            auto y = event.tfinger.y * _height;
            auto dx = event.tfinger.dx * _width;
            auto dy = event.tfinger.dy * _height;

            if (std::abs(_touch->lastTouchDownX() - x) > input::SDLTouch::TAP_MOVE_THRESHOLD || std::abs(_touch->lastTouchDownY() - y) > input::SDLTouch::TAP_MOVE_THRESHOLD)
                _touch->lastTouchDownTime(-1.0f);

            _touch->updateTouch(id, x, y, dx, dy);

            _mouse->x((int)_touch->averageX());
            _mouse->y((int)_touch->averageY());

            _mouse->dX((int)_touch->averageDX());
            _mouse->dY((int)_touch->averageDY());

            _touch->touchMove()->execute(
                _touch,
                id,
                dx,
                dy
            );

            // Gestures
				if (event.tfinger.dx > input::SDLTouch::SWIPE_PRECISION)
            {
                _touch->swipeRight()->execute(_touch);
            }

                if (-event.tfinger.dx > input::SDLTouch::SWIPE_PRECISION)
            {
                _touch->swipeLeft()->execute(_touch);
            }

                if (event.tfinger.dy > input::SDLTouch::SWIPE_PRECISION)
            {
                _touch->swipeDown()->execute(_touch);
            }

                if (-event.tfinger.dy > input::SDLTouch::SWIPE_PRECISION)
            {
                _touch->swipeUp()->execute(_touch);
            }

            if (_touch->numTouches() == 2)
            {
                input::Touch::TouchPoint touch2;
                auto hasTouch2 = false;

                for (auto i = 0u; i < _touch->identifiers().size(); ++i)
                {
                    if (_touch->identifiers()[i] != id)
                    {
                        hasTouch2 = true;
                        touch2 = _touch->touch(_touch->identifiers()[i]);
                }
                }

                if (hasTouch2)
                {
                    auto dX1 = (x - dx) - touch2.x;
                    auto dY1 = (y - dy) - touch2.y;

                    auto dX2 = x - touch2.x;
                    auto dY2 = y - touch2.y;

                    auto dist1 = std::sqrt(std::pow(dX1, 2) + std::pow(dY1, 2));
                    auto dist2 = std::sqrt(std::pow(dX2, 2) + std::pow(dY2, 2));

                    auto deltaDist = dist2 - dist1;

                    if (deltaDist != 0.f)
                    {
                        _touch->pinchZoom()->execute(_touch, deltaDist / (float)(width()));
                    }
                }
            }

            break;
        }
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
            input::SDLJoystick::Button button = input::SDLJoystick::button(event.jbutton.button);

            _joysticks[event.jbutton.which]->joystickButtonDown()->execute(
                _joysticks[event.jbutton.which], event.jbutton.which, input::SDLJoystick::buttonId(button)
            );
            break;
        }

        case SDL_JOYBUTTONUP:
        {
            input::SDLJoystick::Button button = input::SDLJoystick::button(event.jbutton.button);

            _joysticks[event.jbutton.which]->joystickButtonUp()->execute(
                _joysticks[event.jbutton.which], event.jbutton.which, input::SDLJoystick::buttonId(button)
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
                auto sdlJoystick = input::SDLJoystick::create(that, instance_id, joystick);
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
                    if (width() == event.window.data1 && height() == event.window.data2)
                        break;

                    width(event.window.data1);
                    height(event.window.data2);

                    _context->configureViewport(x(), y(), width(), height());
                    _resized->execute(that, width(), height());
                    break;

                case SDL_WINDOWEVENT_ENTER:
                    _onWindow = true;
                    enteredOrLeftThisFrame = true;
                    break;

                case SDL_WINDOWEVENT_LEAVE:
                    _onWindow = false;
                    enteredOrLeftThisFrame = true;
                    break;

                default:
                    _resized->execute(that, width(), height());
                    break;
            }
            break;
        }
#endif // MINKO_PLATFORM_HTML5

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
        case SDL_APP_DIDENTERBACKGROUND:
                suspended()->execute(shared_from_this());
                _enableRendering = false;
            break;

        case SDL_APP_DIDENTERFOREGROUND:
                resumed()->execute(shared_from_this());
                _enableRendering = true;
            break;
#endif // MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID

        default:
            break;
        }
    }

    if (_touch->numTouches() && _touch->lastTouchDownTime() != -1.0f && (_relativeTime - _touch->lastTouchDownTime()) > input::SDLTouch::LONG_HOLD_DELAY_THRESHOLD)
    {
        _touch->longHold()->execute(_touch, _touch->averageX(), _touch->averageY());
        _touch->lastTouchDownTime(-1.0f);
    }

#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
    for (auto worker : _activeWorkers)
        worker->poll();
#endif

    auto absoluteTime = std::chrono::high_resolution_clock::now();
	_relativeTime   = 1e-6f * std::chrono::duration_cast<std::chrono::nanoseconds>(absoluteTime - _startTime).count(); // in milliseconds
    _deltaTime = 1e-6f * std::chrono::duration_cast<std::chrono::nanoseconds>(absoluteTime - _previousTime).count(); // in milliseconds
    _previousTime = absoluteTime;

    if (_enableRendering)
    {
        _enterFrame->execute(that, _relativeTime, _deltaTime);

        if (_swapBuffersAtEnterFrame)
            swapBuffers();
    }

    _frameDuration  = 1e-6f * std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - absoluteTime).count(); // in milliseconds

    // framerate in seconds
    _framerate = 1000.f / _frameDuration;

    auto remainingTime = (1000.f / _desiredFramerate) - _frameDuration;

    if (remainingTime > 0)
    {
        _backend->wait(that, uint(remainingTime));
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

    _audio = nullptr;
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

void
Canvas::swapBuffers()
{
	_backend->swapBuffers(shared_from_this());
}

void
Canvas::desiredFramerate(float desiredFramerate)
{
    _desiredFramerate = desiredFramerate;
}

void
Canvas::resetInputs()
{
    while (_touch->numTouches())
    {
        auto id = _touch->identifiers()[0];
        auto touch = _touch->touches()[id];

        auto x = touch.x;
        auto y = touch.y;

        _touch->updateTouch(id, x, y, 0, 0);
        _touch->touchMove()->execute(_touch, id, 0, 0);

        _touch->removeTouch(id);
        _touch->touchUp()->execute(_touch, id, x, y);
    }

    _mouse->dX(0);
    _mouse->dY(0);

    if (_mouse->leftButtonIsDown())
        _mouse->leftButtonUp()->execute(_mouse);
    if (_mouse->rightButtonIsDown())
        _mouse->rightButtonUp()->execute(_mouse);
    if (_mouse->middleButtonIsDown())
        _mouse->middleButtonUp()->execute(_mouse);
}
