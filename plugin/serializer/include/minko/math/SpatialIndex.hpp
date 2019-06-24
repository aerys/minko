/*
Copyright (c) 2015 Aerys

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
#include "minko/Hash.hpp"

namespace minko
{
    namespace math
    {
        template <typename T>
        class SpatialIndex
        {
        public:
            typedef std::shared_ptr<SpatialIndex>   Ptr;

        private:
            struct                                  IndexHash;
            struct                                  IndexEqual;

            typedef std::unordered_map<
                math::vec3,
                T,
                IndexHash,
                IndexEqual
            >                                       Index;

            struct IndexHash
            {
                float precision;

                std::size_t
                operator()(const math::vec3& position) const
                {
                    return minko::Hash<math::vec3>()(
                        math::floor(position * precision + math::vec3(0.5f)) / precision
                    );
                }

                explicit
                IndexHash(float epsilon) :
                    precision(1.f / epsilon)
                {
                }
            };

            struct IndexEqual
            {
                float epsilon;

                bool
                operator()(const math::vec3& lhs, const math::vec3& rhs) const
                {
                    const auto epsilonEqual = math::epsilonEqual(lhs, rhs, epsilon);

                    return epsilonEqual.x && epsilonEqual.y && epsilonEqual.z;
                }

                explicit
                IndexEqual(float epsilon) :
                    epsilon(epsilon)
                {
                }
            };

        private:
            Index _index;

        public:
            static
            Ptr
            create(float epsilon = 1e-5f)
            {
                auto instance = Ptr(new SpatialIndex<T>(epsilon));

                return instance;
            }

            T&
            at(const math::vec3& position)
            {
                return _index[position];
            }

            const T&
            at(const math::vec3& position) const
            {
                return _index.at(position);
            }

            T&
            operator[](const math::vec3& position)
            {
                return at(position);
            }

            const T&
            operator[](const math::vec3& position) const
            {
                return at(position);
            }

            std::size_t
            size() const
            {
                return _index.size();
            }

            void
            clear()
            {
                _index.clear();
            }

            typename Index::iterator
            begin()
            {
                return _index.begin();
            }

            typename Index::iterator
            end()
            {
                return _index.end();
            }

            typename Index::const_iterator
            begin() const
            {
                return _index.begin();
            }

            typename Index::const_iterator
            end() const
            {
                return _index.end();
            }

        private:
            explicit
            SpatialIndex(float epsilon) :
                _index(0u, IndexHash(epsilon), IndexEqual(epsilon))
            {
            }
        };
    }
}
