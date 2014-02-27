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

namespace minko
{
	namespace data
	{

		class HalfEdge
		{
		public :
			typedef std::shared_ptr<HalfEdge>	HalfEdgePtr;

		public:
			inline static
			HalfEdgePtr	create(unsigned int startNodeId,
							   unsigned int endNodeId,
							   unsigned int edgeId)
			{
				auto he = std::shared_ptr<HalfEdge>(new HalfEdge(startNodeId, endNodeId, edgeId));

				return he;
			}

		private:
			unsigned int				_startNodeId;
			unsigned int				_endNodeId;
			unsigned int				_edgeId;
			HalfEdgePtr					_next;
			HalfEdgePtr					_prec;
			HalfEdgePtr					_adjacent;
			std::vector<HalfEdgePtr>	_face;
			std::vector<HalfEdgePtr>	_firstReverseFace;
			std::vector<HalfEdgePtr>	_secondReverseFace;
			bool						_marked;

		private :
			HalfEdge(unsigned int startNodeId,
					 unsigned int endNodeId,
					 unsigned int edgeId);

		public:
			bool			
			indiceInEdge(unsigned int indice);

			bool
			indiceInFace(unsigned int indice);
			
			unsigned int	
			getThirdVertex();

			inline 
			bool
			marked() const
			{
				return _marked;
			}

			inline 
			unsigned int 
			startNodeId() const
			{
				return _startNodeId;
			}

			inline 
			void 
			marked(bool value)
			{
				_marked = value;
			}

			inline 
			unsigned int 
			endNodeId() const
			{
				return _endNodeId;
			}

			inline 
			unsigned int 
			edgeId() const
			{
				return _edgeId;
			}

			inline 
			HalfEdgePtr 
			next() const
			{
				return _next;
			}

			inline 
			void 
			next(HalfEdgePtr value)
			{
				_next = value;
			}

			inline 
			HalfEdgePtr 
			prec() const
			{
				return _prec;
			}

			inline 
			void 
			prec(HalfEdgePtr value)
			{
				_prec = value;
			}

			inline 
			HalfEdgePtr
			adjacent() const
			{
				return _adjacent;
			}

			inline 
			void 
			adjacent(HalfEdgePtr value)
			{
				_adjacent = value;
			}

			inline 
			const std::vector<HalfEdgePtr>& 
			face() const
			{
				return _face;
			}

			inline 
			std::vector<HalfEdgePtr>& 
			face() 
			{
				return _face;
			}

			void 
			setFace(HalfEdgePtr he1,
					HalfEdgePtr he2,
					HalfEdgePtr he3);

			inline 
			std::vector<HalfEdgePtr>& 
			secondReverseFace() 
			{
				if (_secondReverseFace.size() == 0)
				{
					_secondReverseFace.push_back(_face[2]);
					_secondReverseFace.push_back(_face[0]);
					_secondReverseFace.push_back(_face[1]);
				}

				return _secondReverseFace;
			}

			inline 
			std::vector<HalfEdgePtr>& 
			firstReverseFace() 
			{
				if (_firstReverseFace.size() == 0)
				{
					_firstReverseFace.push_back(_face[1]);
					_firstReverseFace.push_back(_face[2]);
					_firstReverseFace.push_back(_face[0]);
				}

				return _firstReverseFace;
			}
		};

		// debug

		std::ostream &operator<<(std::ostream &flux, const HalfEdge& halfEdge);
		std::ostream &operator<<(std::ostream &flux, std::shared_ptr<HalfEdge> halfEdge);

	}
}

