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
#include "minko/Signal.hpp"

namespace
{
}

namespace minko
{
    namespace data
    {
        class Value
        {
        public:
            typedef std::shared_ptr<Value>  Ptr;

        protected:
            std::shared_ptr<Signal<Ptr>>    _changed;

        public:
            inline
            std::shared_ptr<Signal<Ptr>>
            changed()
            {
                return _changed;
            }

            virtual
            ~Value()
            {
            }

            virtual
            bool
            operator==(const Value& x) const
            {
                return false;
            }

        protected:
            Value() :
                _changed(Signal<Ptr>::create())
            {
            }

            Value(std::shared_ptr<Signal<Ptr>> changed) :
                _changed(changed)
            {
            }
        };
    }
}
