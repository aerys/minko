#pragma once

#include "minko/MkCommon.hpp"
#include "minko/parser/mk/type/HalfEdge.hpp"

namespace minko
{
	namespace parser
	{
		namespace mk
		{

			struct pair_hash
			{
				long operator()(const std::pair<unsigned short, unsigned short> pair) const 
				{ 
					return pair.first * 10000 + pair.second;
				}

			};

			struct pair_comparer
			{
				bool operator()(const std::pair<unsigned short, unsigned short> left, 
								const std::pair<unsigned short, unsigned short> right) const
				{
					return (left.first == right.first) && (left.second == right.second);
				}
			};
			
			class HalfEdgeCollection 
			{
			private:
				typedef std::pair<unsigned short, unsigned short>	PairOfShort;
				typedef std::shared_ptr<HalfEdge>					HalfEdgePtr;

			private :
				std::shared_ptr<IndexStream>	_indexStream;
				std::list<HalfEdge>				_subMeshesList;

			public:
				HalfEdgeCollection (std::shared_ptr<IndexStream> indexStream);
				~HalfEdgeCollection ();

				inline std::list<HalfEdge> subMeshesList() const
				{
					return _subMeshesList;
				};

			private:
				void initialize();
				void computeList();
			};
		}
	}
}