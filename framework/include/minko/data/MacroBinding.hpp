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
#include "minko/data/Binding.hpp"

namespace minko
{
    namespace data
    {
        class MacroBinding : public Binding
        {
        private:
            Type    _type;
            int     _minValue;
            int     _maxValue;

        public:
            MacroBinding(const std::string& propertyName,
                         Source             source,
                         Type               type,
                         int                min = -std::numeric_limits<int>::max(),
                         int                max = std::numeric_limits<int>::max()) :
                Binding(propertyName, source),
                _type(type),
                _minValue(min),
                _maxValue(max)
            {}

            MacroBinding() :
                Binding(),
                _type(Type::UNSET),
                _minValue(-std::numeric_limits<int>::max()),
                _maxValue(std::numeric_limits<int>::max())
            {}

            inline
            Type
            type() const
            {
                return _type;
            }
            inline
            void
            type(Type t)
            {
                _type = t;
            }

            inline
            int
            minValue() const
            {
                return _minValue;
            }
            inline
            void
            minValue(int v)
            {
                _minValue = v;
            }
            
            inline
            int
            maxValue() const
            {
                return _maxValue;
            }
            inline
            void
            maxValue(int v)
            {
                _maxValue = v;
            }
        };
    }
}
