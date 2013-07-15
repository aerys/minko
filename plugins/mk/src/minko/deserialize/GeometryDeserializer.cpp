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

#include "GeometryDeserializer.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/scene/Node.hpp"
#include "minko/controller/Surface.hpp"

using namespace minko;
using namespace minko::deserialize;

std::map<int, std::string>						GeometryDeserializer::_geometryIdToName;
std::map<int, GeometryDeserializer::NodeList>	GeometryDeserializer::_waitForGeometryNodes;

template <typename T>
void
GeometryDeserializer::read(std::stringstream& stream, T& value)
{
	stream.read(reinterpret_cast<char*>(&value), sizeof (T));
}

void
GeometryDeserializer::deserializeGeometry(bool								isCopy,
										  std::string						geometryName,
										  int								copyId,
										  Qark::ByteArray&					geometryData,
										  std::shared_ptr<AssetsLibrary>	library,
										  std::shared_ptr<scene::Node>		mesh,
										  std::shared_ptr<file::Options>	options)
{
	if (isCopy)
	{
		std::map<int, std::string>&						gItTiNameCopy = _geometryIdToName;
		std::map<int, GeometryDeserializer::NodeList>&	_waitGorGeometryNodeCopye = _waitForGeometryNodes;
		
		if (_geometryIdToName.find(copyId) != _geometryIdToName.end())
			mesh->controller<controller::Surface>()->geometry(library->geometry(_geometryIdToName[copyId]));
		else
			_waitForGeometryNodes[copyId].push_back(mesh);
	}
	else
	{
		std::stringstream stream;
		stream.write(&*geometryData.begin(), geometryData.size());

		unsigned char meshType = 0;

		read(stream, meshType);

		std::shared_ptr<render::IndexBuffer> indexStream = readIndexStream(stream, options);
		std::shared_ptr<render::VertexBuffer> vertexStream = readVertexStream(stream, options);

		std::shared_ptr<geometry::Geometry> geometry = geometry::Geometry::create();

		geometry->indices(indexStream);
		geometry->addVertexBuffer(vertexStream);

		mesh->controller<controller::Surface>()->geometry(geometry);

		GeometryDeserializer::_geometryIdToName[copyId] = geometryName;
		library->geometry(geometryName, geometry);

		if (_waitForGeometryNodes.find(copyId) != _waitForGeometryNodes.end())
		{
			for (unsigned int meshId = 0; meshId < _waitForGeometryNodes[copyId].size(); ++meshId)
			{
				std::shared_ptr<scene::Node> m = _waitForGeometryNodes[copyId][meshId];

				mesh->controller<controller::Surface>()->geometry(geometry);
			}
		}

		stream.flush();
	}
}

std::shared_ptr<render::IndexBuffer>
GeometryDeserializer::readIndexStream(std::stringstream&				stream,
									  std::shared_ptr<file::Options>	options)
{
	unsigned int				numIndices = 0;
	std::vector<unsigned short> data;

	read(stream, numIndices);

	data.resize(numIndices);

	stream.read(reinterpret_cast<char*>(&*data.begin()), numIndices * 2);

	return render::IndexBuffer::create(options->context(), data);
}

std::shared_ptr<render::VertexBuffer>
GeometryDeserializer::readVertexStream(std::stringstream&				stream,
									   std::shared_ptr<file::Options>	options)
{
	unsigned short		numVertices				= 0;
	unsigned int		vertexSize				= 0;
	bool				lossy					= false;
	unsigned int		numComponentsPerVertex	= 0;
	std::vector<float>	data;

	read(stream, numVertices);
	read(stream, lossy);

	std::vector<render::VertexBuffer::AttributePtr> vertexAttributes = readVertexFormat(stream, options);

	for (unsigned int attributeId = 0; attributeId < vertexAttributes.size(); ++attributeId)
		vertexSize += std::get<1>(*vertexAttributes[attributeId]);

	numComponentsPerVertex = numVertices * vertexSize;

	data.resize(numComponentsPerVertex);

	stream.read(reinterpret_cast<char*>(&*data.begin()), numComponentsPerVertex * sizeof(float));

	std::shared_ptr<render::VertexBuffer> vstream = render::VertexBuffer::create(
		options->context(), data/*std::begin(data), std::end(data)*/
	);
	
	for (unsigned int attributeId = 0; attributeId < vertexAttributes.size(); ++attributeId)
		vstream->addAttribute(
		std::get<0>(*vertexAttributes[attributeId]), 
		std::get<1>(*vertexAttributes[attributeId]), 
		std::get<2>(*vertexAttributes[attributeId]));

	return vstream;
}

std::vector<render::VertexBuffer::AttributePtr>
GeometryDeserializer::readVertexFormat(std::stringstream&				stream,
									   std::shared_ptr<file::Options>	options)
{
	std::vector<render::VertexBuffer::AttributePtr> attributes;

	unsigned char numComponents = 0;
	unsigned int  offset		= 0;

	read(stream, numComponents);

	while (numComponents > 0)
	{
		unsigned char				numProperties	= 0;
		int							nativeFormat	= 0;
		std::vector<std::string>	properties;

		read(stream, numProperties);
		read(stream, nativeFormat);

		for (unsigned char propertyId = 0; propertyId < numProperties; ++propertyId)
		{
			unsigned short	utfLength = 0;
			std::string		propertyName;

			read(stream, utfLength);

			char* data = new char[utfLength];
			stream.read(data, utfLength);

			propertyName.assign(data, utfLength);
			properties.push_back(propertyName);
		}
		
		unsigned int numPropertiesInt = numProperties;

		if (properties[0] == "x")
			attributes.push_back(render::VertexBuffer::AttributePtr(new render::VertexBuffer::Attribute("position", numPropertiesInt, offset)));
		else if (properties[0] == "nx")
			attributes.push_back(render::VertexBuffer::AttributePtr(new render::VertexBuffer::Attribute("normal", numPropertiesInt, offset)));
		else if (properties[0] == "u")
			attributes.push_back(render::VertexBuffer::AttributePtr(new render::VertexBuffer::Attribute("uv", numPropertiesInt, offset)));
		else 
			attributes.push_back(render::VertexBuffer::AttributePtr(new render::VertexBuffer::Attribute(properties[0], numPropertiesInt, offset)));

		offset += numProperties;
		--numComponents;
	}

	return attributes;
}