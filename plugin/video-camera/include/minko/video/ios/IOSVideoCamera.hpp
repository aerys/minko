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

#include "minko/video/ios/VideoSource.h"

namespace minko
{
    namespace video
    {
        struct ImageBuffer
        {
            std::vector<unsigned char> data;
            int width;
            int height;
        };
        
        class IOSVideoCamera :
            public AbstractVideoCamera
        {
        public:
            typedef std::shared_ptr<IOSVideoCamera> Ptr;
            typedef minko::Signal<const std::vector<unsigned char>&, int, int> IOSFrameSignal;

        private:
            typedef std::shared_ptr<component::SceneManager> SceneManagerPtr;

            FrameSignal::Ptr        _frameReceived;
            IOSFrameSignal::Slot    _iOSFrameReceivedSlot;

            VideoSourceImpl* const  _videoSourceImpl;
            bool                    _receivedFrameData;

        public:
            ~IOSVideoCamera();

            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new IOSVideoCamera());

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
            IOSVideoCamera();

            void
            initialize();

            void
            requestFrame();
            
            void
            decodeBGRAFrame(const unsigned char* data,
                            std::vector<unsigned char>& rgbaFrame,
                            int width,
                            int height);
            
            ImageBuffer _imageBuffer;
            std::vector<unsigned char> _rgbaFrame;
        };
    }
}
