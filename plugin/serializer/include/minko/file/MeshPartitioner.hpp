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
#include "minko/Hash.hpp"
#include "minko/SerializerCommon.hpp"
#include "minko/StreamingCommon.hpp"
#include "minko/file/AbstractWriterPreprocessor.hpp"

namespace minko
{
    namespace file
    {
        class MeshPartitioner :
            public AbstractWriterPreprocessor<std::shared_ptr<scene::Node>>
        {
        public:
            struct Options;

            typedef std::shared_ptr<MeshPartitioner>    Ptr;

            typedef std::shared_ptr<scene::Node>        NodePtr;
            typedef std::shared_ptr<scene::NodeSet>     NodeSetPtr;

            typedef std::shared_ptr<component::Surface> SurfacePtr;

            struct SurfaceIndexer
            {
                std::function<std::size_t(SurfacePtr)>      hash;
                std::function<bool(SurfacePtr, SurfacePtr)> equal;
            };

            struct Options
            {
                static const unsigned int none = 0;

                static const unsigned int mergeSurfaces                     = 1 << 0;
                static const unsigned int createOneNodePerSurface           = 1 << 1;
                static const unsigned int applyCrackFreePolicy              = 1 << 2;

                static const unsigned int all                               = mergeSurfaces |
                                                                              createOneNodePerSurface |
                                                                              applyCrackFreePolicy;

                int                                                         maxNumTrianglesPerNode;
                int                                                         maxNumIndicesPerNode;

                int                                                         maxNumSurfacesPerSurfaceBucket;
                int                                                         maxNumTrianglesPerSurfaceBucket;

                unsigned int                                                flags;

                std::function<math::vec3(Options&, NodeSetPtr)>             partitionMaxSizeFunction;

                std::function<void(NodeSetPtr, math::vec3&, math::vec3&)>   worldBoundsFunction;

                std::function<bool(NodePtr)>                                nodeFilterFunction;
                SurfaceIndexer                                              surfaceIndexer;

                std::function<bool(SurfacePtr)>                             validSurfacePredicate;
                std::function<bool(SurfacePtr)>                             instanceSurfacePredicate;

                Options();
            };

        private:
            typedef std::shared_ptr<file::AssetLibrary> AssetLibraryPtr;

        private:
            struct OctreeNode;

            typedef std::shared_ptr<OctreeNode> OctreeNodePtr;
            typedef std::weak_ptr<OctreeNode> OctreeNodeWeakPtr;

            typedef std::shared_ptr<data::HalfEdge> HalfEdgePtr;
            typedef std::shared_ptr<data::HalfEdgeCollection> HalfEdgeCollectionPtr;

            typedef std::shared_ptr<geometry::Geometry> GeometryPtr;

            struct OctreeNode
            {
                OctreeNode(int depth,
                           const math::vec3& minBound,
                           const math::vec3& maxBound,
                           OctreeNodePtr parent) :
                    depth(depth),
                    minBound(minBound),
                    maxBound(maxBound),
                    triangles(1, std::vector<unsigned int>()),
                    sharedTriangles(1, std::vector<unsigned int>()),
                    indices(1, std::set<unsigned int>()),
                    sharedIndices(1, std::set<unsigned int>()),
                    parent(parent),
                    children()
                {
                }

                int                                     depth;
                math::vec3                              minBound;
                math::vec3                              maxBound;

                std::vector<std::vector<unsigned int>>  triangles;
                std::vector<std::vector<unsigned int>>  sharedTriangles;

                std::vector<std::set<unsigned int>>     indices;
                std::vector<std::set<unsigned int>>     sharedIndices;

                OctreeNodeWeakPtr                       parent;

                std::vector<OctreeNodePtr>              children;
            };

            struct PartitionInfo
            {
                NodePtr                     root;
                std::vector<SurfacePtr>     surfaces;

                bool                        useRootSpace;
                bool                        isInstance;

                std::vector<unsigned int>   indices;
                std::vector<float>          vertices;

                math::vec3                  minBound;
                math::vec3                  maxBound;

                unsigned int                vertexSize;
                unsigned int                positionAttributeOffset;

                int                         baseDepth;

                std::vector<HalfEdgePtr>    halfEdges;
                std::list<HalfEdgePtr>      halfEdgeReferences;

                std::unordered_set<
                    unsigned int
                >                           protectedIndices;

                std::shared_ptr<
                    math::SpatialIndex<std::unordered_set<unsigned int>>
                >                           mergedIndices;
                std::unordered_set<
                    unsigned int
                >                           markedDiscontinousIndices;

                OctreeNodePtr               rootPartitionNode;

                PartitionInfo() = default;
                ~PartitionInfo() = default;
            };

        private:
            Options                                                     _options;
            std::shared_ptr<StreamingOptions>                           _streamingOptions;

            AssetLibraryPtr                                             _assetLibrary;

            NodeSetPtr                                                  _filteredNodes;

            math::vec3                                                  _worldMinBound;
            math::vec3                                                  _worldMaxBound;

            std::unordered_map<GeometryPtr, std::vector<GeometryPtr>>   _processedInstances;

            float                                                       _progressRate;
            StatusChangedSignal::Ptr                                    _statusChanged;

        public:
            ~MeshPartitioner() = default;

            inline
            static
            Ptr
            create(Options options, std::shared_ptr<StreamingOptions> streamingOptions)
            {
                auto instance = Ptr(new MeshPartitioner());

                instance->_options = options;
                instance->_streamingOptions = streamingOptions;

                return instance;
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
            process(NodePtr& node, AssetLibraryPtr assetLibrary);

        private:
            MeshPartitioner();

            static
            SurfaceIndexer
            defaultSurfaceIndexer();

            static
            bool
            defaultValidSurfacePredicate(SurfacePtr surface);

            static
            bool
            defaultInstanceSurfacePredicate(SurfacePtr surface);

            static
            bool
            defaultNodeFilterFunction(NodePtr node);

            static
            void
            defaultWorldBoundsFunction(NodeSetPtr nodes, math::vec3& minBound, math::vec3& maxBound);

            static
            math::vec3
            defaultPartitionMaxSizeFunction(Options& options, NodeSetPtr nodes);

            void
            findInstances(const std::vector<SurfacePtr>& surfaces);

            bool
            surfaceBucketIsValid(const std::vector<SurfacePtr>& surfaceBucket) const;

            void
            splitSurfaceBucket(const std::vector<SurfacePtr>&           surfaceBucket,
                               std::vector<std::vector<SurfacePtr>>&    splitSurfaceBucket);

            void
            splitSurface(SurfacePtr                 surface,
                         std::vector<SurfacePtr>&   splitSurface);

            OctreeNodePtr
            ensurePartitionSizeIsValid(OctreeNodePtr        node,
                                       const math::vec3&    maxSize,
                                       PartitionInfo&       partitionInfo);

            GeometryPtr
            createGeometry(GeometryPtr                      referenceGeometry,
                           const std::vector<unsigned int>& triangleIndices,
                           PartitionInfo&                   partitionInfo);

            void
            markProtectedVertices(GeometryPtr                                               geometry,
                                  const std::unordered_map<unsigned short, unsigned int>&   indices,
                                  PartitionInfo&                                            partitionInfo);

            std::vector<std::vector<SurfacePtr>>
            mergeSurfaces(const std::vector<SurfacePtr>& surfaces);

            bool
            preprocessMergedSurface(PartitionInfo&  partitionInfo,
                                    SurfacePtr      surface,
                                    int             index);

            bool
            buildGlobalIndex(PartitionInfo& partitionInfo);

            bool
            buildHalfEdges(PartitionInfo& partitionInfo);

            bool
            buildPartitions(PartitionInfo& partitionInfo);

            void
            registerSharedTriangle(OctreeNodePtr    partitionNode,
                                   unsigned int     triangleIndex,
                                   PartitionInfo&   partitionInfo);

            bool
            buildGeometries(NodePtr                     node,
                            PartitionInfo&              partitionInfo,
                            std::vector<GeometryPtr>&   geometries);

            bool
            patchNode(NodePtr                           node,
                      PartitionInfo&                    partitionInfo,
                      const std::vector<GeometryPtr>&   geometries);

            static
            int
            indexAt(int x, int y, int z);

            void
            insertTriangle(OctreeNodePtr        partitionNode,
                           unsigned int         triangleIndex,
                           PartitionInfo&       partitionInfo);

            void
            splitNode(OctreeNodePtr     partitionNode,
                      PartitionInfo&    partitionInfo);

            int
            countTriangles(OctreeNodePtr partitionNode);

            static
            math::vec3
            positionAt(unsigned int         index,
                       const PartitionInfo& partitionInfo);

            static
            int
            computeDepth(OctreeNodePtr partitionNode);
        };
    }
}
