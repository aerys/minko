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

namespace minko
{
    namespace audio
    {
        class SoundTransform :
            public std::enable_shared_from_this<SoundTransform>
        {
        public:
            typedef std::shared_ptr<SoundTransform>        Ptr;

            static
            Ptr
            create(float volume = 1.0f)
            {
                return Ptr(new SoundTransform(volume));
            }

            float
            volume()
            {
                return _volume;
            }

            Ptr
            volume(float value)
            {
                if (value < 0.f)
                    _volume = 0.f;
                else if (value > 1.0f)
                    _volume = 1.0f;
                else
                    _volume = value;

                return shared_from_this();
            }

            float
            left()
            {
                return _left;
            }

            Ptr
            left(float value)
            {
                if (value < 0.f)
                    _left = 0.f;
                else if (value > 1.0f)
                    _left = 1.0f;
                else
                    _left = value;

                return shared_from_this();
            }

            float
            right()
            {
                return _right;
            }

            Ptr
            right(float value)
            {
                if (value < 0.f)
                    _right = 0.f;
                else if (value > 1.0f)
                    _right = 1.0f;
                else
                    _right = value;

                return shared_from_this();
            }

            virtual
            ~SoundTransform()
            {
            }

        protected:
            SoundTransform(float volume) :
                _left(1.0f),
                _right(1.0f),
                _volume(volume)
            {
            }

        private:
            float _left;
            float _right;
            float _volume;
        };
    }
}
