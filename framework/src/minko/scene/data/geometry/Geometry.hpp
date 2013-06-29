#pragma once

#include "minko/Common.hpp"
#include "minko/render/stream/VertexStream.hpp"
#include "minko/render/stream/VertexAttribute.hpp"

namespace
{
	using namespace minko::scene::data;
	using namespace minko::render::stream;
}

namespace minko
{
	namespace scene
	{
		namespace data
		{
			namespace geometry
			{
				class Geometry
				{
				public:
					typedef std::shared_ptr<Geometry> Ptr;

				private:
					std::shared_ptr<DataProvider>	_data;
					unsigned int					_vertexSize;

				public:
					inline
					std::shared_ptr<DataProvider>
					data()
					{
						return _data;
					}

					static
					Ptr
					create()
					{
						return std::shared_ptr<Geometry>(new Geometry());
					}

					inline
					std::shared_ptr<DataProvider>
					vertices()
					{
						return _data;
					}

					inline
					void
					indices(std::shared_ptr<IndexStream> indices)
					{
						_data->set("geometry/indices", indices);
					}

					inline
					std::shared_ptr<IndexStream>
					indices()
					{
						return _data->get<std::shared_ptr<IndexStream>>("geometry/indices");
					}

					inline
					void
					addVertexStream(std::shared_ptr<VertexStream> vertexStream)
					{
						for (auto attribute : vertexStream->attributes())
						{
							_data->set("geometry/vertex/attribute/" + attribute->name(), vertexStream);
							_vertexSize += attribute->size();
						}

						_data->set("geometry/vertex/size", _vertexSize);
					}

				protected:
					Geometry() :
						_data(DataProvider::create()),
						_vertexSize(0)
					{
					}
				};
			}
		}
	}
}
