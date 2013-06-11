#pragma once

#include "minko/Common.hpp"

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
					typedef std::shared_ptr<Geometry> ptr;

				private:
					std::shared_ptr<DataProvider>	_data;

				public:
					inline
					std::shared_ptr<DataProvider>
					data()
					{
						return _data;
					}

					static
					ptr
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
						_data->setProperty("geometry/indices", indices);
					}

					inline
					std::shared_ptr<IndexStream>
					indices()
					{
						return _data->getProperty<std::shared_ptr<IndexStream>>("geometry/indices");
					}

					template <typename T>
					inline
					void
					addVertexStream(std::shared_ptr<VertexStream<T>> vertexStream)
					{
						_data->setProperty("geometry/vertices/" + vertexStream->name(), vertexStream);
					}

				protected:
					Geometry() :
						_data(DataProvider::create())
					{
					}
				};
			}
		}
	}
}
