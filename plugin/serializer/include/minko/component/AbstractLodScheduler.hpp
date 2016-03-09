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
#include "minko/component/AbstractComponent.hpp"
#include "minko/data/Provider.hpp"

namespace minko
{
    namespace component
    {
        class AbstractLodScheduler :
            public AbstractComponent
        {
        public:
            typedef std::shared_ptr<AbstractLodScheduler>           Ptr;

            typedef std::shared_ptr<data::Provider>                 ProviderPtr;

            typedef std::shared_ptr<scene::Node>                    NodePtr;

            typedef std::shared_ptr<Surface>                        SurfacePtr;

            typedef std::function<AbstractComponent::Ptr(NodePtr)>  ComponentSolverFunction;

            struct LodInfo
            {
                int     requiredLod;
                float   priority;

                inline
                LodInfo() :
                    requiredLod(0),
                    priority(0.f)
                {
                }

                inline
                bool
                equals(const LodInfo& other) const
                {
                    return
                        requiredLod == other.requiredLod &&
                        priority == other.priority;
                }
            };

        protected:
            struct ResourceInfo
            {
                ProviderPtr														data;

                bool															lodRequirementIsInvalid;

                LodInfo															lodInfo;

                Signal<ProviderPtr, const data::Provider::PropertyName&>::Slot  propertyChangedSlot;
                Signal<NodePtr, NodePtr>::Slot                                  layoutChangedSlot;

                inline
                explicit
                ResourceInfo(ProviderPtr data) :
                    data(data),
                    lodRequirementIsInvalid(true),
                    lodInfo(),
                    propertyChangedSlot(),
                    layoutChangedSlot()
                {
                }

                inline
                const std::string&
                uuid() const
                {
                    return data->uuid();
                }
            };

        private:
            typedef std::shared_ptr<AbstractComponent>      AbstractComponentPtr;
            typedef std::shared_ptr<Renderer>               RendererPtr;
            typedef std::shared_ptr<SceneManager>           SceneManagerPtr;
            typedef std::shared_ptr<MasterLodScheduler>     MasterLodSchedulerPtr;

            typedef data::Store*                            StorePtr;

        protected:
            static const int																DEFAULT_LOD;

        private:
            MasterLodSchedulerPtr															_masterLodScheduler;

            std::unordered_map<std::string, ResourceInfo>									_resources;

            ComponentSolverFunction															_sceneManagerFunction;
            ComponentSolverFunction															_rendererFunction;
            ComponentSolverFunction															_masterLodSchedulerFunction;

            Signal<NodePtr, NodePtr, NodePtr>::Slot											_nodeAddedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot											_nodeRemovedSlot;

            Signal<NodePtr, NodePtr, AbstractComponentPtr>::Slot							_componentAddedSlot;
            Signal<NodePtr, NodePtr, AbstractComponentPtr>::Slot							_componentRemovedSlot;

            Signal<SceneManagerPtr, float, float>::Slot										_frameBeginSlot;

            Signal<data::Store&, ProviderPtr, const data::Provider::PropertyName&>::Slot	_rootNodePropertyChangedSlot;
            Signal<data::Store&, ProviderPtr, const data::Provider::PropertyName&>::Slot	_rendererNodePropertyChangedSlot;

            std::unordered_map<NodePtr, Signal<NodePtr, NodePtr>::Slot>                     _nodeLayoutChangedSlots;
            std::unordered_map<SurfacePtr, Signal<AbstractComponentPtr>::Slot>              _surfaceLayoutmaskChangedSlots;

            std::list<SurfacePtr>															_addedSurfaces;
            std::list<SurfacePtr>															_removedSurfaces;

            bool                                                                            _enabled;

            float                                                                           _frameTime;

        public:
            ~AbstractLodScheduler() = default;

            inline
            ComponentSolverFunction
            sceneManagerFunction() const
            {
                return _sceneManagerFunction;
            }

            inline
            Ptr
            sceneManagerFunction(ComponentSolverFunction value)
            {
                _sceneManagerFunction = value;

                return std::static_pointer_cast<AbstractLodScheduler>(shared_from_this());
            }

            inline
            ComponentSolverFunction
            rendererFunction() const
            {
                return _rendererFunction;
            }

            inline
            Ptr
            rendererFunction(ComponentSolverFunction value)
            {
                _rendererFunction = value;

                return std::static_pointer_cast<AbstractLodScheduler>(shared_from_this());
            }

            inline
            ComponentSolverFunction
            masterLodSchedulerFunction() const
            {
                return _masterLodSchedulerFunction;
            }

            inline
            Ptr
            masterLodSchedulerFunction(ComponentSolverFunction value)
            {
                _masterLodSchedulerFunction = value;

                return std::static_pointer_cast<AbstractLodScheduler>(shared_from_this());
            }

            void
            invalidateLodRequirement();

            void
            forceUpdate();

            inline
            bool
            enabled() const
            {
                return _enabled;
            }

            inline
            Ptr
            enabled(bool value)
            {
                if (_enabled == value)
                    return std::static_pointer_cast<AbstractLodScheduler>(shared_from_this());

                _enabled = value;

                if (_enabled)
                    invalidateLodRequirement();

                return std::static_pointer_cast<AbstractLodScheduler>(shared_from_this());
            }

            void
            layoutMask(scene::Layout value) override;

        protected:
            AbstractLodScheduler();

            inline
            MasterLodSchedulerPtr
            masterLodScheduler() const
            {
                return _masterLodScheduler;
            }

            void
            targetAdded(NodePtr target) override;

            void
            targetRemoved(NodePtr target) override;

            ResourceInfo&
            registerResource(ProviderPtr data);

            void
            unregisterResource(const std::string& uuid);

            void
            updated(float time);

            void
            invalidateLodRequirement(ResourceInfo& resource);

            virtual
            void
            sceneManagerSet(SceneManagerPtr sceneManager);

            virtual
            void
            rendererSet(RendererPtr renderer);

            virtual
            void
            masterLodSchedulerSet(MasterLodSchedulerPtr masterLodScheduler);

            virtual
            void
            surfaceAdded(SurfacePtr surface);

            virtual
            void
            surfaceRemoved(SurfacePtr surface);

            virtual
            void
            viewPropertyChanged(const math::mat4&   worldToScreenMatrix,
                                const math::mat4&   viewMatrix,
                                const math::vec3&   eyePosition,
                                float               fov,
                                float               aspectRatio,
                                float               zNear,
                                float               zFar);

            virtual
            void
            viewportChanged(const math::vec4& viewport);

            virtual
            void
            maxAvailableLodChanged(ResourceInfo&    resource,
                                   int              maxAvailableLod);

            virtual
            void
            lodInfoChanged(ResourceInfo&    resource,
                           const LodInfo&   previousLodInfo,
                           const LodInfo&   lodInfo);

            virtual
            LodInfo
            lodInfo(ResourceInfo&   resource,
                    float           time) = 0;

        private:
            static
            AbstractComponent::Ptr
            defaultSceneManagerFunction(NodePtr node);

            static
            AbstractComponent::Ptr
            defaultRendererFunction(NodePtr node);

            static
            AbstractComponent::Ptr
            defaultMasterLodSchedulerFunction(NodePtr node);

            void
            nodeAddedHandler(NodePtr target, NodePtr node);

            void
            nodeRemovedHandler(NodePtr target, NodePtr node);

            void
            componentAddedHandler(NodePtr target, AbstractComponentPtr component);

            void
            componentRemovedHandler(NodePtr target, AbstractComponentPtr component);

            void
            frameBeginHandler(SceneManagerPtr sceneManager, float time, float deltaTime);

            void
            rootNodePropertyChangedHandler(data::Store&								store,
                                           ProviderPtr								provider,
										   const data::Provider::PropertyName&		propertyName);

            void
            rendererNodePropertyChangedHandler(data::Store&								store,
                                               ProviderPtr								provider,
											   const data::Provider::PropertyName&		propertyName);

            void
            collectSurfaces();

            bool
            checkSurfaceLayout(SurfacePtr surface);

            void
            surfaceLayoutMaskInvalidated(SurfacePtr surface);

            void
            watchSurface(SurfacePtr surface);

            void
            unwatchSurface(SurfacePtr surface);

            void
            addPendingSurface(SurfacePtr surface);

            void
            removePendingSurface(SurfacePtr surface);
        };
    }
}
