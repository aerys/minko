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
        class Binding
        {
        public:
            enum class Source
            {
                TARGET,
                RENDERER,
                ROOT
            };

            enum class Type
            {
                UNSET,
                INT,
                INT2,
                INT3,
                INT4,
                FLOAT,
                FLOAT2,
                FLOAT3,
                FLOAT4,
                BOOL,
                BOOL2,
                BOOL3,
                BOOL4,
                TEXTURE
            };

            class DefaultValue
            {
                friend class Binding;

            private:
                Type _type;
                Any _values;

            public:
                inline
                Type
                type() const
                {
                    return _type;
                }

                template<typename T>
                const std::vector<T>&
                values() const
                {
                    return *Any::cast<std::vector<T>>(&_values);
                }
            };

        private:
            std::string     _propertyName;
            Source          _source;
            DefaultValue*   _defaultValue;

        public:
            Binding() :
                _propertyName(),
                _source(Source::TARGET),
                _defaultValue(nullptr)
            {}

            Binding(const std::string& propertyName, Source source) :
                _propertyName(propertyName),
                _source(source),
                _defaultValue(nullptr)
            {}

            ~Binding()
            {
                if (_defaultValue)
                    delete _defaultValue;
            }

            inline
            const std::string&
            propertyName() const
            {
                return _propertyName;
            }
            inline
            void
            propertyName(const std::string& p)
            {
                _propertyName = p;
            }

            inline
            const Source&
            source() const
            {
                return _source;
            }
            inline
            void
            source(Source s)
            {
                _source = s;
            }

            inline
            bool
            hasDefautValue() const
            {
                return _defaultValue != nullptr;
            }

            inline
            const DefaultValue&
            defaultValue() const
            {
                return *_defaultValue;
            }

            template <typename T>
            void
            defaultValue(Type type, const std::vector<T>& values)
            {
                if (!_defaultValue)
                    _defaultValue = new DefaultValue();

                _defaultValue->_type = type;    
                _defaultValue->_values = values;
            }

            inline
            bool
            operator==(const Binding& rhs) const
            {
                return _propertyName == rhs._propertyName && _source == rhs._source;
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
            auto h1 = std::hash<minko::uint>()((minko::uint)binding.source());
            auto h2 = std::hash<std::string>()(binding.propertyName());

            return h1 ^ (h2 << 1);
        }
    };
}
