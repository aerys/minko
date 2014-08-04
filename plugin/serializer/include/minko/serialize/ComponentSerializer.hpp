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

#include "minko/SerializerCommon.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "msgpack.hpp"

namespace minko
{
    namespace serialize
    {
        class ComponentSerializer
        {
        public:
            typedef std::shared_ptr<scene::Node>                        NodePtr;
            typedef std::shared_ptr<component::Surface>                    SurfacePtr;
            typedef std::shared_ptr<file::Dependency>                    DependencyPtr;
            typedef msgpack::type::tuple<std::string, std::string>        SimpleProperty;
            typedef msgpack::type::tuple<std::vector<SimpleProperty>>    SimplePropertyVector;

        public:
            static
            std::string
            serializeTransform(NodePtr            node,
                               DependencyPtr    dependencies);

            static
            std::string
            serializePerspectiveCamera(NodePtr            node,
                                       DependencyPtr    dependencies);

            static
            std::string
            serializeAmbientLight(NodePtr        node,
                                  DependencyPtr dependencies);

            static
            std::string
            serializeDirectionalLight(NodePtr        node,
                                      DependencyPtr dependencies);

            static
            std::string
            serializePointLight(NodePtr            node,
                                DependencyPtr    dependencies);

            static
            std::string
            serializeSpotLight(NodePtr            node,
                               DependencyPtr    dependencies);

            static
            std::string
            serializeSurface(NodePtr        node,
                             DependencyPtr    dependencies);

            static
            std::string
            serializeRenderer(NodePtr        node,
                              DependencyPtr dependencies);

            static
            std::string
            serializeBoundingBox(NodePtr         node,
                                 DependencyPtr     dependencies);

            static
            std::string
            getSurfaceExtension(NodePtr, SurfacePtr);


            static
            SimpleProperty
            serializeSimpleProperty(std::string propertyName, int value)
            {
                std::string serializedValue = TypeSerializer::serializeVector(std::vector<int>(value));

                return SimpleProperty(propertyName, serializedValue);
            }

            static
            SimpleProperty
            serializeSimpleProperty(std::string propertyName, bool value)
            {
                std::string serializedValue = TypeSerializer::serializeVector(std::vector<float>(value ? 1.0 : 0.0));

                return SimpleProperty(propertyName, serializedValue);
            }

            static
            SimpleProperty
            serializeSimpleProperty(std::string propertyName, float value)
            {
                std::string serializedValue = TypeSerializer::serializeVector(std::vector<float>(value));

                return SimpleProperty(propertyName, serializedValue);
            }

            static
            SimpleProperty
            serializeSimpleProperty(std::string propertyName, std::string value)
            {
                return SimpleProperty(propertyName, value);
            }
        };
    }
}
