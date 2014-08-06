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
#include "msgpack.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/Options.hpp"

using namespace minko;
using namespace minko::file;

std::unordered_map<uint, std::function<std::shared_ptr<render::IndexBuffer>(std::string&, std::shared_ptr<render::AbstractContext>)>>    GeometryParser::indexBufferParserFunctions;
std::unordered_map<uint, std::function<std::shared_ptr<render::VertexBuffer>(std::string&, std::shared_ptr<render::AbstractContext>)>>    GeometryParser::vertexBufferParserFunctions;

void
GeometryParser::initialize()
{
    registerIndexBufferParserFunction(
        std::bind(&GeometryParser::deserializeIndexBuffer, std::placeholders::_1, std::placeholders::_2),
        0
    );

    registerIndexBufferParserFunction(
        std::bind(&GeometryParser::deserializeIndexBufferChar, std::placeholders::_1, std::placeholders::_2),
        1
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
    msgpack::object                                                        msgpackObject;
    msgpack::zone                                                        mempool;
    msgpack::type::tuple<std::string, std::vector<SerializeAttribute>>    deserializedVertex;

    msgpack::unpack(serializedVertexBuffer.data(), serializedVertexBuffer.size(), NULL, &mempool, &msgpackObject);
    msgpackObject.convert(&deserializedVertex);

    std::vector<float>        vector            = deserialize::TypeDeserializer::deserializeVector<float>(deserializedVertex.a0);
    VertexBufferPtr            vertexBuffer    = render::VertexBuffer::create(context, vector);

    serializedVertexBuffer.clear();
    serializedVertexBuffer.shrink_to_fit();

    uint numAttributes = deserializedVertex.a1.size();

    for (unsigned int attributesIndex = 0; attributesIndex < numAttributes; ++attributesIndex)
        vertexBuffer->addAttribute(
            deserializedVertex.a1[attributesIndex].a0,
            deserializedVertex.a1[attributesIndex].a1,
            deserializedVertex.a1[attributesIndex].a2);

    return vertexBuffer;
}

GeometryParser::IndexBufferPtr
GeometryParser::deserializeIndexBuffer(std::string&                                serializedIndexBuffer,
                                       std::shared_ptr<render::AbstractContext> context)
{
    std::vector<unsigned short> vector = deserialize::TypeDeserializer::deserializeVector<unsigned short>(serializedIndexBuffer);

    serializedIndexBuffer.clear();
    serializedIndexBuffer.shrink_to_fit();

    return render::IndexBuffer::create(context, vector);
}

GeometryParser::IndexBufferPtr
GeometryParser::deserializeIndexBufferChar(std::string&                                serializedIndexBuffer,
                                           std::shared_ptr<render::AbstractContext> context)
{
    std::vector<unsigned short> vector = deserialize::TypeDeserializer::deserializeVector<unsigned short, unsigned char>(serializedIndexBuffer);

    serializedIndexBuffer.clear();
    serializedIndexBuffer.shrink_to_fit();

    return render::IndexBuffer::create(context, vector);
}

void
GeometryParser::parse(const std::string&                filename,
                      const std::string&                resolvedFilename,
                      std::shared_ptr<Options>          options,
                      const std::vector<unsigned char>&    data,
                      std::shared_ptr<AssetLibrary>        assetLibrary)
{
    readHeader(filename, data);

    if (_magicNumber != 0x4D4B0347)
        throw std::logic_error("Invalid geometry data");

    msgpack::object            msgpackObject;
    msgpack::zone            mempool;
    std::string                folderPathName = extractFolderPath(resolvedFilename);
    extractDependencies(assetLibrary, data, _headerSize, _dependenciesSize, options, folderPathName);
    geometry::Geometry::Ptr geom    = geometry::Geometry::create();
    SerializedGeometry        serializedGeometry;

    msgpack::unpack((char*)&data[_headerSize + _dependenciesSize], _sceneDataSize, NULL, &mempool, &msgpackObject);
    msgpackObject.convert(&serializedGeometry);

    std::vector<unsigned char>* d = (std::vector<unsigned char>*)&data;
    d->clear();
    d->shrink_to_fit();

    uint indexBufferFunction = 0;
    uint vertexBufferFunction = 0;

    computeMetaByte(serializedGeometry.a0, indexBufferFunction, vertexBufferFunction);

    geom->indices(indexBufferParserFunctions[indexBufferFunction](serializedGeometry.a2, options->context()));
    serializedGeometry.a2.clear();
    serializedGeometry.a2.shrink_to_fit();

    for (std::string serializedVertexBuffer : serializedGeometry.a3)
    {
        geom->addVertexBuffer(vertexBufferParserFunctions[vertexBufferFunction](serializedVertexBuffer, options->context()));
        serializedVertexBuffer.clear();
        serializedVertexBuffer.shrink_to_fit();
    }

    geom = options->geometryFunction()(serializedGeometry.a1, geom);

    if (options->disposeIndexBufferAfterLoading())
    {
        geom->disposeIndexBufferData();
    }

    if (options->disposeVertexBufferAfterLoading())
    {
        geom->disposeVertexBufferData();
    }

    assetLibrary->geometry(serializedGeometry.a1, geom);
    _lastParsedAssetName = serializedGeometry.a1;
}

void
GeometryParser::computeMetaByte(unsigned char byte, uint& indexBufferFunctionId, uint& vertexBufferFunctionId)
{
    indexBufferFunctionId    = 0x00000000 + ((byte >> 4) & 0x0F);
    vertexBufferFunctionId    = 0x00000000 + (byte & 0x0F);
}
