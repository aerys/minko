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

//
//  Created by Warren Seine on Jul 14, 2012.
//  Copyright (c) 2012 Aerys. All rights reserved.
//
//
//  Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <algorithm>
#include <typeinfo>

namespace minko
{
    template <typename T>
    struct RemoveReference
    {
        typedef T Type;
    };

    template <typename T>
    struct RemoveReference<T&>
    {
        typedef T Type;
    };

    class Any
    {
    public:
        Any() :
            _content(0)
        {
        }

        template <typename ValueType>
        Any(const ValueType & value) :
            _content(new Holder<ValueType>(value))
        {
        }

        Any(const Any& other) :
            _content(other._content ? other._content->clone() : 0)
        {
        }

        Any(Any&& other) :
            _content(other._content ? other._content : 0)
        {
            other._content = 0;
        }

        ~Any()
        {
            delete _content;
        }

        Any&
        swap(Any& rhs)
        {
            std::swap(_content, rhs._content);
            return *this;
        }

        template <typename ValueType>
        Any&
        operator=(const ValueType& rhs)
        {
            Any(rhs).swap(*this);
            return *this;
        }

        Any&
        operator=(Any& rhs)
        {
            rhs.swap(*this);
            return *this;
        }

        Any&
        operator=(Any&& rhs)
        {
            _content = rhs._content;

            rhs._content = 0;

            return *this;
        }

        bool
        empty() const
        {
            return !_content;
        }

        const std::type_info&
        type() const
        {
            return _content ? _content->type() : typeid(void);
        }

        template <typename ValueType>
        static ValueType*
        cast(Any* operand)
        {
            return operand && operand->type() == typeid(ValueType) ?
                &static_cast<Any::Holder<ValueType> *>(operand->_content)->_held : 0;
        }

        template <typename ValueType>
        static ValueType*
        unsafe_cast(Any* operand)
        {
            return operand ? &static_cast<Any::Holder<ValueType> *>(operand->_content)->_held : 0;
        }

        template <typename ValueType>
        static inline const ValueType*
        cast(const Any* operand)
        {
            return Any::cast<ValueType>(const_cast<Any*>(operand));
        }

        template <typename ValueType>
        static inline const ValueType*
        unsafe_cast(const Any* operand)
        {
            return Any::unsafe_cast<ValueType>(const_cast<Any*>(operand));
        }

        template <typename ValueType>
        static ValueType
        cast(Any& operand)
        {
            typedef typename RemoveReference<ValueType>::Type NonRef;

            NonRef* result = cast<NonRef>(&operand);

            if (!result)
                throw std::bad_cast();

            return *result;
        }

        template <typename ValueType>
        static ValueType
        unsafe_cast(Any& operand)
        {
            typedef typename RemoveReference<ValueType>::Type NonRef;

            NonRef* result = unsafe_cast<NonRef>(&operand);

            if (!result)
                throw std::bad_cast();

            return *result;
        }

        template <typename ValueType>
        static inline ValueType
        cast(const Any& operand)
        {
            typedef typename RemoveReference<ValueType>::Type NonRef;

            return cast<const NonRef&>(const_cast<Any&>(operand));
        }

        template <typename ValueType>
        static inline ValueType
        unsafe_cast(const Any& operand)
        {
            typedef typename RemoveReference<ValueType>::Type NonRef;

            return unsafe_cast<const NonRef&>(const_cast<Any&>(operand));
        }

        class Placeholder
        {
        public:
            virtual ~Placeholder()
            {
            }

        public:
            virtual const std::type_info&
            type() const = 0;

            virtual Placeholder*
            clone() const = 0;
        };

        template <typename ValueType>
        class Holder :
            public Placeholder
        {
        public:
            Holder(const ValueType & value) :
                _held(value)
            {
            }

            virtual const std::type_info&
            type() const
            {
                return typeid(ValueType);
            }

            virtual Placeholder*
            clone() const
            {
                return new Holder(_held);
            }

        public:
            ValueType _held;

        private:
            // Intentionally left unimplemented.
            Holder& operator=(const Holder &);
        };

        Placeholder* _content;
    };
}
