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
    namespace render
    {
        class Blending
        {
        public:
            enum class Source
            {
                ZERO                 = 1 << 0,
                ONE                  = 1 << 1,
                SRC_COLOR            = 1 << 2,
                ONE_MINUS_SRC_COLOR  = 1 << 3,
                SRC_ALPHA            = 1 << 4,
                ONE_MINUS_SRC_ALPHA  = 1 << 5,
                DST_ALPHA            = 1 << 6,
                ONE_MINUS_DST_ALPHA  = 1 << 7
            };

            enum class Destination
            {
                ZERO                 = 1 << 8,
                ONE                  = 1 << 9,
                DST_COLOR            = 1 << 10,
                ONE_MINUS_DST_COLOR  = 1 << 11,
                SRC_ALPHA_SATURATE   = 1 << 12,
                ONE_MINUS_SRC_ALPHA  = 1 << 13,
                DST_ALPHA            = 1 << 14,
                ONE_MINUS_DST_ALPHA  = 1 << 15
            };

            enum class Mode
            {
                DEFAULT     = (uint)Source::ONE | (uint)Destination::ZERO,
                ALPHA       = (uint)Source::SRC_ALPHA | (uint)Destination::ONE_MINUS_SRC_ALPHA,
                ADDITIVE    = (uint)Source::SRC_ALPHA | (uint)Destination::ONE
            };
        };

        inline
        Blending::Mode
        operator|(Blending::Source s, Blending::Destination d)
        {
            return static_cast<Blending::Mode>(static_cast<unsigned int>(s) | static_cast<unsigned int>(d));
        }

        inline
        uint
        operator&(const Blending::Mode mode, const uint mask)
        {
            return static_cast<uint>(mode) & mask;
        }

        inline
        uint
        operator&(const Blending::Mode mode, const Blending::Destination mask)
        {
            return static_cast<uint>(mode) & static_cast<uint>(mask);
        }

        inline
        uint
        operator&(const Blending::Mode mode, const Blending::Source mask)
        {
            return static_cast<uint>(mode) & static_cast<uint>(mask);
        }
    }
}
