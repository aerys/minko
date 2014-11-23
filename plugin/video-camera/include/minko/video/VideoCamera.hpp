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

#include "minko/video/AbstractVideoCamera.hpp"

#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
# include "minko/video/AndroidVideoCamera.hpp"
#endif

#if MINKO_PLATFORM == MINKO_PLATFORM_HTML5
# include "minko/video/HTML5VideoCamera.hpp"
#endif

namespace minko
{
    namespace video
    {
        class VideoCamera
        {
        public:
            typedef std::shared_ptr<AbstractVideoCamera> Ptr;

        public:
            ~VideoCamera();

            inline
            static
            Ptr
            create()
            {
#if MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
                return AndroidVideoCamera::create();
#elif MINKO_PLATFORM == MINKO_PLATFORM_HTML5
                return HTML5VideoCamera::create();
#else
                return nullptr;
#endif
            }

        private:
            VideoCamera();
        };
    }
}
