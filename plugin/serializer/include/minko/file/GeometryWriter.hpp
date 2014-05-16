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
#include "minko/file/WriterOptions.hpp"

namespace minko
{
	namespace file
	{
		class GeometryWriter :
			public AbstractWriter<std::shared_ptr<geometry::Geometry>>
		{
		public:
			typedef std::shared_ptr<GeometryWriter> Ptr;
			typedef std::function<std::string(std::shared_ptr<render::IndexBuffer>)>	IndexBufferWriteFunc;
			typedef std::function<std::string(std::shared_ptr<render::VertexBuffer>)>	VertexBufferWriteFunc;
			typedef std::function<bool(std::shared_ptr<geometry::Geometry>)>			GeometryTestFunc;

		private:
			typedef std::shared_ptr<file::WriterOptions> WriterOptionsPtr;

		private :
			static std::unordered_map<uint, IndexBufferWriteFunc>		indexBufferWriterFunctions;
			static std::unordered_map<uint, VertexBufferWriteFunc>		vertexBufferWriterFunctions;

			static std::unordered_map<uint, GeometryTestFunc>			indexBufferTestFunctions;
			static std::unordered_map<uint, GeometryTestFunc>			vertexBufferTestFunctions;


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
				  Dependency::Ptr					dependency,
				  WriterOptionsPtr					writerOptions)
			{
				geometry::Geometry::Ptr		geometry				= data();
				uint						indexBufferFunctionId	= 0;
				uint						vertexBufferFunctionId	= 0;
				uint						metaByte				= computeMetaByte(geometry, indexBufferFunctionId, vertexBufferFunctionId, writerOptions);
				const std::string&			serializedIndexBuffer	= indexBufferWriterFunctions[indexBufferFunctionId](geometry->indices());
				std::vector<std::string>	serializedVertexBuffers;
				std::stringstream			sbuf;

				for (std::shared_ptr<render::VertexBuffer> vertexBuffer : geometry->vertexBuffers())
					serializedVertexBuffers.push_back(vertexBufferWriterFunctions[vertexBufferFunctionId](vertexBuffer));

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
			registerIndexBufferWriterFunction(IndexBufferWriteFunc f, GeometryTestFunc testFunc, uint functionId)
			{
				indexBufferWriterFunctions[functionId]	= f;
				indexBufferTestFunctions[functionId]	= testFunc;
			}

			inline
			static
			void
			registerVertexBufferWriterFunction(VertexBufferWriteFunc f, GeometryTestFunc testFunc, uint functionId)
			{
				vertexBufferWriterFunctions[functionId] = f;
				vertexBufferTestFunctions[functionId]	= testFunc;
			}

			static
			std::string
			serializeIndexStream(std::shared_ptr<render::IndexBuffer> indexBuffer);

			static
			bool
			indexBufferFitCharCompression(std::shared_ptr<geometry::Geometry> geometry);

		private:

			void
			initialize();

			unsigned char
			computeMetaByte(std::shared_ptr<geometry::Geometry> geometry,
							uint&								indexBufferFunctionId,
							uint&								vertexBufferFunctionId,
							WriterOptionsPtr					writerOptions);


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
