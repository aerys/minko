/*
Copyright (c) 2014 Aerys

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
#include "minko/geometry/Geometry.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/deserialize/Unpacker.hpp"
#include "minko/file/Options.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::deserialize;

std::unordered_map<uint, std::function<std::shared_ptr<render::IndexBuffer>(std::string&, std::shared_ptr<render::AbstractContext>)>>    GeometryParser::indexBufferParserFunctions;
std::unordered_map<uint, std::function<std::shared_ptr<render::VertexBuffer>(std::string&, std::shared_ptr<render::AbstractContext>)>>    GeometryParser::vertexBufferParserFunctions;

void
GeometryParser::initialize()
{
    // Default 16-bit index.
    registerIndexBufferParserFunction(
        [](std::string&                             serializedindexBuffer,
           std::shared_ptr<render::AbstractContext> context)
        {
            return deserializeIndexBuffer<unsigned short>(serializedindexBuffer, context);
        },
        0
    );

    // 8-bit index.
    registerIndexBufferParserFunction(
        std::bind(&GeometryParser::deserializeIndexBufferChar, std::placeholders::_1, std::placeholders::_2),
        1
    );

    // 32-bit index.
    registerIndexBufferParserFunction(
        [](std::string&                             serializedindexBuffer,
           std::shared_ptr<render::AbstractContext> context)
        {
            return deserializeIndexBuffer<unsigned int>(serializedindexBuffer, context);
        },
        2
    );

    registerVertexBufferParserFunction(
        std::bind(&GeometryParser::deserializeVertexBuffer, std::placeholders::_1, std::placeholders::_2),
        0
    );
}

std::shared_ptr<render::VertexBuffer>
GeometryParser::deserializeVertexBuffer(std::string&                                serializedVertexBuffer,
                                        std::shared_ptr<render::AbstractContext>    context)
{
	msgpack::type::tuple<std::string, std::vector<SerializeAttribute>>	deserializedVertex;

    unpack(deserializedVertex, serializedVertexBuffer.data(), serializedVertexBuffer.size());

	std::vector<float>		vector			= deserialize::TypeDeserializer::deserializeVector<float>(deserializedVertex.get<0>());
	VertexBufferPtr			vertexBuffer	= render::VertexBuffer::create(context, vector);

	uint numAttributes = deserializedVertex.get<1>().size();

	for (unsigned int attributesIndex = 0; attributesIndex < numAttributes; ++attributesIndex)
		vertexBuffer->addAttribute(
			deserializedVertex.get<1>()[attributesIndex].get<0>(),
			deserializedVertex.get<1>()[attributesIndex].get<1>(),
			deserializedVertex.get<1>()[attributesIndex].get<2>());

	return vertexBuffer;
}

GeometryParser::IndexBufferPtr
GeometryParser::deserializeIndexBufferChar(std::string&                             serializedIndexBuffer,
                                           std::shared_ptr<render::AbstractContext> context)
{
    std::vector<unsigned short> vector = deserialize::TypeDeserializer::deserializeVector<unsigned short, unsigned char>(serializedIndexBuffer);

    return render::IndexBuffer::create(context, vector);
}

void
GeometryParser::parse(const std::string&                filename,
                      const std::string&                resolvedFilename,
                      std::shared_ptr<Options>          options,
                      const std::vector<unsigned char>& data,
                      std::shared_ptr<AssetLibrary>     assetLibrary)
{
    if (!readHeader(filename, data, 0x47))
        return;

	std::string				folderPathName = extractFolderPath(resolvedFilename);
	geometry::Geometry::Ptr geom	= geometry::Geometry::create(filename);
    SerializedGeometry          serializedGeometry;

    extractDependencies(assetLibrary, data, _headerSize, _dependencySize, options, folderPathName);

    unpack(serializedGeometry, data, _sceneDataSize, _headerSize + _dependencySize);

    uint indexBufferFunction = 0;
    uint vertexBufferFunction = 0;

	computeMetaData(serializedGeometry.get<0>(), indexBufferFunction, vertexBufferFunction);

    geom->indices(indexBufferParserFunctions[indexBufferFunction](serializedGeometry.get<2>(), options->context()));

    for (std::string serializedVertexBuffer : serializedGeometry.get<3>())
    {
        geom->addVertexBuffer(vertexBufferParserFunctions[vertexBufferFunction](serializedVertexBuffer, options->context()));
	}

    geom = options->geometryFunction()(serializedGeometry.get<1>(), geom);

    if (options->disposeIndexBufferAfterLoading())
    {
        geom->disposeIndexBufferData();
    }

    if (options->disposeVertexBufferAfterLoading())
    {
        geom->disposeVertexBufferData();
    }

    static auto nameId = 0;
    auto uniqueName = serializedGeometry.get<1>();

    while (assetLibrary->geometry(uniqueName) != nullptr)
        uniqueName = "geometry" + std::to_string(nameId++);

	assetLibrary->geometry(uniqueName, geom);
	_lastParsedAssetName = uniqueName;

    complete()->execute(shared_from_this());
}

void
GeometryParser::computeMetaData(unsigned short metaData, uint& indexBufferFunctionId, uint& vertexBufferFunctionId)
{
	indexBufferFunctionId	= 0x00000000 + ((metaData >> 4) & 0x0F);
	vertexBufferFunctionId	= 0x00000000 + (metaData & 0x0F);
}
