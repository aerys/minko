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

std::function<std::string(std::shared_ptr<render::IndexBuffer>)>	GeometryWriter::indexBufferWriterFunction;
std::function<std::string(std::shared_ptr<render::VertexBuffer>)>	GeometryWriter::vertexBufferWriterFunction;

void
GeometryWriter::initialize()
{
	indexBufferWriterFunction	= std::bind(&GeometryWriter::serializeIndexStream, std::placeholders::_1);
	vertexBufferWriterFunction	= std::bind(&GeometryWriter::serializeVertexStream, std::placeholders::_1);
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
	std::list<render::VertexBuffer::AttributePtr>									attributes			= vertexBuffer->attributes();
	std::vector<msgpack::type::tuple<std::string, unsigned char, unsigned char>>	serializedAttributes;
	
	auto attributesIt = attributes.begin();
	
	while (attributesIt != attributes.end())
	{
		serializedAttributes.push_back(msgpack::type::tuple<std::string, unsigned char, unsigned char>(
			std::get<0>((**attributesIt)), 
			std::get<1>((**attributesIt)), 
			std::get<2>((**attributesIt))));

		attributesIt++;
	}

	std::string serializedVector = serialize::TypeSerializer::serializeVector<float>(vertexBuffer->data());

	std::stringstream			sbuf;
	msgpack::type::tuple<std::string, std::vector<msgpack::type::tuple<std::string, unsigned char, unsigned char>>> res(
		serializedVector,
		serializedAttributes);

	msgpack::pack(sbuf, res);

	return sbuf.str();
}

unsigned char
GeometryWriter::computeMetaByte(std::shared_ptr<geometry::Geometry> geometry)
{
	std::vector<unsigned short>::iterator maxIndice = std::max_element(geometry->indices()->data().begin(), geometry->indices()->data().end());

	unsigned char metaByte = 0x00;
	
	if (*maxIndice <= 255)
	{
		metaByte += 1u << 7;
		indexBufferWriterFunction	= std::bind(&GeometryWriter::serializeIndexStreamChar, std::placeholders::_1);
	}
	else
		indexBufferWriterFunction	= std::bind(&GeometryWriter::serializeIndexStream, std::placeholders::_1);

	return metaByte;
}