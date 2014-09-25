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

#include "minko/Any.hpp"
#include "minko/Common.hpp"
#include "minko/video/AbstractVideoCamera.hpp"

namespace minko
{
    namespace video
    {
        class AndroidVideoCamera :
            public AbstractVideoCamera
        {
        public:
            typedef std::shared_ptr<AndroidVideoCamera> Ptr;

        private:
            typedef std::shared_ptr<component::SceneManager> SceneManagerPtr;

            struct JniImpl;

        private:
            static const unsigned int _defaultDesiredFrameRate;

            SceneManagerPtr _sceneManager;

            FrameSignal::Ptr _frameReceived;

            Any _frameBeginSlot;
            Any _frameEndSlot;

            unsigned int _desiredFrameRate;

            std::shared_ptr<JniImpl> _jniImpl;

        public:
            ~AndroidVideoCamera() = default;

            inline
            static
            Ptr
            create(SceneManagerPtr sceneManager)
            {
                auto instance = Ptr(new AndroidVideoCamera());

                instance->_sceneManager = sceneManager;

                instance->initialize();

                return instance;
            }

            inline
            AbstractVideoCamera::Ptr
            desiredFrameRate(unsigned int frameRate)
            {
                if (frameRate == 0)
                    throw std::invalid_argument("AbstractVideoCamera::desiredFrameRate, frameRate");

                _desiredFrameRate = frameRate;

                return shared_from_this();
            }

            AbstractVideoCamera::Ptr
            desiredSize(unsigned int width, unsigned int height);

            inline
            FrameSignal::Ptr
            frameReceived()
            {
                return _frameReceived;
            }

            void
            start();

            void
            stop();

        private:
            AndroidVideoCamera();

            void
            initialize();

            void
            frameBeginHandler(SceneManagerPtr sceneManager, float time, float deltaTime);

            void
            frameEndHandler(SceneManagerPtr sceneManager, float time, float deltaTime);

            bool
            retrieveFrame();
        };
    }
}
