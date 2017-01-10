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
#include "minko/component/JobManager.hpp"
#include "minko/file/AbstractStreamedAssetParser.hpp"

namespace minko
{
    namespace file
    {
        class POPGeometryParser :
            public AbstractStreamedAssetParser
        {
        public:
            typedef std::shared_ptr<POPGeometryParser> Ptr;

        private:
            struct LodInfo
            {
                int     level;
                int     precisionLevel;

                int     indexCount;
                int     vertexCount;

                int     blobOffset;
                int     blobSize;

                bool    isRead;

                LodInfo(int level,
                        int precisionLevel,
                        int indexCount,
                        int vertexCount,
                        int blobOffset,
                        int blobSize) :
                    level(level),
                    precisionLevel(precisionLevel),
                    indexCount(indexCount),
                    vertexCount(vertexCount),
                    blobOffset(blobOffset),
                    blobSize(blobSize),
                    isRead(false)
                {
                }
            };

        private:
            int                                                                         _lodCount;
            int                                                                         _minLod;
            int                                                                         _maxLod;
            int                                                                         _fullPrecisionLod;

            math::vec3                                                                  _minBound;
            math::vec3                                                                  _maxBound;

            bool                                                                        _isSharedPartition;
            int                                                                         _minBorderPrecision;
            int                                                                         _maxDeltaBorderPrecision;

            int                                                                         _vertexSize;
            int                                                                         _numVertexBuffers;
            std::vector<msgpack::type::tuple<
                unsigned int,
                std::string,
                unsigned int,
                unsigned int
            >>                                                                          _vertexAttributes;

            std::map<int, LodInfo>                                                      _lods;

            int                                                                         _geometryIndexOffset;
            int                                                                         _geometryVertexOffset;

            std::shared_ptr<geometry::Geometry>                                         _geometry;

            std::size_t                                                                 _lodRequestNumPrimitivesLoaded;

        public:
            inline
            static
            Ptr
            create()
            {
                return Ptr(new POPGeometryParser());
            }

            std::size_t
            lodRequestNumPrimitivesLoaded() override;

        protected:
            bool
            useDescriptor(const std::string&                filename,
                          std::shared_ptr<Options>          options,
                          const std::vector<unsigned char>& data,
                          std::shared_ptr<AssetLibrary>     assetLibrary) override;

            void
            parsed(const std::string&                filename,
                   const std::string&                resolvedFilename,
                   std::shared_ptr<Options>          options,
                   const std::vector<unsigned char>& data,
                   std::shared_ptr<AssetLibrary>     assetLibrary) override;

            void
            headerParsed(const std::vector<unsigned char>&   data,
                         std::shared_ptr<Options>            options,
                         unsigned int&                       linkedAssetId) override;

            void
            lodParsed(int                                previousLod,
                      int                                currentLod,
                      const std::vector<unsigned char>&  data,
                      std::shared_ptr<Options>           options) override;

            bool
            complete(int currentLod) override;
            
            void
            completed() override;

            void
            lodRangeFetchingBound(int  currentLod,
                                  int  requiredLod,
                                  int& lodRangeMinSize,
                                  int& lodRangeMaxSize,
                                  int& lodRangeRequestMinSize,
                                  int& lodRangeRequestMaxSize) override;

            void
            lodRangeRequestByteRange(int lowerLod, int upperLod, int& offset, int& size) const override;

            int
            lodLowerBound(int lod) const override;

            int
            maxLod() const override;

            void
            lodParsed(int                                previousLod,
                      int                                currentLod,
                      const std::vector<unsigned char>&  data,
                      std::shared_ptr<Options>           options,
                      bool                               disposeIndexBuffer,
                      bool                               disposeVertexBuffer);

        private:
            POPGeometryParser();

            std::shared_ptr<geometry::Geometry>
            createPOPGeometry(std::shared_ptr<AssetLibrary> assetLibrary,
                              std::shared_ptr<Options>      options,
                              const std::string&            fileName);
        };
    }
}
