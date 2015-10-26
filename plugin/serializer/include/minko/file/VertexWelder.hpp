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

#include "minko/file/AbstractWriterPreprocessor.hpp"

namespace minko
{
    namespace file
    {
        class VertexWelder :
            public AbstractWriterPreprocessor<std::shared_ptr<scene::Node>>
        {
        public:
            typedef std::shared_ptr<VertexWelder>           Ptr;

            template <typename T>
            using VertexAttributePredicateFunction = std::function<bool(const std::string&,
                                                                        const T&,
                                                                        const T&)>;

        private:
            typedef std::shared_ptr<scene::Node>        NodePtr;
            typedef std::shared_ptr<AssetLibrary>       AssetLibraryPtr;

            typedef std::shared_ptr<component::Surface> SurfacePtr;

            typedef std::shared_ptr<geometry::Geometry> GeometryPtr;

            struct SpatialIndexHash;
            struct SpatialIndexEqual;

            typedef std::unordered_map<
                math::vec3,
                std::vector<unsigned int>,
                SpatialIndexHash,
                SpatialIndexEqual
            >                                           SpatialIndex;

            struct SpatialIndexHash
            {
                float epsilon;

                std::size_t
                operator()(const math::vec3& position) const;

                explicit
                SpatialIndexHash(float epsilon) :
                    epsilon(epsilon)
                {
                }
            };

            struct SpatialIndexEqual
            {
                float epsilon;

                bool
                operator()(const math::vec3& lhs, const math::vec3& rhs) const;

                explicit
                SpatialIndexEqual(float epsilon) :
                    epsilon(epsilon)
                {
                }
            };

        private:
            StatusChangedSignal::Ptr                        _statusChanged;
            float                                           _progressRate;

            VertexAttributePredicateFunction<float>         _scalarAttributeWeldablePredicateFunction;
            VertexAttributePredicateFunction<math::vec2>    _vec2AttributeWeldablePredicateFunction;
            VertexAttributePredicateFunction<math::vec3>    _vec3AttributeWeldablePredicateFunction;
            VertexAttributePredicateFunction<math::vec4>    _vec4AttributeWeldablePredicateFunction;

            std::unordered_set<GeometryPtr>                 _weldedGeometrySet;

            static const float                              _defaultEpsilon;

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
            float
            progressRate() const
            {
                return _progressRate;
            }

            inline
            StatusChangedSignal::Ptr
            statusChanged()
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
            weldSurfaceGeometry(SurfacePtr surface);

            void
            buildSpatialIndex(GeometryPtr geometry, SpatialIndex& index);

            bool
            canWeldVertices(GeometryPtr                         geometry,
                            const std::vector<unsigned int>&    indices);

            math::vec3
            weldVec3VertexAttribute(const render::VertexAttribute&      attribute,
                                    const std::vector<float>&           data,
                                    const std::vector<unsigned int>&    indices);
        };
    }
}
