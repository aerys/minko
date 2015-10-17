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
    template <typename T>
    struct Hash;

    // Explicit specialisations of Hash are expected to expose operator().
    //
    // size_t
    // operator()(const T& x) const;

    template <typename T>
    struct EqualTo
    {
        virtual
        bool
        operator()(const T& a, const T& b) const = 0;
    };

    template <typename T, typename H = Hash<T>>
    inline
    void
    hash_combine(std::size_t& seed, const T& v)
    {
        H hasher;

        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template <class T>
    struct Hash<T*>
    {
        size_t
        operator()(const T* x) const
        {
            size_t value = (size_t)x;

            value = ~value + (value << 15);
            value = value ^ (value >> 12);
            value = value + (value << 2);
            value = value ^ (value >> 4);
            value = value * 2057;
            value = value ^ (value >> 16);

            return value;
        }
    };
    
    template<>
    struct Hash<minko::render::TextureFormat>
    {
        inline
        size_t
        operator()(const minko::render::TextureFormat& x) const
        {
            return std::hash<unsigned int>()(static_cast<unsigned int>(x));
        }
    };

    // generic std::hash and std::equal_to specializations for std::pair
    template<typename T, typename V>
    struct Hash<std::pair<T, V>>
    {
        inline
        size_t
        operator()(const std::pair<T, V>& key) const
        {
            size_t seed = 0;

            hash_combine(seed, key.first);
            hash_combine(seed, key.second);

            return seed;
        }
    };

    template<typename T, typename V>
    struct EqualTo<std::pair<T, V>>
    {
        inline
        bool
        operator()(const std::pair<T, V>& lhs, const std::pair<T, V>& rhs) const
        {
            return lhs.first == rhs.first && lhs.second == rhs.second;
        }
    };

    // generic std::hash and std::equal_to specializations for std::tuple
    // https://stackoverflow.com/questions/7110301/generic-hash-for-std::tuples-in-unordered-map-unordered-set
    namespace
    {
        // Recursive template code derived from Matthieu M.
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct HashValueImpl
        {
            static void apply(size_t& seed, Tuple const& tuple)
            {
                HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
                hash_combine(seed, std::get<Index>(tuple));
            }
        };

        template <class Tuple>
        struct HashValueImpl<Tuple, 0>
        {
            static void apply(size_t& seed, Tuple const& tuple)
            {
                hash_combine(seed, std::get<0>(tuple));
            }
        };
    }

    template <typename... T>
    struct Hash<std::tuple<T...>>
    {
        inline
        size_t
        operator()(std::tuple<T...> const& tt) const
        {
            size_t seed = 0;

            HashValueImpl<std::tuple<T...> >::apply(seed, tt);

            return seed;
        }
    };

    template <typename... T>
    struct EqualTo<std::tuple<T...>>
    {
        inline
        bool
        operator()(const std::tuple<T...>& lhs, const std::tuple<T...>& rhs) const
        {
            return lhs == rhs;
        }
    };

    template <>
    struct Hash<math::vec3>
    {
        inline
        std::size_t
        operator()(const math::vec3& value) const
        {
            auto seed = std::size_t();

            hash_combine<float, std::hash<float>>(seed, value.x);
            hash_combine<float, std::hash<float>>(seed, value.y);
            hash_combine<float, std::hash<float>>(seed, value.z);

            return seed;
        }
    };
}
