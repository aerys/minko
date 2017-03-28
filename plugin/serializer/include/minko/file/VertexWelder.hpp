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
#include "minko/file/AbstractWriterPreprocessor.hpp"
#include "minko/render/VertexAttribute.hpp"

namespace minko
{
    namespace file
    {
        class VertexWelder :
            public AbstractWriterPreprocessor<std::shared_ptr<scene::Node>>
        {
        public:
            typedef std::shared_ptr<VertexWelder>           Ptr;

            typedef std::shared_ptr<scene::Node>            NodePtr;

            typedef std::function<bool(NodePtr)>            NodePredicateFunction;

            template <typename T>
            using VertexAttributePredicateFunction = std::function<bool(const std::string&,
                                                                        const T&,
                                                                        const T&)>;

        private:
            typedef std::shared_ptr<AssetLibrary>           AssetLibraryPtr;
            typedef std::shared_ptr<render::IndexBuffer>    IndexBufferPtr;
            typedef std::shared_ptr<component::Surface>     SurfacePtr;
            typedef std::shared_ptr<geometry::Geometry>     GeometryPtr;

        private:
            StatusChangedSignal::Ptr                        _statusChanged;
            float                                           _progressRate;

            NodePredicateFunction                           _nodePredicateFunction;

            VertexAttributePredicateFunction<float>         _scalarAttributeWeldablePredicateFunction;
            VertexAttributePredicateFunction<math::vec2>    _vec2AttributeWeldablePredicateFunction;
            VertexAttributePredicateFunction<math::vec3>    _vec3AttributeWeldablePredicateFunction;
            VertexAttributePredicateFunction<math::vec4>    _vec4AttributeWeldablePredicateFunction;

            std::unordered_set<GeometryPtr>                 _weldedGeometrySet;

        public:
            ~VertexWelder() = default;

            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new VertexWelder());

                return instance;
            }

            inline
            const NodePredicateFunction&
            nodePredicateFunction() const
            {
                return _nodePredicateFunction;
            }

            inline
            Ptr
            nodePredicateFunction(const NodePredicateFunction& func)
            {
                _nodePredicateFunction = func;

                return std::static_pointer_cast<VertexWelder>(shared_from_this());
            }

            inline
            const VertexAttributePredicateFunction<float>&
            scalarAttributeWeldablePredicateFunction() const
            {
                return _scalarAttributeWeldablePredicateFunction;
            }

            inline
            Ptr
            scalarAttributeWeldablePredicateFunction(const VertexAttributePredicateFunction<float>& func)
            {
                _scalarAttributeWeldablePredicateFunction = func;

                return std::static_pointer_cast<VertexWelder>(shared_from_this());
            }

            inline
            const VertexAttributePredicateFunction<math::vec2>&
            vec2AttributeWeldablePredicateFunction() const
            {
                return _vec2AttributeWeldablePredicateFunction;
            }

            inline
            Ptr
            vec2AttributeWeldablePredicateFunction(const VertexAttributePredicateFunction<math::vec2>& func)
            {
                _vec2AttributeWeldablePredicateFunction = func;

                return std::static_pointer_cast<VertexWelder>(shared_from_this());
            }

            inline
            const VertexAttributePredicateFunction<math::vec3>&
            vec3AttributeWeldablePredicateFunction() const
            {
                return _vec3AttributeWeldablePredicateFunction;
            }

            inline
            Ptr
            vec3AttributeWeldablePredicateFunction(const VertexAttributePredicateFunction<math::vec3>& func)
            {
                _vec3AttributeWeldablePredicateFunction = func;

                return std::static_pointer_cast<VertexWelder>(shared_from_this());
            }

            inline
            const VertexAttributePredicateFunction<math::vec4>&
            vec4AttributeWeldablePredicateFunction() const
            {
                return _vec4AttributeWeldablePredicateFunction;
            }

            inline
            Ptr
            vec4AttributeWeldablePredicateFunction(const VertexAttributePredicateFunction<math::vec4>& func)
            {
                _vec4AttributeWeldablePredicateFunction = func;

                return std::static_pointer_cast<VertexWelder>(shared_from_this());
            }

            inline
            float
            progressRate() const override
            {
                return _progressRate;
            }

            inline
            StatusChangedSignal::Ptr
            statusChanged() override
            {
                return _statusChanged;
            }

            void
            process(NodePtr& node, AssetLibraryPtr assetLibrary) override;

        private:
            VertexWelder();

            bool
            acceptsSurface(SurfacePtr surface);

            void
            weldSurfaceGeometry(SurfacePtr surface, AssetLibraryPtr assetLibrary);

            void
            buildSpatialIndex(GeometryPtr                                                       geometry,
                              std::shared_ptr<math::SpatialIndex<std::vector<unsigned int>>>    index);

            bool
            canWeldVertices(GeometryPtr                         geometry,
                            const std::vector<unsigned int>&    indices);

            template <typename T>
            T
            weldAttribute(const render::VertexAttribute&        attribute,
                          const std::vector<float>&             data,
                          const std::vector<unsigned int>&      indices,
                          std::function<T(const float* const)>  makeVec)
            {
                auto values = std::vector<T>(indices.size());

                for (auto i = 0u; i < indices.size(); ++i)
                    values[i] = makeVec(&data.at(indices.at(i) * *attribute.vertexSize + attribute.offset));

                auto result = values.front();

                for (auto i = 1; i < values.size(); ++i)
                    result += values.at(i);

                result /= float(values.size());

                return result;
            }

            template <typename T>
            bool
            canWeldAttribute(const render::VertexAttribute&                 attribute,
                             const std::vector<float>&                      data,
                             const std::pair<unsigned int, unsigned int>&   indices,
                             std::function<T(const float* const)>           makeVec,
                             VertexAttributePredicateFunction<T>            predicate)
            {
                const auto lhsValue = makeVec(&data.at(
                    indices.first * *attribute.vertexSize + attribute.offset
                ));

                const auto rhsValue = makeVec(&data.at(
                    indices.second * *attribute.vertexSize + attribute.offset
                ));

                if (predicate && !predicate(*attribute.name, lhsValue, rhsValue))
                    return false;

                return true;
            }

            template <typename T>
            IndexBufferPtr
            createIndexBuffer(const std::vector<unsigned int>&  indices,
                              unsigned int                      newNumIndices,
                              unsigned int                      primitiveSize,
                              const std::vector<int>&           indexMap,
                              AssetLibraryPtr                   assetLibrary);
        };
    }
}
