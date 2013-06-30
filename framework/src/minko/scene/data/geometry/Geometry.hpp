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
#include "minko/scene/data/DataProvider.hpp"
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
