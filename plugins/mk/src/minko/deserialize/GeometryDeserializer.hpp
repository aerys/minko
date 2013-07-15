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
#include "minko/Qark.hpp"
#include "minko/AssetsLibrary.hpp"
#include "minko/file/Options.hpp"
#include "minko/render/VertexBuffer.hpp"

namespace minko
{
	namespace deserialize
	{
		class GeometryDeserializer
		{
		private:
			typedef std::vector<std::shared_ptr<scene::Node>> NodeList;

		private:
			static std::map<int, std::string>	_geometryIdToName;
			static std::map<int, NodeList>		_waitForGeometryNodes;

		public:

			template <typename T>
			static void
			read(std::stringstream& stream, T& value);

			static
			void
			deserializeGeometry(bool							isCopy,
								std::string						geometryName,
								int								copyId,
								Qark::ByteArray&				geometryData,
								std::shared_ptr<AssetsLibrary>	library,
								std::shared_ptr<scene::Node>	mesh,
								std::shared_ptr<file::Options>	options);

			static
			std::shared_ptr<render::IndexBuffer>
			readIndexStream(std::stringstream&				stream,
							std::shared_ptr<file::Options>	options);

			static
			std::shared_ptr<render::VertexBuffer>
			readVertexStream(std::stringstream&				stream,
							 std::shared_ptr<file::Options>	options);

			static
			std::vector<render::VertexBuffer::AttributePtr>
			readVertexFormat(std::stringstream&				stream,
							 std::shared_ptr<file::Options> options);
		};
	}
}
