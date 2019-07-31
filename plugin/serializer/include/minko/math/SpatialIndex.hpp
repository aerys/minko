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
            struct                                  CompareVec3;
            struct                                  IndexHash;
            struct                                  IndexEqual;

            typedef std::map<
                math::vec3,
                T,
                CompareVec3
            >                                       Index;

            struct CompareVec3
            {
                float epsilon;

                bool
                operator()(const math::vec3& lhs, const math::vec3& rhs) const
                {
                    const auto epsilonEqual = math::epsilonEqual(lhs, rhs, epsilon);

                    if (!epsilonEqual.x)
                        return lhs.x < rhs.x;

                    if (!epsilonEqual.y)
                        return lhs.y < rhs.y;

                    return !epsilonEqual.z && lhs.z < rhs.z;
                }

                explicit
                CompareVec3(float epsilon) :
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
            find(const math::vec3& position)
            {
                return _index.find(position);
            }

            typename Index::const_iterator
            find(const math::vec3& position) const
            {
                return _index.find(position);
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
                _index(CompareVec3(epsilon))
            {
            }
        };
    }
}
