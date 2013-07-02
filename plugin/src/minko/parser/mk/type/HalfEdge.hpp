#pragma once

#include "minko/MkCommon.hpp"

namespace minko
{
	namespace parser
	{
		namespace mk
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
				BOOLEAN						_marked;

			private :
				HalfEdge(unsigned int startNodeId,
						 unsigned int endNodeId,
						 unsigned int edgeId);

			public:
				bool			indiceInEdge(unsigned int indice);
				bool			indiceInFace(unsigned int indice);
				unsigned int	getThirdVertex();
	
				inline BOOLEAN marked() const
				{
					return _marked;
				}

				inline unsigned int startNodeId() const
				{
					return _startNodeId;
				}

				inline void marked(BOOLEAN value)
				{
					_marked = value;
				}

				inline unsigned int endNodeId() const
				{
					return _endNodeId;
				}

				inline unsigned int edgeId() const
				{
					return _edgeId;
				}

				inline HalfEdgePtr next() const
				{
					return _next;
				}

				inline void next(HalfEdgePtr value)
				{
					_next = value;
				}

				inline HalfEdgePtr prec() const
				{
					return _prec;
				}

				inline void prec(HalfEdgePtr value)
				{
					_prec = value;
				}

				inline HalfEdgePtr adjacent() const
				{
					return _adjacent;
				}

				inline void adjacent(HalfEdgePtr value)
				{
					_adjacent = value;
				}

				inline const std::vector<HalfEdgePtr>& face() const
				{
					return _face;
				}

				inline std::vector<HalfEdgePtr>& face() 
				{
					return _face;
				}

				void setFace(HalfEdgePtr he1,
							 HalfEdgePtr he2,
				 			 HalfEdgePtr he3);

				inline std::vector<HalfEdgePtr>& secondReverseFace() 
				{
					if (_secondReverseFace.size() == 0)
					{
						_secondReverseFace.push_back(_face[2]);
						_secondReverseFace.push_back(_face[0]);
						_secondReverseFace.push_back(_face[1]);
					}

					return _secondReverseFace;
				}

				inline std::vector<HalfEdgePtr>& firstReverseFace() 
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
}

