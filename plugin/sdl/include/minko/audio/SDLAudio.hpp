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

#include "minko/Common.hpp"

#ifndef SDL_AUDIO_ENABLED
# if MINKO_PLATFORM == MINKO_PLATFORM_HTML5 ||    \
     MINKO_PLATFORM == MINKO_PLATFORM_WINDOWS ||  \
     MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
#  define SDL_AUDIO_ENABLED 1
# elif MINKO_PLATFORM == MINKO_PLATFORM_IOS && MINKO_DEVICE == MINKO_DEVICE_NATIVE
#  define SDL_AUDIO_ENABLED 1
# endif
#endif

namespace minko
{
    class Canvas;

    namespace audio
    {
        class SDLAudio
        {
            friend class Canvas;

        public:
            static
            std::shared_ptr<SDLAudio>
            create(std::shared_ptr<Canvas> canvas);

            ~SDLAudio();

        private:
            SDLAudio(std::shared_ptr<Canvas> canvas);
        };
    }
}
