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

#include "minko/component/AbstractLodScheduler.hpp"

namespace minko
{
	namespace component
	{
		class POPGeometryLodScheduler :
			public AbstractLodScheduler
		{
		public:
			typedef std::shared_ptr<POPGeometryLodScheduler>    Ptr;

        private:
            typedef std::shared_ptr<scene::Node>                NodePtr;

            typedef std::shared_ptr<AbstractComponent>          AbstractComponentPtr;
            typedef std::shared_ptr<SceneManager>               SceneManagerPtr;
            typedef std::shared_ptr<Renderer>                   RendererPtr;
            typedef std::shared_ptr<MasterLodScheduler>         MasterLodSchedulerPtr;
            typedef std::shared_ptr<Surface>                    SurfacePtr;

            typedef std::shared_ptr<math::Box>                  BoxPtr;

            typedef std::shared_ptr<geometry::Geometry>         GeometryPtr;

            struct SurfaceInfo
            {
                SurfacePtr                      surface;
                BoxPtr                          box;

                Signal<NodePtr, NodePtr>::Slot  layoutChangedSlot;
                Signal<
                    AbstractComponentPtr
                >::Slot                         layoutMaskChangedSlot;

                int                             activeLod;

                float                           requiredPrecisionLevel;

                float                           weight;

                explicit
                SurfaceInfo(SurfacePtr surface) :
                    surface(surface),
                    box(),
                    layoutChangedSlot(),
                    layoutMaskChangedSlot(),
                    activeLod(-1),
                    requiredPrecisionLevel(0),
                    weight(0.f)
                {
                }
            };

            struct POPGeometryResourceInfo
            {
                ResourceInfo*                               base;

                GeometryPtr                                 geometry;

                int                                         minLod;
                int                                         maxLod;
                int                                         minAvailableLod;
                int                                         maxAvailableLod;
                int                                         fullPrecisionLod;

                const std::map<
                    int,
                    ProgressiveOrderedMeshLodInfo
                >*                                          availableLods;

                static const ProgressiveOrderedMeshLodInfo  defaultLodInfo;
                std::vector<
                    const ProgressiveOrderedMeshLodInfo*
                >                                           lodToClosestValidLod;
                std::vector<
                    const ProgressiveOrderedMeshLodInfo*
                >                                           precisionLevelToClosestLod;

                std::unordered_multimap<
                    NodePtr,
                    Signal<data::Store&, ProviderPtr, const data::Provider::PropertyName&>::Slot
                >                                           propertyChangedSlots;
                std::vector<SurfaceInfo>                    surfaceInfoCollection;

                POPGeometryResourceInfo() :
                    base(nullptr),
                    geometry(),
                    minLod(-1),
                    maxLod(-1),
                    minAvailableLod(-1),
                    maxAvailableLod(-1),
                    fullPrecisionLod(-1),
                    availableLods(nullptr),
                    lodToClosestValidLod(),
                    precisionLevelToClosestLod(),
                    propertyChangedSlots(),
                    surfaceInfoCollection()
                {
                }
            };

        private:
            SceneManagerPtr                                             _sceneManager;
            RendererPtr                                                 _renderer;

            std::unordered_map<ProviderPtr, POPGeometryResourceInfo>    _popGeometryResources;

            math::vec3                                                  _eyePosition;
            float                                                       _fov;
            float                                                       _aspectRatio;

            math::vec4                                                  _viewport;

            math::mat4                                                  _worldToScreenMatrix;
            math::mat4                                                  _viewMatrix;

            float                                                       _blendingRange;

        public:
            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new POPGeometryLodScheduler());

                return instance;
            }

            void
            blendingRange(float value);

        protected:
            void
            sceneManagerSet(SceneManagerPtr sceneManager);

            void
            rendererSet(RendererPtr renderer);

            void
            surfaceAdded(SurfacePtr surface);

            void
            surfaceRemoved(SurfacePtr surface);

            void
            viewPropertyChanged(const math::mat4&   worldToScreenMatrix,
                                const math::mat4&   viewMatrix,
                                const math::vec3&   eyePosition,
                                float               fov,
                                float               aspectRatio,
                                float               zNear,
                                float               zFar);

            void
            viewportChanged(const math::vec4& viewport);

            void
            maxAvailableLodChanged(ResourceInfo&    resource,
                                   int              maxAvailableLod);

            LodInfo
            lodInfo(ResourceInfo&   resource,
                    float           time);

        private:
            POPGeometryLodScheduler();

            void
            layoutChanged(POPGeometryResourceInfo&  resource,
                          SurfaceInfo&              surfaceInfo);

            void
            activeLodChanged(POPGeometryResourceInfo&   resource,
                             SurfaceInfo&               surfaceInfo,
                             int                        previousLod,
                             int                        lod,
                             float                      requiredPrecisionLevel,
                             float                      time);

            int
            computeRequiredLod(const POPGeometryResourceInfo&   resource,
                               SurfaceInfo&                     surfaceInfo,
                               float&                           requiredPrecisionLevel);

            float
            computeLodPriority(const POPGeometryResourceInfo&  resource,
                               SurfaceInfo&                    surfaceInfo,
                               int                             requiredLod,
                               int                             activeLod,
                               float                           time);

            bool
            findClosestValidLod(const POPGeometryResourceInfo&          resource,
                                int                                     lod,
                                const ProgressiveOrderedMeshLodInfo*&   result) const;

            bool
            findClosestLodByPrecisionLevel(const POPGeometryResourceInfo&           resource,
                                           int                                      precisionLevel,
                                           const ProgressiveOrderedMeshLodInfo*&    result) const;

            void
            updateClosestLods(POPGeometryResourceInfo& resource);

            const ProgressiveOrderedMeshLodInfo&
            precisionLevelToClosestLod(const POPGeometryResourceInfo& resource, int precisionLevel) const;

            const ProgressiveOrderedMeshLodInfo&
            lodToClosestValidLod(const POPGeometryResourceInfo& resource, int lod) const;

            float
            distanceFromEye(const POPGeometryResourceInfo&  resource,
                            SurfaceInfo&                    surfaceInfo,
                            const math::vec3&               eyePosition);

            void
            requiredPrecisionLevelChanged(const POPGeometryResourceInfo&    resource,
                                          SurfaceInfo&                      surfaceInfo);
		};
	}
}
