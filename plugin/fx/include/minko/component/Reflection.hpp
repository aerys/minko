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
#include "minko/component/AbstractScript.hpp"
#include "minko/component/Camera.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/SceneManager.hpp"

namespace minko
{
    namespace component
    {
        class Reflection : public AbstractScript
        {
        public:
            typedef std::shared_ptr<Reflection> Ptr;

        private:
            typedef std::shared_ptr<scene::Node>                            NodePtr;
            typedef std::shared_ptr<AbstractComponent>                      AbsCmpPtr;
            typedef std::shared_ptr<render::AbstractTexture>                AbsTexturePtr;
            typedef std::shared_ptr<render::Texture>                        TexturePtr;
            typedef Signal<SceneManager::Ptr, uint, AbsTexturePtr>::Slot    RenderSignalSlot;
            typedef std::shared_ptr<data::Provider>                         ProviderPtr;
            typedef Signal<ProviderPtr, const std::string&>::Slot           PropertyChangedSlot;

        private:
            // Signals
            Signal<AbsCmpPtr, NodePtr>::Ptr         _rootAdded;

            // Slots
            Signal<AbsCmpPtr, NodePtr>::Slot        _targetAddedSlot;
            Signal<AbsCmpPtr, NodePtr>::Slot        _targetRemovedSlot;
            Signal<AbsCmpPtr, NodePtr>::Slot        _rootAddedSlot;
            PropertyChangedSlot                     _viewMatrixChangedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot _addedToSceneSlot;
            RenderSignalSlot                        _frameRenderingSlot;

            uint                                    _width;
            uint                                    _height;
            uint                                    _clearColor;
            std::shared_ptr<file::AssetLibrary>     _assets;

            // One active camera only
            std::shared_ptr<render::Texture>        _renderTarget;
            NodePtr                                 _virtualCamera;
            NodePtr                                 _activeCamera;
            Camera::Ptr                             _camera;
            Transform::Ptr                          _cameraTransform;
            Transform::Ptr                          _virtualCameraTransform;
            Renderer::Ptr                           _reflectionRenderer;
            math::mat4                              _reflectedViewMatrix;

            // Multiple active cameras
            std::shared_ptr<render::Effect>         _reflectionEffect;
            std::vector<NodePtr>                    _cameras;
            std::vector<NodePtr>                    _virtualCameras;
            std::vector<TexturePtr>                 _renderTargets;
            std::array<float, 4>                    _clipPlane;

            bool                                    _enabled;

        public:
            inline static
            Ptr
            create(std::shared_ptr<file::AssetLibrary> assets,
                   uint renderTargetWidth,
                   uint renderTargetHeight,
                   uint clearColor)
            {
                return std::shared_ptr<Reflection>(new Reflection(
                    assets, renderTargetWidth, renderTargetHeight, clearColor
                ));
            }

            AbstractComponent::Ptr
            clone(const CloneOption& option);

            inline
            std::shared_ptr<render::Texture>
            renderTarget()
            {
                return _renderTarget;
            }

            void
            updateReflectionMatrix();

        private:
            Reflection(std::shared_ptr<file::AssetLibrary> assets,
                       uint renderTargetWidth,
                       uint renderTargetHeight,
                       uint clearColor);

            Reflection(const Reflection& reflection, const CloneOption& option);

            void
            start(NodePtr target);

            void
            update(NodePtr target);

            void
            stop(NodePtr target);
        };
    }
}
