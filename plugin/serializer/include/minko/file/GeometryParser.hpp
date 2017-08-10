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

#pragma once

#include "minko/Common.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/AbstractSerializerParser.hpp"
#include "minko/render/IndexBuffer.hpp"

namespace minko
{
    namespace file
    {
        class GeometryParser:
            public AbstractSerializerParser
        {
        public:
            typedef std::shared_ptr<GeometryParser>             Ptr;
            typedef std::shared_ptr<render::AbstractContext>    AbstractContextPtr;
            typedef std::shared_ptr<render::IndexBuffer>        IndexBufferPtr;
            typedef std::shared_ptr<render::VertexBuffer>       VertexBufferPtr;

        private:
            typedef unsigned char                                                                    uchar;
            typedef msgpack::type::tuple<std::string, uchar, uchar>                                  SerializeAttribute;
            typedef msgpack::type::tuple<uchar, std::string, std::string, std::vector<std::string>>  SerializedGeometry;

        private:
            static std::unordered_map<uint, std::function<IndexBufferPtr(std::string&, AbstractContextPtr)>>    indexBufferParserFunctions;
            static std::unordered_map<uint, std::function<VertexBufferPtr(std::string&, AbstractContextPtr)>>   vertexBufferParserFunctions;

        public:
            inline static
            Ptr
            create()
            {
                return std::shared_ptr<GeometryParser>(new GeometryParser());
            }

            void
            parse(const std::string&                filename,
                  const std::string&                resolvedFilename,
                  std::shared_ptr<Options>          options,
                  const std::vector<unsigned char>& data,
                  std::shared_ptr<AssetLibrary>     assetLibrary);

            inline
            static
            void
            registerIndexBufferParserFunction(std::function<IndexBufferPtr(std::string&, AbstractContextPtr)> f, uint functionId)
            {
                indexBufferParserFunctions[functionId] = f;
            }

            inline
            static
            void
            registerVertexBufferParserFunction(std::function<VertexBufferPtr(std::string&, AbstractContextPtr)> f, uint functionId)
            {
                vertexBufferParserFunctions[functionId] = f;
            }

            template <typename T>
            static
            IndexBufferPtr
            deserializeIndexBuffer(std::string&          serializedIndexBuffer,
                                   AbstractContextPtr    context)
            {
                auto vector = deserialize::TypeDeserializer::deserializeVector<T>(serializedIndexBuffer);

                return render::IndexBuffer::create(context, vector);
            }

        private:
            GeometryParser()
            {
                initialize();
            }

            void
            computeMetaData(unsigned short metaData, uint& indexBufferFunctionId, uint& vertexBufferFunctionId);

            void
            initialize();

            static
            VertexBufferPtr
            deserializeVertexBuffer(std::string&        serializedVertexBuffer,
                                    AbstractContextPtr  context);


            static
            IndexBufferPtr
            deserializeIndexBufferChar(std::string&          serializedIndexBuffer,
                                       AbstractContextPtr    context);

        };
    }
}
