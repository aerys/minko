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
        class ScreenSpaceAreaNotifier :
            public AbstractComponent
        {
        public:
            typedef std::shared_ptr<ScreenSpaceAreaNotifier>           Ptr;

            typedef std::shared_ptr<data::Provider>                 ProviderPtr;

            typedef std::shared_ptr<scene::Node>                    NodePtr;

            typedef std::shared_ptr<Surface>                        SurfacePtr;

            typedef std::function<AbstractComponent::Ptr(NodePtr)>  ComponentSolverFunction;

        private:
            typedef std::shared_ptr<scene::NodeSet>                 NodeSetPtr;

            typedef std::shared_ptr<AbstractComponent>              AbstractComponentPtr;
            typedef std::shared_ptr<Renderer>                       RendererPtr;
            typedef std::shared_ptr<SceneManager>                   SceneManagerPtr;

            typedef data::Store*                                    StorePtr;

            struct NodeEntry
            {
                NodePtr                     node;
                ProviderPtr                 provider;

                std::array<math::vec3, 8>   boxVertices;

                bool                        updateNeeded;

                float                       previousValue;
                float                       targetValue;

                Signal<
                    data::Store&,
                    ProviderPtr,
                    const data::Provider::PropertyName&
                >::Slot                     modelToWorldMatrixChangedSlot;

                NodeEntry() :
                    node(),
                    boxVertices(),
                    updateNeeded(true),
                    previousValue(0.f),
                    targetValue(0.f),
                    modelToWorldMatrixChangedSlot()
                {
                }
            };

        private:
            ComponentSolverFunction                                                         _sceneManagerFunction;
            ComponentSolverFunction                                                         _rendererFunction;

            Signal<NodePtr, NodePtr, NodePtr>::Slot                                         _nodeAddedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot                                         _nodeRemovedSlot;

            Signal<NodePtr, NodePtr, AbstractComponentPtr>::Slot                            _componentAddedSlot;
            Signal<NodePtr, NodePtr, AbstractComponentPtr>::Slot                            _componentRemovedSlot;

            Signal<SceneManagerPtr, float, float>::Slot                                     _frameBeginSlot;

            Signal<data::Store&, ProviderPtr, const data::Provider::PropertyName&>::Slot    _rootNodePropertyChangedSlot;
            Signal<data::Store&, ProviderPtr, const data::Provider::PropertyName&>::Slot    _rendererNodePropertyChangedSlot;

            math::vec3                                                                      _eyePosition;
            float                                                                           _fov;
            float                                                                           _aspectRatio;

            math::vec4                                                                      _viewport;

            math::mat4                                                                      _worldToScreenMatrix;
            math::mat4                                                                      _viewMatrix;

            std::string                                                                     _propertyName;
            float                                                                           _updateRate;

            std::unordered_map<NodePtr, NodeEntry>                                          _nodeEntries;

            bool                                                                            _updateNeeded;
            float                                                                           _updateTime;
            float                                                                           _previousTime;

        public:
            inline
            static
            Ptr
            create(const std::string& propertyName = "screenSpaceArea", float updateRate = 0.f)
            {
                auto instance = Ptr(new ScreenSpaceAreaNotifier());

                instance->propertyName(propertyName);
                instance->updateRate(updateRate);

                return instance;
            }

            ~ScreenSpaceAreaNotifier() = default;

            inline
            const std::string&
            propertyName() const
            {
                return _propertyName;
            }

            inline
            Ptr
            propertyName(const std::string& value)
            {
                _propertyName  = value;

                return std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this());
            }

            inline
            float
            updateRate() const
            {
                return _updateRate;
            }

            inline
            Ptr
            updateRate(float value)
            {
                if (_updateRate == value)
                    return std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this());

                _updateRate = value;

                if (_updateRate > 0.f)
                {
                    _updateTime = 1.f / _updateRate;
                }
                else
                {
                    _updateTime = 0.f;
                }

                return std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this());
            }

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

                return std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this());
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

                return std::static_pointer_cast<ScreenSpaceAreaNotifier>(shared_from_this());
            }

        private:
            ScreenSpaceAreaNotifier();

            void
            targetAdded(NodePtr target);

            void
            targetRemoved(NodePtr target);

            void
            sceneManagerSet(SceneManagerPtr sceneManager);

            void
            candidateNodeAdded(NodePtr node);

            void
            candidateNodeRemoved(NodePtr node);

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
            update(float time);

            static
            AbstractComponent::Ptr
            defaultSceneManagerFunction(NodePtr node);

            static
            AbstractComponent::Ptr
            defaultRendererFunction(NodePtr node);

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
            rootNodePropertyChangedHandler(data::Store&                             store,
                                           ProviderPtr                              provider,
                                           const data::Provider::PropertyName&      propertyName);

            void
            rendererNodePropertyChangedHandler(data::Store&                             store,
                                               ProviderPtr                              provider,
                                               const data::Provider::PropertyName&      propertyName);

            void
            modelToWorldMatrixChanged(NodeEntry&        nodeEntry,
                                      const math::mat4& modelToWorldMatrix);

            float
            computeScreenSpaceArea(NodeEntry&          nodeEntry,
                                   const math::vec3&   eyePosition,
                                   const math::vec4&   viewport,
                                   const math::mat4&   worldToScreenMatrix,
                                   const math::mat4&   viewMatrix,
                                   float               time);

            float
            accurateScreenSpaceArea(const std::array<math::vec3, 8>& boxVertices,
                                    const math::mat4&                modelToWorld,
                                    const math::vec3&                eyePosition,
                                    const math::vec4&                viewport,
                                    const math::mat4&                worldToScreenMatrix,
                                    const math::mat4&                viewMatrix) const;

            float
            aabbApproxScreenSpaceArea(const std::array<math::vec3, 8>&   boxVertices,
                                      const math::mat4&                  modelToWorld,
                                      const math::vec3&                  eyePosition,
                                      const math::vec4&                  viewport,
                                      const math::mat4&                  worldToScreenMatrix,
                                      const math::mat4&                  viewMatrix) const;
        };
    }
}
