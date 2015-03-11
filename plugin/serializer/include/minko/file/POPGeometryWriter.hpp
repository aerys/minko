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
#include "minko/StreamingCommon.hpp"
#include "minko/file/AbstractWriter.hpp"

namespace minko
{
    namespace file
    {
        class POPGeometryWriter :
            public AbstractWriter<std::shared_ptr<geometry::Geometry>>
        {
        public:
            typedef std::shared_ptr<POPGeometryWriter>                              Ptr;

            typedef std::tuple<int, int, int>                                       QuantizationIndex;

            typedef std::function<void(
                std::shared_ptr<geometry::Geometry>,
                std::shared_ptr<component::BoundingBox>,
                int&,
                int&)
            >                                                                       RangeFunction;

        private:
            struct LodData
            {
                int                             precisionLevel;

                std::vector<unsigned short>     indices;
                std::vector<std::vector<float>> vertices;
                std::vector<int>                vertexSizes;

                explicit LodData(int precisionLevel) :
                    precisionLevel(precisionLevel),
                    indices(),
                    vertices()
                {
                }
            };

            template <typename T>
            struct hash
            {
                std::size_t
                operator()(const std::tuple<T, T, T>& value) const
                {
                    return std::get<0>(value) ^ std::get<1>(value) ^ std::get<2>(value);
                }
            };

            template <typename T>
            struct equal
            {
                bool
                operator()(const std::tuple<T, T, T>& left, const std::tuple<T, T, T>& right) const
                {
                    return std::get<0>(left) == std::get<0>(right) &&
                           std::get<1>(left) == std::get<1>(right) &&
                           std::get<2>(left) == std::get<2>(right);
                }
            };

            typedef std::unordered_map<QuantizationIndex,
                                       std::vector<unsigned short>,
                                       hash<int>,
                                       equal<int>>                              QuantizationMap;

        public:
            static const int                    _fullPrecisionLevel;

        private:
            static const bool                   _keepSplitVertexPattern;

            static const int                    _defaultMinPrecisionLevel;
            static const int                    _defaultMaxPrecisionLevel;

            static const int                    _smallFeatureTriangleCountThreshold;

            std::shared_ptr<file::AssetLibrary> _assetLibrary;
            std::shared_ptr<file::Options>      _options;
            std::shared_ptr<StreamingOptions>   _streamingOptions;

            std::shared_ptr<geometry::Geometry> _geometry;

            std::shared_ptr<LinkedAsset>        _linkedAsset;
            int                                 _linkedAssetId;

            int                                 _minLevel;
            int                                 _maxLevel;

            math::vec3                          _minBound;
            math::vec3                          _maxBound;

            RangeFunction                       _rangeFunction;

        public:
            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new POPGeometryWriter());

                return instance;
            }

            inline
            void
            streamingOptions(std::shared_ptr<StreamingOptions> value)
            {
                _streamingOptions = value;
            }

            inline
            void
            linkedAsset(std::shared_ptr<LinkedAsset> linkedAsset, int linkedAssetId)
            {
                _linkedAsset = linkedAsset;

                _linkedAssetId = linkedAssetId;
            }

            std::string
            embed(std::shared_ptr<AssetLibrary>     assetLibrary,
                  std::shared_ptr<Options>          options,
                  std::shared_ptr<Dependency>       dependency,
                  std::shared_ptr<WriterOptions>    writerOptions,
                  std::vector<unsigned char>&       embeddedHeaderData);

        private:
            POPGeometryWriter();

            static
            QuantizationIndex
            quantify(const math::vec3&  position,
                     int                level,
                     int                maxLevel,
                     const math::vec3&  minBound,
                     const math::vec3&  maxBound,
                     const math::vec3&  boxSize,
                     bool               outputQuantizedPosition,
                     math::vec3&        quantizedPosition);

            static
            void
            defaultRangeFunction(std::shared_ptr<geometry::Geometry>        geometry,
                                 std::shared_ptr<component::BoundingBox>    boundingBox,
                                 int&                                       minLevel,
                                 int&                                       maxLevel);

            void
            buildLodData(std::map<int, LodData>& lodData,
                         int                     minLevel,
                         int                     maxLevel);

            void
            serializeGeometry(std::shared_ptr<Dependency>       dependency,
                              std::shared_ptr<WriterOptions>    writerOptions,
                              const std::map<int, LodData>&     lodData,
                              std::vector<unsigned char>&       headerData,
                              std::vector<unsigned char>&       blobData);

            void
            serializeHeader(msgpack::sbuffer&                   headerBuffer,
                            msgpack::sbuffer&                   blobBuffer,
                            const std::map<int, LodData>&       lodData);

            void
            serializeLod(std::vector<msgpack::type::tuple<int, int, int, int, int, int>>& lodHeaders,
                         msgpack::sbuffer&                                                blobBuffer,
                         int                                                              level,
                         const LodData&                                                   lod);

            void
            updateBoundaryLevels(const std::map<int, LodData>& lodData);
        };

        inline
        POPGeometryWriter::QuantizationIndex
        operator+(const POPGeometryWriter::QuantizationIndex& left,
                  const POPGeometryWriter::QuantizationIndex& right)
        {
            return POPGeometryWriter::QuantizationIndex(
                std::get<0>(left) + std::get<0>(right),                                                                             
                std::get<1>(left) + std::get<1>(right),                                                                
                std::get<2>(left) + std::get<2>(right)
            );
        }
    }
}
