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

#include "minko/file/GeometryParser.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "msgpack.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/Options.hpp"

using namespace minko;
using namespace minko::file;

std::function<std::shared_ptr<render::IndexBuffer>(std::string, std::shared_ptr<render::AbstractContext>)>	GeometryParser::indexBufferParserFunction;
std::function<std::shared_ptr<render::VertexBuffer>(std::string, std::shared_ptr<render::AbstractContext>)>	GeometryParser::vertexBufferParserFunction;

void
GeometryParser::initialize()
{
	indexBufferParserFunction	= std::bind(&GeometryParser::deserializeIndexBuffer, std::placeholders::_1, std::placeholders::_2);
	vertexBufferParserFunction	= std::bind(&GeometryParser::deserializeVertexBuffer, std::placeholders::_1, std::placeholders::_2);
}

std::shared_ptr<render::VertexBuffer>
GeometryParser::deserializeVertexBuffer(std::string serializedVertexBuffer, std::shared_ptr<render::AbstractContext> context)
{
	msgpack::object																									msgpackObject;
	msgpack::zone																									mempool;
	msgpack::type::tuple<std::string, std::vector<msgpack::type::tuple<std::string, unsigned char, unsigned char>>> deserializedVertex;

	msgpack::unpack(serializedVertexBuffer.data(), serializedVertexBuffer.size(), NULL, &mempool, &msgpackObject);
	msgpackObject.convert(&deserializedVertex);

	std::vector<float>			vector			= deserialize::TypeDeserializer::deserializeVector<float>(deserializedVertex.a0);
	render::VertexBuffer::Ptr	vertexBuffer	= render::VertexBuffer::create(context, vector);

	uint numAttributes = deserializedVertex.a1.size();

	for (unsigned int attributesIndex = 0; attributesIndex < numAttributes; ++attributesIndex)
		vertexBuffer->addAttribute(
			deserializedVertex.a1[attributesIndex].a0, 
			deserializedVertex.a1[attributesIndex].a1,
			deserializedVertex.a1[attributesIndex].a2);

	return vertexBuffer;
}

std::shared_ptr<render::IndexBuffer>
GeometryParser::deserializeIndexBuffer(std::string serializedIndexBuffer, std::shared_ptr<render::AbstractContext> context)
{
	std::vector<unsigned short> vector = deserialize::TypeDeserializer::deserializeVector<unsigned short>(serializedIndexBuffer);

	return render::IndexBuffer::create(context, vector);
}

std::shared_ptr<render::IndexBuffer>
GeometryParser::deserializeIndexBufferChar(std::string serializedIndexBuffer, std::shared_ptr<render::AbstractContext> context)
{
	std::vector<unsigned short> vector = deserialize::TypeDeserializer::deserializeVector<unsigned short, unsigned char>(serializedIndexBuffer);

	return render::IndexBuffer::create(context, vector);
}

void
GeometryParser::parse(const std::string&				filename,
					  const std::string&                resolvedFilename,
					  std::shared_ptr<Options>          options,
					  const std::vector<unsigned char>&	data,
					  std::shared_ptr<AssetLibrary>		assetLibrary)
{
	msgpack::object			msgpackObject;
	msgpack::zone			mempool;
	std::string				str = extractDependencies(assetLibrary, data, options, extractFolderPath(filename));
	geometry::Geometry::Ptr geom = geometry::Geometry::create();
	msgpack::type::tuple<unsigned char, std::string, std::string, std::vector<std::string>> serializedGeometry;

	msgpack::unpack(str.data(), str.size(), NULL, &mempool, &msgpackObject);
	msgpackObject.convert(&serializedGeometry);

	computeMetaByte(serializedGeometry.a0);

	geom->indices(indexBufferParserFunction(serializedGeometry.a2, options->context()));
	
	for (std::string serializedVertexBuffer : serializedGeometry.a3)
		geom->addVertexBuffer(vertexBufferParserFunction(serializedVertexBuffer, options->context()));
		
	assetLibrary->geometry(serializedGeometry.a1, geom);
	_lastParsedAssetName = serializedGeometry.a1;
}

void
GeometryParser::computeMetaByte(unsigned char byte)
{
	if (byte & (1u << 7))
		indexBufferParserFunction	= std::bind(&GeometryParser::deserializeIndexBufferChar, std::placeholders::_1, std::placeholders::_2);
	else
		indexBufferParserFunction = std::bind(&GeometryParser::deserializeIndexBuffer, std::placeholders::_1, std::placeholders::_2);

}