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

namespace std
{
    template<>
	struct hash<minko::math::mat4>
	{
		inline
		size_t
		operator()(const minko::math::mat4& matrix) const
		{
			return (size_t)minko::math::value_ptr(matrix);
		}
	};

#ifdef __ANDROID__
    template <typename T>
    inline
    string
    to_string(T v)
    {
        ostringstream oss;
        oss << v;
        return oss.str();
    }

    inline
    unsigned long
    stoul(const std::string& str, std::size_t* pos = 0, int base = 10)
    {
        // FIXME: Deal with pos != nullptr.
        return strtoul(str.c_str(), 0, base);
    }
#endif

	inline
	std::string
	to_string(const minko::math::mat4& matrix)
	{
		std::string str = "mat4(";
		auto ptr = minko::math::value_ptr(matrix);
        for (auto i = 0; i < 15; ++i)
        {
            if (i % 4 == 0)
                str += "\n";

            str += std::to_string(ptr[i]) + ", ";
        }

		str += std::to_string(ptr[15]) + ")";

		return str;
	}

    template<typename T, minko::math::precision P>
    inline
    std::string
    to_string(const minko::math::tvec1<T, P>& v)
    {
        return minko::math::to_string(v);
    }

    template<typename T, minko::math::precision P>
    inline
    std::string
    to_string(const minko::math::tvec2<T, P>& v)
    {
        return minko::math::to_string(v);
    }

    template<typename T, minko::math::precision P>
    inline
    std::string
    to_string(const minko::math::tvec3<T, P>& v)
    {
        return minko::math::to_string(v);
    }

    template<typename T, minko::math::precision P>
    inline
    std::string
    to_string(const minko::math::tvec4<T, P>& v)
    {
        return minko::math::to_string(v);
    }

    template<typename T>
    inline
    std::string
    to_string(const std::list<T>& list, const std::string& separator = ", ")
    {
        std::string s = "";

        for (const T& e : list)
        {
            if (s.size())
                s += separator;
            s += std::to_string(e);
        }

        return s;
    }

    template<>
    inline
    std::string
    to_string<std::string>(const std::list<std::string>& list, const std::string& separator)
    {
        std::string s = "";

        for (const std::string& e : list)
        {
            if (s.size())
                s += separator;
            s += e;
        }

        return s;
    }

    template<typename T>
    inline
    std::string
    to_string(const std::vector<T>& list, const std::string& separator = ", ")
    {
        std::string s = "";

        for (const T& e : list)
        {
            if (s.size())
                s += separator;
            s += std::to_string(e);
        }

        return s;
    }

    template<>
    inline
    std::string
    to_string<std::string>(const std::vector<std::string>& list, const std::string& separator)
    {
        std::string s = "";

        for (const std::string& e : list)
        {
            if (s.size())
                s += separator;
            s += e;
        }

        return s;
    }

    inline
    std::string
    replaceAll(std::string str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;

        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }

        return str;
    }
}

// generic std::hash and std::equal_to specializations for std::pair
namespace std
{
    /*template<typename T, typename V>
    struct hash<pair<T, V>>
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
    struct equal_to<pair<T, V>>
    {
        inline
        bool
        operator()(const std::pair<T, V>& lhs, const std::pair<T, V>& rhs) const
        {
            return lhs.first == rhs.first && lhs.second == rhs.second;
        }
    };*/
}

// generic std::equal_to specializations for std::shared_ptr
namespace std
{
    /*template <typename T>
    struct equal_to<shared_ptr<T>>
    {
        inline
        bool
        operator()(const shared_ptr<T>& lhs, const shared_ptr<T>& rhs) const
        {
            return lhs == rhs;
        }
    };*/
}

// generic std::hash and std::equal_to specializations for std::tuple
// https://stackoverflow.com/questions/7110301/generic-hash-for-tuples-in-unordered-map-unordered-set
namespace std
{
    //namespace
    //{
    //    // Recursive template code derived from Matthieu M.
    //    template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
    //    struct HashValueImpl
    //    {
    //        static void apply(size_t& seed, Tuple const& tuple)
    //        {
    //            HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
    //            hash_combine(seed, std::get<Index>(tuple));
    //        }
    //    };

    //    template <class Tuple>
    //    struct HashValueImpl<Tuple,0>
    //    {
    //        static void apply(size_t& seed, Tuple const& tuple)
    //        {
    //            hash_combine(seed, std::get<0>(tuple));
    //        }
    //    };
    //}

    //template <typename ... T>
    //struct hash<tuple<T...>>
    //{
    //    inline
    //    size_t
    //    operator()(tuple<T...> const& tt) const
    //    {
    //        size_t seed = 0;

    //        HashValueImpl<tuple<T...> >::apply(seed, tt);

    //        return seed;
    //    }
    //};

    //template <typename ... T>
    //struct equal_to<tuple<T...>>
    //{
    //    inline
    //    bool
    //    operator()(const tuple<T...>& lhs, const tuple<T...>& rhs) const
    //    {
    //        return lhs == rhs;
    //    }
    //};
}
