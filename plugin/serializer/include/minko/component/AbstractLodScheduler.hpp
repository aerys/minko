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

                Signal<ProviderPtr, const data::Provider::PropertyName&>::Slot   propertyChangedSlot;

                inline
                explicit
                ResourceInfo(ProviderPtr data) :
                    data(data),
                    lodRequirementIsInvalid(true),
                    lodInfo()
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
            typedef std::shared_ptr<scene::NodeSet>         NodeSetPtr;

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

            std::list<SurfacePtr>															_addedSurfaces;
            std::list<SurfacePtr>															_removedSurfaces;
            NodeSetPtr																		_candidateNodes;

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

        protected:
            AbstractLodScheduler();

            inline
            MasterLodSchedulerPtr
            masterLodScheduler() const
            {
                return _masterLodScheduler;
            }

            void
            targetAdded(NodePtr target);

            void
            targetRemoved(NodePtr target);

            ResourceInfo&
            registerResource(ProviderPtr data);

            void
            unregisterResource(const std::string& uuid);

            void
            invalidateLodRequirement(ResourceInfo& resource);

            virtual
            void
            sceneManagerSet(SceneManagerPtr sceneManager);

            virtual
            void
            masterLodSchedulerSet(MasterLodSchedulerPtr masterLodScheduler);

            virtual
            void
            candidateNodeAdded(NodePtr target, NodePtr node);

            virtual
            void
            candidateNodeRemoved(NodePtr target, NodePtr node);

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
            update();

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
            lodInfo(ResourceInfo& resource) = 0;

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
            rendererSet(RendererPtr renderer);

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
            updateCandidateNodes(NodePtr target);

            bool
            nodeIsCandidate(NodePtr node);
        };
    }
}
