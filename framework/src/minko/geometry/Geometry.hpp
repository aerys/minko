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
#include "minko/data/Provider.hpp"
#include "minko/render/VertexBuffer.hpp"

namespace minko
{
	namespace geometry
	{
		class Geometry
		{
		public:
			typedef std::shared_ptr<Geometry> Ptr;

		private:
			std::shared_ptr<data::Provider>	_data;
			unsigned int					_vertexSize;

		public:
			inline
			std::shared_ptr<data::Provider>
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
			std::shared_ptr<data::Provider>
			vertices()
			{
				return _data;
			}

			inline
			void
			indices(std::shared_ptr<render::IndexBuffer> indices)
			{
				_data->set("geometry.indices", indices);
			}

			inline
			std::shared_ptr<render::IndexBuffer>
			indices()
			{
				return _data->get<std::shared_ptr<render::IndexBuffer>>("geometry.indices");
			}

			inline
			void
			addVertexBuffer(std::shared_ptr<render::VertexBuffer> VertexBuffer)
			{
				for (auto attribute : VertexBuffer->attributes())
				{
					_data->set("geometry.vertex.attribute." + std::get<0>(*attribute), VertexBuffer);
					_vertexSize += std::get<1>(*attribute);
				}

				_data->set("geometry.vertex.size", _vertexSize);
			}
			
			inline
			unsigned int
			vertexSize()
			{
				return _vertexSize;
			};

		protected:
			Geometry() :
				_data(data::Provider::create()),
				_vertexSize(0)
			{
			}

			inline
			void
			vertexSize(unsigned int value)
			{
				_vertexSize = value;
			};
		};
	}
}
