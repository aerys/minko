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
#include "minko/Signal.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/OpenGLES2Context.hpp"
#include "minko/AbstractCanvas.hpp"
#include "minko/input/Joystick.hpp"
#include "minko/input/Touch.hpp"
#include "minko/input/SDLMouse.hpp"
#include "minko/input/SDLKeyboard.hpp"
#include "minko/input/SDLTouch.hpp"
#include "minko/input/SDLJoystick.hpp"
#include "minko/async/Worker.hpp"

// Note: cannot be added to the .cpp because this must be compiled within the
// main compilation-unit.
#if MINKO_PLATFORM & (MINKO_PLATFORM_IOS | MINKO_PLATFORM_ANDROID)
# include "SDL_main.h"
#endif

struct SDL_Window;
struct SDL_Surface;

namespace minko
{
    class SDLBackend;

    namespace audio
    {
        class SDLAudio;
    }

    class Canvas :
        public AbstractCanvas,
        public std::enable_shared_from_this<Canvas>
    {
    public:
        typedef std::shared_ptr<Canvas>    Ptr;

        typedef enum
        {
            FULLSCREEN = (1u << 0),
            RESIZABLE = (1u << 1),
            HIDDEN = (1u << 2),
            CHROMELESS = (1u << 3),
            STENCIL = (1u << 4)
        } Flags;

    private:
        typedef std::chrono::high_resolution_clock::time_point                  time_point;
        typedef std::shared_ptr<scene::Node>                                    NodePtr;
        typedef std::shared_ptr<async::Worker>                                  WorkerPtr;

        std::string                                                             _name;
        uint                                                                    _x;
        uint                                                                    _y;
        uint                                                                    _width;
        uint                                                                    _height;
        std::shared_ptr<data::Provider>                                         _data;
        int                                                                     _flags;

        bool                                                                    _active;
        render::AbstractContext::Ptr                                            _context;
        std::shared_ptr<SDLBackend>                                             _backend;
        SDL_Surface*                                                            _screen;
        SDL_Window*                                                             _window;
        float                                                                   _relativeTime;
        float                                                                   _frameDuration;
        float                                                                   _deltaTime;
        time_point                                                              _previousTime;
        time_point                                                              _startTime;
        float                                                                   _framerate;
        float                                                                   _desiredFramerate;
        bool                                                                    _swapBuffersAtEnterFrame;

        std::shared_ptr<audio::SDLAudio>                                        _audio;

        std::shared_ptr<input::SDLMouse>                                        _mouse;
        std::unordered_map<int, std::shared_ptr<input::SDLJoystick>>            _joysticks;
        std::shared_ptr<input::SDLKeyboard>                                     _keyboard;
        std::shared_ptr<input::SDLTouch>                                        _touch;

        // Events
        Signal<Ptr, float, float>::Ptr                                          _enterFrame;
        Signal<AbstractCanvas::Ptr, uint, uint>::Ptr                            _resized;
        Signal<AbstractCanvas::Ptr, uint, uint>::Slot                           _resizedSlot;
        // File dropped
        Signal<const std::string&>::Ptr                                         _fileDropped;
        // Joystick events
        Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::Ptr      _joystickAdded;
        Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::Ptr      _joystickRemoved;

        Signal<AbstractCanvas::Ptr>::Ptr                                        _suspended;
        Signal<AbstractCanvas::Ptr>::Ptr                                        _resumed;

        std::list<std::shared_ptr<async::Worker>>                               _activeWorkers;
        std::list<Any>                                                          _workerCompleteSlots;

        bool                                                                    _onWindow;
        NodePtr                                                                 _camera;
        bool                                                                    _enableRendering;

    public:
        static inline
        Ptr
        create(const std::string&    name,
               const uint            width      = 1280,
               const uint            height     = 720,
               int                   flags      = RESIZABLE)
        {
            auto canvas = std::shared_ptr<Canvas>(new Canvas(name, width, height, flags));

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

        NodePtr
        createScene();

        uint
        x() override;

        uint
        y() override;

        uint
        width() override;

        uint
        height() override;

        float
        aspectRatio() override
        {
            return float(width()) / float(height());
        }

        inline
        SDL_Window*
        window()
        {
            return _window;
        }

        void*
        systemWindow() const;

        int
        getJoystickAxis(input::Joystick::Ptr joystick, int axis) override;

        inline
        std::shared_ptr<data::Provider>
        data() const override
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
        mouse() override
        {
            return _mouse;
        }

        inline
        std::shared_ptr<input::Keyboard>
        keyboard() override
        {
            return _keyboard;
        }

        inline
        std::shared_ptr<input::Touch>
        touch() override
        {
            return _touch;
        }

        inline
        std::shared_ptr<input::Joystick>
        joystick(uint id) override
        {
            return id < numJoysticks() ? _joysticks[id] : nullptr;
        }

        inline
        std::unordered_map<int, std::shared_ptr<input::SDLJoystick>>
        joysticks()
        {
            return _joysticks;
        }

        inline
        uint
        numJoysticks() override
        {
            return _joysticks.size();
        }

        inline
        Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::Ptr
        joystickAdded() override
        {
            return _joystickAdded;
        }

        inline
        Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::Ptr
        joystickRemoved() override
        {
            return _joystickRemoved;
        }

        inline
        Signal<AbstractCanvas::Ptr, uint, uint>::Ptr
        resized() override
        {
            return _resized;
        }

        inline
        Signal<const std::string&>::Ptr
        fileDropped()
        {
            return _fileDropped;
        }

        inline
        Signal<AbstractCanvas::Ptr>::Ptr
        suspended() override
        {
            return _suspended;
        }

        inline
        Signal<AbstractCanvas::Ptr>::Ptr
        resumed() override
        {
            return _resumed;
        }

        inline
        minko::render::AbstractContext::Ptr
        context() override
        {
            return _context;
        }

        void
        swapBuffers() override;

        inline
        void
        swapBuffersAtEnterFrame(bool value)
        {
            _swapBuffersAtEnterFrame = value;
        }

        inline
        bool
        swapBuffersAtEnterFrame()
        {
            return _swapBuffersAtEnterFrame;
        }

        inline
        float
        framerate() override
        {
            return _framerate;
        }

        inline
        float
        desiredFramerate() override
        {
            return _desiredFramerate;
        }

        void
        desiredFramerate(float desiredFramerate) override;

        // Current frame execution time in milliseconds.
        inline
        float
        frameDuration() const override
        {
            return _frameDuration;
        }

        // Time in millisecond since last frame.
        inline
        float
        deltaTime() const
        {
            return _deltaTime;
        }

        // Time in milliseconds since application started.
        inline
        float
        relativeTime() const override
        {
            return _relativeTime;
        }

        WorkerPtr
        getWorker(const std::string& name) override;

        bool
        isWorkerRegistered(const std::string& name) override
        {
            return _workers.count(name) != 0;
        };

        void
        run();

        void
        quit();

        void
        resetInputs();

    private:
        Canvas(const std::string&   name,
               const uint           width,
               const uint           height,
               int                  flags);

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
        initializeContext();

        void
        initializeWindow();

    public:
        void
        step();
    };
}
