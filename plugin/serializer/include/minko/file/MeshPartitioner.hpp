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
        class MeshPartitioner :
            public AbstractWriterPreprocessor<std::shared_ptr<scene::Node>>
        {
        public:
            typedef std::shared_ptr<MeshPartitioner> Ptr;

            typedef std::shared_ptr<scene::Node> NodePtr;

            struct Options
            {
                static const unsigned int none = 0;

                static const unsigned int mergeSurfaces                 = 1 << 0;
                static const unsigned int createOneNodePerSurface       = 1 << 1;
                static const unsigned int useBorderAsSharedTriangles    = 1 << 2;
                static const unsigned int uniformizeSize                = 1 << 3;

                static const unsigned int all                           =
                    mergeSurfaces |
                    createOneNodePerSurface |
                    useBorderAsSharedTriangles |
                    uniformizeSize;

                int                                                     _maxDepth;
                int                                                     _maxTriangleCountPerNode;

                unsigned int                                            _flags;

                unsigned int                                            _borderMinPrecision;
                unsigned int                                            _borderMaxDeltaPrecision;

                std::function<math::vec3(NodePtr)>                      _partitionMaxSizeFunction;

                std::function<void(NodePtr, math::vec3&, math::vec3&)>  _worldBoundsFunction;
            };

        private:
            typedef std::shared_ptr<file::AssetLibrary> AssetLibraryPtr;

        private:
            struct OctreeNode;

            typedef std::shared_ptr<OctreeNode> OctreeNodePtr;
            typedef std::weak_ptr<OctreeNode> OctreeNodeWeakPtr;

            typedef std::shared_ptr<geometry::Geometry> GeometryPtr;

            typedef std::shared_ptr<component::Surface> SurfacePtr;

            struct OctreeNode
            {
                OctreeNode(int depth,
                           const math::vec3& minBound,
                           const math::vec3& maxBound,
                           OctreeNodePtr parent) :
                    _depth(depth),
                    _minBound(minBound),
                    _maxBound(maxBound),
                    _triangles(1, std::vector<int>()),
                    _sharedTriangles(1, std::vector<int>()),
                    _parent(parent),
                    _children()
                {
                }

                int _depth;
                math::vec3 _minBound;
                math::vec3 _maxBound;

                std::vector<std::vector<int>> _triangles;
                std::vector<std::vector<int>> _sharedTriangles;

                OctreeNodeWeakPtr _parent;

                std::vector<OctreeNodePtr> _children;
            };

        private:
            Options _options;

            math::vec3 _worldMinBound;
            math::vec3 _worldMaxBound;

            std::vector<int> _indices;
            std::vector<float> _vertices;

            int _vertexSize;
            int _positionAttributeOffset;

            int _baseDepth;

        public:
            ~MeshPartitioner() = default;

            inline
            static
            Ptr
            create(Options options)
            {
                auto instance = Ptr(new MeshPartitioner());

                instance->_options = options;

                return instance;
            }

            void
            process(NodePtr& node, AssetLibraryPtr assetLibrary);

        private:
            MeshPartitioner();

            OctreeNodePtr
            pickBestPartitions(OctreeNodePtr      root,
                               const math::vec3&  modelMinBound,
                               const math::vec3&  modelMaxBound);

            OctreeNodePtr
            ensurePartitionSizeIsValid(OctreeNodePtr       node,
                                       const math::vec3&   maxSize);

            OctreeNodePtr
            buildPartitions(const std::vector<SurfacePtr>&  surfaces,
                            NodePtr                         root,
                            bool                            transformPositions,
                            const math::mat4&               transformMatrix);

            void
            patchNodeFromPartitions(NodePtr         node,
                                    SurfacePtr      referenceSurface,
                                    OctreeNodePtr   partitionNode,
                                    AssetLibraryPtr assetLibrary);

            void
            processBorders(OctreeNodePtr   partitionNode,
                           int             borderMinPrecision,
                           int             borderMaxDeltaPrecision);

            GeometryPtr
            createGeometry(GeometryPtr referenceGeometry, const std::vector<int>& triangleIndices);

            static
            std::vector<std::vector<SurfacePtr>>
            mergeSurfaces(const std::vector<SurfacePtr>& surfaces);

            static
            int
            indexAt(int x, int y, int z);

            void
            insertTriangle(OctreeNodePtr        partitionNode,
                           int                  triangleIndex,
                           const math::mat4&    transformMatrix);

            void
            splitNode(OctreeNodePtr     partitionNode,
                      const math::mat4& transformMatrix);

            static
            int
            computeDepth(OctreeNodePtr partitionNode);
        };
    }
}
