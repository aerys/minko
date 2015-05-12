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
#include "minko/render/IndexBuffer.hpp"
#include "minko/data/HalfEdge.hpp"
#include <queue>

namespace minko
{
    namespace data
    {
        struct pair_hash
        {
            long
            operator()(const std::pair<unsigned int, unsigned int> pair) const
            {
                return pair.first * 10000 + pair.second;
            }

        };

        struct pair_comparer
        {
            bool
            operator()(const std::pair<unsigned int, unsigned int> left,
                       const std::pair<unsigned int, unsigned int> right) const
            {
                return (left.first == right.first) && (left.second == right.second);
            }
        };

        class HalfEdgeCollection
        {
        public:
            typedef std::shared_ptr<HalfEdgeCollection> Ptr;

        private:
            typedef std::shared_ptr<minko::render::IndexBuffer>                              IndexStreamPtr;
            typedef std::pair<unsigned int, unsigned int>                                    PairOfUInt;
            typedef std::shared_ptr<HalfEdge>                                                HalfEdgePtr;
            typedef std::unordered_map<PairOfUInt, HalfEdgePtr, pair_hash, pair_comparer>    HalfEdgeMap;
            typedef std::list<HalfEdgePtr>                                                   HalfEdgeList;

        private:
            std::vector<unsigned int>       _indices;
            std::list<HalfEdgeList>         _subMeshesList;
            HalfEdgeList                    _halfEdges;

        public:
            inline 
            static
            Ptr
            create(const std::vector<unsigned int>& indices)
            {
                return Ptr(new HalfEdgeCollection(indices));
            }

            inline
            std::list<HalfEdgeList>
            subMeshesList() const
            {
                return _subMeshesList;
            };

            inline
            const HalfEdgeList&
            halfEdges() const
            {
                return _halfEdges;
            }

        private:
            explicit
            HalfEdgeCollection(const std::vector<unsigned int>& indices);

            void
            initialize();

            void
            computeList(HalfEdgeMap unmarked);
        };
    }
}