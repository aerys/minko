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
#include "minko/log/Logger.hpp"
#include "minko/SDLBackend.hpp"
#include "minko/scene/Node.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/math/Matrix4x4.hpp"

#if MINKO_PLATFORM != MINKO_PLATFORM_HTML5
# include "minko/file/FileProtocolWorker.hpp"
#endif

#include "SDL.h"
#include "SDL_syswm.h"

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
using namespace minko::component;
using namespace minko::scene;
using namespace minko::math;
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
    _onWindow(false)
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
    int initFlags = 0;

#if !defined(MINKO_PLUGIN_OFFSCREEN)
    initFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK;
#endif

    if (SDL_Init(initFlags) < 0)
        throw std::runtime_error(SDL_GetError());

    if (_flags & STENCIL)
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
    int windowFlags = SDL_WINDOW_OPENGL;

    if (_flags & RESIZABLE)
        windowFlags |= SDL_WINDOW_RESIZABLE;

    if (_flags & CHROMELESS)
        windowFlags |= SDL_WINDOW_BORDERLESS;

    if (_flags & FULLSCREEN)
        windowFlags |= SDL_WINDOW_FULLSCREEN;

    if (_flags & HIDDEN)
        windowFlags |= SDL_WINDOW_HIDDEN;

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

# if MINKO_PLATFORM & (MINKO_PLATFORM_HTML5 | MINKO_PLATFORM_WINDOWS | MINKO_PLATFORM_ANDROID)
    _audio = SDLAudio::create(shared_from_this());
# endif
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

Canvas::NodePtr  
Canvas::createScene()
{
    auto sceneManager = SceneManager::create(shared_from_this());
    auto root = Node::create("root")
        ->addComponent(sceneManager);

    _camera = Node::create("camera")
		->addComponent(Renderer::create(0x7f7f7fff))
		->addComponent(Transform::create(
		    Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 0.f, 3.f))
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

    auto enteredOrLeftThisFrame = false;

    auto gotTextInput = false;

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
            if (gotTextInput)
                break;

            gotTextInput = true;
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
            break;
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
            break;
        }

        // Touch events
        case SDL_FINGERDOWN:
        {
            auto x = event.tfinger.x * _width;
            auto y = event.tfinger.y * _height;
            auto id = (int)(event.tfinger.fingerId);

            _touch->addTouch(id, x, y);
            
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

                if (dT < SDLTouch::TAP_DELAY_THRESHOLD &&
                    dX < SDLTouch::TAP_MOVE_THRESHOLD && 
                    dY < SDLTouch::TAP_MOVE_THRESHOLD)
                {
                    _touch->tap()->execute(_touch, x, y);
                    
                    dX = std::abs(x - _touch->lastTapX()) * 0.75f;
                    dY = std::abs(y - _touch->lastTapY()) * 0.75f;
                    dT = _relativeTime - _touch->lastTapTime();
                    
                    if (_touch->lastTapTime() != -1.0f &&
                        dT < SDLTouch::DOUBLE_TAP_DELAY_THRESHOLD &&
                        dX < SDLTouch::TAP_MOVE_THRESHOLD &&
                        dY < SDLTouch::TAP_MOVE_THRESHOLD)
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

            if (std::abs(_touch->lastTouchDownX() - x) > SDLTouch::TAP_MOVE_THRESHOLD || std::abs(_touch->lastTouchDownY() - y) > SDLTouch::TAP_MOVE_THRESHOLD)
                _touch->lastTouchDownTime(-1.0f);
            
            _touch->updateTouch(id, x, y);
            
            _mouse->x((int)_touch->averageX());
            _mouse->y((int)_touch->averageY());

            _touch->touchMove()->execute(
                _touch, 
                id,
                dx,
                dy
            );
            
            // Gestures
            if (event.tfinger.dx > SDLTouch::SWIPE_PRECISION)
            {
                _touch->swipeRight()->execute(_touch);
            }

            if (-event.tfinger.dx > SDLTouch::SWIPE_PRECISION)
            {
                _touch->swipeLeft()->execute(_touch);
            }

            if (event.tfinger.dy > SDLTouch::SWIPE_PRECISION)
            {
                _touch->swipeDown()->execute(_touch);
            }

            if (-event.tfinger.dy > SDLTouch::SWIPE_PRECISION)
            {
                _touch->swipeUp()->execute(_touch);
            }

            if (_touch->numTouches() == 2)
            {
                Vector2::Ptr touch2 = nullptr;

                for (auto i = 0; i < _touch->identifiers().size(); ++i)
                {
                    if (_touch->identifiers()[i] != id)
                        touch2 = _touch->touch(_touch->identifiers()[i]);
                }
                
                if (touch2 != nullptr)
                {
                    auto dX1 = (x - dx) - touch2->x();
                    auto dY1 = (y - dy) - touch2->y();

                    auto dX2 = x - touch2->x();
                    auto dY2 = y - touch2->y();
                    
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
            break;

        case SDL_APP_DIDENTERFOREGROUND:
            resumed()->execute(shared_from_this());
            break;
#endif // MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID

        default:
            break;
        }
    }

    if (_touch->numTouches() && _touch->lastTouchDownTime() != -1.0f && (_relativeTime - _touch->lastTouchDownTime()) > SDLTouch::LONG_HOLD_DELAY_THRESHOLD)
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
    
#if MINKO_PLATFORM & (MINKO_PLATFORM_HTML5 | MINKO_PLATFORM_WINDOWS | MINKO_PLATFORM_ANDROID)
    _audio = nullptr;
#endif
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
