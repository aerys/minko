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
#include "minko/Any.hpp"

namespace minko
{
    namespace data
    {
        struct Binding
        {
            enum class Source
            {
                TARGET,
                RENDERER,
                ROOT
            };

            std::string propertyName;
            Source      source;

            Binding() :
                propertyName(),
                source(Source::TARGET)
            {}

            Binding(const std::string& propertyName, Source source) :
                propertyName(propertyName),
                source(source)
            {}

            ~Binding()
            {
            }

            inline
            bool
            operator==(const Binding& rhs) const
            {
                return propertyName == rhs.propertyName && source == rhs.source;
            }
        };
    }
}

namespace std
{
    template<>
    struct hash<minko::data::Binding>
    {
        inline
        size_t
        operator()(const minko::data::Binding& binding) const
        {
            auto h1 = std::hash<minko::uint>()((minko::uint)binding.source);
            auto h2 = std::hash<std::string>()(binding.propertyName);

            return h1 ^ (h2 << 1);
        }
    };
}
