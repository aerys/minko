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
#include "minko/file/AbstractWriter.hpp"
#include "minko/geometry/Geometry.hpp"
#include "msgpack.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Dependency.hpp"

namespace minko
{
	namespace file
	{
		class GeometryWriter :
			public AbstractWriter<std::shared_ptr<geometry::Geometry>>
		{
		public:
			typedef std::shared_ptr<GeometryWriter> Ptr;

		private :
			static std::function<std::string(std::shared_ptr<render::IndexBuffer>)>		indexBufferWriterFunction;
			static std::function<std::string(std::shared_ptr<render::VertexBuffer>)>	vertexBufferWriterFunction;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<GeometryWriter>(new GeometryWriter());
			}

			std::string
			embed(std::shared_ptr<AssetLibrary>		assetLibrary,
				  std::shared_ptr<Options>			options,
				  Dependency::Ptr					dependency)
			{
				geometry::Geometry::Ptr		geometry = data();
				uint						metaByte = computeMetaByte(geometry);
				const std::string&			serializedIndexBuffer = indexBufferWriterFunction(geometry->indices());
				std::vector<std::string>	serializedVertexBuffers;
				std::stringstream			sbuf;

				for (std::shared_ptr<render::VertexBuffer> vertexBuffer : geometry->vertexBuffers())
					serializedVertexBuffers.push_back(vertexBufferWriterFunction(vertexBuffer));

				msgpack::type::tuple<unsigned char, std::string, std::string, std::vector<std::string>> res(
					metaByte,
					assetLibrary->geometryName(geometry), 
					serializedIndexBuffer, 
					serializedVertexBuffers);
				msgpack::pack(sbuf, res);

				return sbuf.str();
			}

			inline
			static
			void
			registerIndexBufferWriterFunction(std::function<std::string(std::shared_ptr<render::IndexBuffer>)> f)
			{
				indexBufferWriterFunction = f;
			}

			inline
			static
			void
			registerVertexBufferWriterFunction(std::function<std::string(std::shared_ptr<render::VertexBuffer>)> f)
			{
				vertexBufferWriterFunction = f;
			}

		private:

			void
			initialize();

			unsigned char
			computeMetaByte(std::shared_ptr<geometry::Geometry> geometry);

			static
			std::string
			serializeIndexStream(std::shared_ptr<render::IndexBuffer> indexBuffer); 

			static
			std::string
			serializeIndexStreamChar(std::shared_ptr<render::IndexBuffer> indexBuffer);

			static
			std::string
			serializeVertexStream(std::shared_ptr<render::VertexBuffer> vertexBuffer);

			GeometryWriter()
			{
				initialize();
			}
		};
	}
}
