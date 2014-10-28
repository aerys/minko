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
        class HtmlWebCamera :
            public AbstractVideoCamera
        {
        public:
            typedef std::shared_ptr<HtmlWebCamera> Ptr;

        private:
            FrameSignal::Ptr    _frameReceived;
            int                 _frameId;
            static bool         _js_script_loaded;
            int                 _cameraBufferSize;
            int                 _cameraWidth;
            int                 _cameraHeight;

        public:
            ~HtmlWebCamera() = default;

            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new HtmlWebCamera());

                instance->initialize();

                return instance;
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
            HtmlWebCamera();

            void
            initialize();

            void
            requestFrame();

            static
            void
            js_load_success();

            static
            void
            js_load_error();
        };
    }
}
