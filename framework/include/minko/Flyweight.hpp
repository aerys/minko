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

namespace minko
{
    template <typename T>
    class Flyweight
    {
    private:
        const T* _value;

    public:
        static
        std::unordered_set<T>&
        getValues()
        {
            static std::unordered_set<T> values;

            return values;
        }

        Flyweight(const T& v) :
            _value(&(*getValues().insert(v).first))
        {
        }

        template <typename... U>
        Flyweight(U... args) :
            _value(_value = &(*(getValues().emplace(args...).first)))
        {
        }

        Flyweight(const Flyweight& f) :
            _value(f._value)
        {
        }

        Flyweight(Flyweight&& f) :
            _value(f._value)
        {
            f._value = nullptr;
        }

        ~Flyweight()
        {
        }

        inline
        const T*
        value() const
        {
            return _value;
        }

        inline
        const T&
        operator*() const
        {
            return *_value;
        }

        inline
        bool
        operator==(const Flyweight<T>& rhs) const
        {
            return _value == rhs._value;
        }

        inline
        const Flyweight<T>&
        operator=(const Flyweight<T>& rhs)
        {
            _value = rhs._value;

            return *this;
        }

        inline
        Flyweight<T>&
        operator=(Flyweight<T>&& other)
        {
            _value = other._value;

            return *this;
        }
    };
}

namespace std
{
    template <typename T>
    struct hash<minko::Flyweight<T>>
    {
        inline
        size_t
        operator()(const minko::Flyweight<T>& key) const
        {
            return reinterpret_cast<size_t>(key.value());
        }
    };

    template <typename T>
    struct equal_to<minko::Flyweight<T>>
    {
        inline
        bool
        operator()(const minko::Flyweight<T>& a, const minko::Flyweight<T>& b) const
        {
            return a.value() == b.value();
        }
    };
}
