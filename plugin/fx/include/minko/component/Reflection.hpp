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
#include "minko/component/AbstractComponent.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/Transform.hpp"

namespace minko
{
    namespace component
    {
        class Reflection :
            public AbstractComponent,
            public std::enable_shared_from_this<Reflection>
        {
        public:
            typedef std::shared_ptr<Reflection>	Ptr;

        private:
            typedef std::shared_ptr<scene::Node>		        NodePtr;
            typedef std::shared_ptr<AbstractComponent>	        AbsCmpPtr;
            typedef std::shared_ptr<data::StructureProvider>	ContainerPtr;

        private:
            // Signals
            Signal<AbsCmpPtr, NodePtr>::Ptr				            _rootAdded;

            // Slots
            Signal<AbsCmpPtr, NodePtr>::Slot				                    _targetAddedSlot;
            Signal<AbsCmpPtr, NodePtr>::Slot				                    _targetRemovedSlot;
            Signal<AbsCmpPtr, NodePtr>::Slot				                    _rootAddedSlot;
            Signal<std::shared_ptr<data::Provider>, const std::string&>::Slot	_viewMatrixChangedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot								_addedToSceneSlot;

            uint                                                _width;
            uint                                                _height;
            uint                                                _clearColor;
            std::shared_ptr<file::AssetLibrary>                 _assets;

            // One active camera only
            std::shared_ptr<render::Texture>                    _renderTarget;
            NodePtr                                             _activeCamera;
            NodePtr                                             _virtualCamera;
            PerspectiveCamera::Ptr                              _perspectiveCamera;
            Transform::Ptr                                      _cameraTransform;
            Transform::Ptr                                      _virtualCameraTransform;

            // Multiple active cameras
            std::shared_ptr<render::Effect>                     _reflectionEffect;
            std::vector<NodePtr>                                _cameras;
            std::vector<NodePtr>                                _virtualCameras;
            std::vector<std::shared_ptr<render::Texture>>       _renderTargets;
            std::array<float, 4>                                _clipPlane;

        public:
            inline static
            Ptr
            create(
                std::shared_ptr<file::AssetLibrary> assets,
                uint renderTargetWidth,
                uint renderTargetHeight,
                uint clearColor)
            {
                auto reflection = std::shared_ptr<Reflection>(new Reflection(
                    assets, renderTargetWidth, renderTargetHeight, clearColor));

                reflection->initialize();

                return reflection;
            }

            inline
            Signal<AbsCmpPtr, std::shared_ptr<scene::Node>>::Ptr
            rootAdded() const
            {
                return _rootAdded;
            }

            inline
            std::shared_ptr<render::Texture>
            getRenderTarget()
            {
                return _renderTarget;
            }

            void
            updateReflectionMatrix();

        private:

            Reflection(
                std::shared_ptr<file::AssetLibrary> assets,
                uint renderTargetWidth,
                uint renderTargetHeight,
                uint clearColor);

            void
            initialize();

            void
            targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

            void
            targetAddedToScene(NodePtr node, NodePtr target, NodePtr ancestor);

            void
            cameraPropertyValueChangedHandler(std::shared_ptr<data::Provider> provider, const std::string& property);

            void
            updateReflectionMatrixes();
        };
    }
}
