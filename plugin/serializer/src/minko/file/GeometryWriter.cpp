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

#include "minko/file/GeometryWriter.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "minko/render/IndexBuffer.hpp"

using namespace minko;
using namespace minko::file;

std::unordered_map<uint, std::function<std::string(std::shared_ptr<render::IndexBuffer>)>>	GeometryWriter::indexBufferWriterFunctions;
std::unordered_map<uint, std::function<std::string(std::shared_ptr<render::VertexBuffer>)>>	GeometryWriter::vertexBufferWriterFunctions;

std::unordered_map<uint, GeometryWriter::GeometryTestFunc>		GeometryWriter::indexBufferTestFunctions;
std::unordered_map<uint, GeometryWriter::GeometryTestFunc>		GeometryWriter::vertexBufferTestFunctions;

void
GeometryWriter::initialize()
{
	_magicNumber = 0x00000047 | MINKO_SCENE_MAGIC_NUMBER;

	registerIndexBufferWriterFunction(
		std::bind(
			GeometryWriter::serializeIndexStream,
			std::placeholders::_1),
		[=](std::shared_ptr < geometry::Geometry> geometry){return true; },
		0
	);

	registerIndexBufferWriterFunction(
		std::bind(
			GeometryWriter::serializeIndexStreamChar,
			std::placeholders::_1
			),
		std::bind(
			GeometryWriter::indexBufferFitCharCompression,
			std::placeholders::_1
		),
		1
	);

	registerVertexBufferWriterFunction(
		std::bind(
			GeometryWriter::serializeVertexStream,
			std::placeholders::_1
		),
		[=](std::shared_ptr < geometry::Geometry> geometry){return true; },
		0
	);
}

std::string
GeometryWriter::embed(std::shared_ptr<AssetLibrary>		assetLibrary,
					  std::shared_ptr<Options>			options,
					  Dependency::Ptr					dependency,
					  WriterOptions::Ptr				writerOptions)
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

std::string
GeometryWriter::serializeIndexStream(std::shared_ptr<render::IndexBuffer> indexBuffer)
{
	return serialize::TypeSerializer::serializeVector<unsigned short>(indexBuffer->data());
}

std::string
GeometryWriter::serializeIndexStreamChar(std::shared_ptr<render::IndexBuffer> indexBuffer)
{
	return serialize::TypeSerializer::serializeVector<unsigned short, unsigned char>(indexBuffer->data());
}

std::string
GeometryWriter::serializeVertexStream(std::shared_ptr<render::VertexBuffer> vertexBuffer)
{
	std::vector<msgpack::type::tuple<std::string, unsigned char, unsigned char>> serializedAttributes;
	
    for (const auto& attribute : vertexBuffer->attributes())
	{
		serializedAttributes.push_back(msgpack::type::tuple<std::string, unsigned char, unsigned char>(
            attribute.name,
            attribute.size,
            attribute.offset
        ));
	}

	std::string serializedVector = serialize::TypeSerializer::serializeVector<float>(vertexBuffer->data());

	std::stringstream sbuf;
	msgpack::type::tuple<std::string, std::vector<msgpack::type::tuple<std::string, unsigned char, unsigned char>>> res(
		serializedVector,
		serializedAttributes
    );

	msgpack::pack(sbuf, res);

	return sbuf.str();
}

unsigned char
GeometryWriter::computeMetaByte(std::shared_ptr<geometry::Geometry> geometry, 
							    uint&								indexBufferFunctionId, 
								uint&								vertexBufferFunctionId,
								WriterOptionsPtr					writerOptions)
{
	unsigned char metaByte = 0x00;
	
	for (auto functionIdTestFunc : indexBufferTestFunctions)
		if (functionIdTestFunc.second(geometry) && functionIdTestFunc.first >= indexBufferFunctionId)
			indexBufferFunctionId = functionIdTestFunc.first;

	for (auto functionIdTestFunc : vertexBufferTestFunctions)
		if (functionIdTestFunc.second(geometry) && functionIdTestFunc.first >= vertexBufferFunctionId)
			vertexBufferFunctionId = functionIdTestFunc.first;

	metaByte = ((indexBufferFunctionId << 4) & 0xF0) + (vertexBufferFunctionId & 0x0F);

	return metaByte;
}

bool
GeometryWriter::indexBufferFitCharCompression(std::shared_ptr<geometry::Geometry> geometry)
{
	std::vector<unsigned short>::iterator maxIndice = std::max_element(geometry->indices()->data().begin(), geometry->indices()->data().end());

	return (*maxIndice <= 255);

}