/*
Copyright (c) 2013 Aerys

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
			operator()(const std::pair<unsigned short, unsigned short> pair) const 
			{ 
				return pair.first * 10000 + pair.second;
			}

		};

		struct pair_comparer
		{
			bool 
			operator()(const std::pair<unsigned short, unsigned short> left, 
					   const std::pair<unsigned short, unsigned short> right) const
			{
				return (left.first == right.first) && (left.second == right.second);
			}
		};

		class HalfEdgeCollection 
		{
		private:
			typedef std::shared_ptr<minko::render::IndexBuffer>							IndexStreamPtr;
			typedef std::pair<unsigned short, unsigned short>								PairOfShort;
			typedef std::shared_ptr<HalfEdge>												HalfEdgePtr;
			typedef std::unordered_map<PairOfShort, HalfEdgePtr, pair_hash, pair_comparer>	HalfEdgeMap;
			typedef std::list<HalfEdgePtr>													HalfEdgeList;

		private :
			IndexStreamPtr				_indexStream;
			std::list<HalfEdgeList>		_subMeshesList;

		public:

			inline static
			std::shared_ptr<HalfEdgeCollection> 
			create(std::shared_ptr<minko::render::IndexBuffer> indexStream)
			{
				return std::shared_ptr<HalfEdgeCollection>(new HalfEdgeCollection(indexStream));
			}

			inline 
			std::list<HalfEdgeList> 
			subMeshesList() const
			{
				return _subMeshesList;
			};

		private:
			HalfEdgeCollection (std::shared_ptr<minko::render::IndexBuffer> indexStream);

			void 
			initialize();

			void 
			computeList(HalfEdgeMap unmarked);
		};
	}
}