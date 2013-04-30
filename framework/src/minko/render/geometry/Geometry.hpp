#pragma once

#include "minko/Common.hpp"

namespace minko
{
	namespace render
	{
		namespace geometry
		{
			class Geometry
			{
			public:
				typedef std::shared_ptr<Geometry> ptr;

				struct VertexXYZ
				{
					float x;
					float y;
					float z;
				};

			private:
				VertexXYZ*		_vertices;
				short*			_indices;

			protected:

				Geometry()
				{
				}

				Geometry(VertexXYZ* vertices, short* indices)
					: _vertices(vertices), _indices(indices)
				{
				}

			public:

				static
				ptr
				create()
				{
					return std::shared_ptr<Geometry>(new Geometry());
				}

				static
				ptr
				create(VertexXYZ* vertices, short* indices)
				{
					return std::shared_ptr<Geometry>(new Geometry(vertices, indices));
				}

				void
				vertices(VertexXYZ* vertices)
				{
					_vertices = vertices;
				}

				void
				indices(short* indices)
				{
					_indices = indices;
				}
			};
		}
	}
}
