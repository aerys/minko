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
#include "minko/SerializerCommon.hpp"
#include "minko/file/AbstractWriterPreprocessor.hpp"
#include "minko/log/Logger.hpp"
#include "minko/scene/Layout.hpp"

namespace minko
{
    namespace file
    {
        class SurfaceClusterBuilder :
            public AbstractWriterPreprocessor<std::shared_ptr<scene::Node>>
        {
        public:
            typedef std::shared_ptr<SurfaceClusterBuilder>                      Ptr;

            struct NodeInfo;

            typedef std::shared_ptr<component::Surface>                         SurfacePtr;

            typedef std::shared_ptr<AssetLibrary>                               AssetLibraryPtr;

            typedef std::shared_ptr<scene::Node>                                NodePtr;

            typedef std::function<bool(const NodeInfo&)>                        SurfaceClusterPredicateFunction;

            struct NodeInfo
            {
                std::vector<SurfacePtr> surfaces;

                unsigned int            numVertices;
                unsigned int            numTriangles;

                std::pair<
                    math::vec3,
                    math::vec3
                >                       bounds;
                math::vec3              size;
                float                   xyArea;
                float                   xzArea;
                float                   yzArea;
                float                   volume;
                float                   vertexDensity;
                float                   triangleDensity;

                float                   worldVolumeRatio;
                float                   worldNumVerticesRatio;
                float                   worldNumTrianglesRatio;
                math::vec3              worldSizeRatio;
                float                   worldXyAreaRatio;
                float                   worldXzAreaRatio;
                float                   worldYzAreaRatio;

                NodeInfo();
            };

        private:
            struct SurfaceClusterEntry
            {
                scene::Layout                   layout;
                SurfaceClusterPredicateFunction predicate;

                SurfaceClusterEntry(scene::Layout                   layout,
                                    SurfaceClusterPredicateFunction predicate) :
                    layout(layout),
                    predicate(predicate)
                {
                }
            };

        private:
            StatusChangedSignal::Ptr            _statusChanged;
            float                               _progressRate;

            std::vector<SurfaceClusterEntry>    _surfaceClusters;

            NodeInfo                            _rootNodeInfo;
            std::vector<NodeInfo>               _surfaceNodeInfo;

        public:
            ~SurfaceClusterBuilder() = default;

            static
            Ptr
            create()
            {
                auto instance = Ptr(new SurfaceClusterBuilder());

                return instance;
            }

            static
            void
            cacheNodeInfo(NodePtr                   root,
                          NodeInfo&                 rootNodeInfo,
                          std::vector<NodeInfo>&    surfaceNodeInfo);

            Ptr
            registerSurfaceCluster(scene::Layout                            layout,
                                   const SurfaceClusterPredicateFunction&   predicate);

            float
            progressRate() const override
            {
                return _progressRate;
            }

            StatusChangedSignal::Ptr
            statusChanged() override
            {
                return _statusChanged;
            }

            void
            process(NodePtr& node, AssetLibraryPtr assetLibrary) override;

        private:
            SurfaceClusterBuilder();

            void
            buildClusters();

            bool
            clusterAccepts(const SurfaceClusterEntry&   surfaceClusterEntry,
                           const NodeInfo&              nodeInfo);

            void
            addToCluster(const SurfaceClusterEntry&   surfaceClusterEntry,
                         const NodeInfo&              nodeInfo);
        };
    }
}
