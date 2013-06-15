#pragma once

#include "minko/Common.hpp"
#include "minko/render/stream/VertexFormat.hpp"
#include "minko/render/stream/VertexStream.hpp"
#include "minko/render/stream/IndexStream.hpp"

namespace minko
{
	namespace scene
	{
		namespace data
		{
			namespace geometry
			{
				class CubeGeometry :
					public Geometry
				{
				public:
					typedef std::shared_ptr<Geometry> ptr;

					static
					ptr
					create(std::shared_ptr<AbstractContext> context)
					{
						return std::shared_ptr<CubeGeometry>(new CubeGeometry(context));
					}

				protected:
					CubeGeometry(std::shared_ptr<AbstractContext> context)
					{
						float xyzData[] = {
							0.5, 0.5, 0.5, 			-0.5, 0.5, -0.5, 	0.5, 0.5, -0.5,
							0.5, 0.5, 0.5, 			-0.5, 0.5, 0.5, 		-0.5, 0.5, -0.5,
							-0.5, -0.5, -0.5,		0.5, -0.5, 0.5,		0.5, -0.5, -0.5,
							-0.5, -0.5, 0.5,			0.5, -0.5, 0.5,		-0.5, -0.5, -0.5,
							0.5, -0.5, 0.5,			-0.5, 0.5, 0.5,		0.5, 0.5, 0.5,
							-0.5, 0.5, 0.5,			0.5, -0.5, 0.5,		-0.5, -0.5, 0.5,
							-0.5, 0.5, -0.5,			-0.5, -0.5, -0.5,	0.5, 0.5, -0.5,
							-0.5, -0.5, -0.5,		0.5, -0.5, -0.5,		0.5, 0.5, -0.5,
							-0.5, -0.5, -0.5,		-0.5, 0.5, 0.5,		-0.5, -0.5, 0.5,
							-0.5, 0.5, 0.5,			-0.5, -0.5, -0.5,	-0.5, 0.5, -0.5,
							0.5, -0.5, 0.5,			0.5, 0.5, 0.5,		0.5, 0.5, -0.5,
							0.5, 0.5, -0.5,			0.5, -0.5, -0.5,		0.5, -0.5, 0.5
						};

						unsigned short i[] = {
							0, 1, 2, 3, 4, 5,
							6, 7, 8, 9, 10, 11,
							12, 13, 14, 15, 16, 17,
							18, 19, 20, 21, 22, 23,
							24, 25, 26, 27, 28, 29,
							30, 31, 32, 33, 34, 35
						};

						auto vstream = VertexStream::create(
							context, std::begin(xyzData), std::end(xyzData)
						);

						vstream->addAttribute(VertexAttribute::create("position", 3, 0));
						addVertexStream(vstream);
						
						indices(IndexStream::create(context, std::begin(i), std::end(i)));
					}
				};			
			}
		}
	}
}
