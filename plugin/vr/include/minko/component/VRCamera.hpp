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

#include "minko/Minko.hpp"
#include "minko/Common.hpp"
#include "minko/Signal.hpp"
#include "minko/component/AbstractComponent.hpp"

namespace minko
{
    namespace vr
    {
        class VRImpl;
    }

    namespace component
    {
        class VRCamera :
            public AbstractComponent
        {
        public:
            typedef std::shared_ptr<VRCamera> Ptr;

        private:
            typedef std::shared_ptr<scene::Node>				NodePtr;
            typedef std::shared_ptr<AbstractComponent>			AbsCmpPtr;
            typedef std::shared_ptr<SceneManager>				SceneMgrPtr;
            typedef std::shared_ptr<render::AbstractTexture>    AbsTexturePtr;

        private:
            SceneMgrPtr                                         _sceneManager;

            Signal<NodePtr, NodePtr, NodePtr>::Slot             _addedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot             _removedSlot;
            Signal<SceneMgrPtr, float, float>::Slot             _frameBeginSlot;

            std::shared_ptr<vr::VRImpl>                         _VRImpl;

            std::shared_ptr<component::Renderer>                _leftRenderer;
            std::shared_ptr<component::Renderer>                _rightRenderer;
            std::shared_ptr<scene::Node>                        _leftCameraNode;
            std::shared_ptr<scene::Node>                        _rightCameraNode;

            float                                               _viewportWidth;
            float                                               _viewportHeight;
            uint                                                _rendererClearColor;

        public:
            inline static
            Ptr
            create(int viewportWidth,
                   int viewportHeight,
                   float zNear = 0.1f,
                   float zFar = 1000.0f,
                   uint rendererClearColor = 0,
                   void* window = nullptr,
                   Renderer::Ptr leftRenderer = nullptr,
                   Renderer::Ptr rightRenderer = nullptr)
            {
                auto ptr = std::shared_ptr<VRCamera>(new VRCamera());

                if (!ptr)
                    return nullptr;

                ptr->initialize(viewportWidth, viewportHeight, zNear, zFar, rendererClearColor, window, leftRenderer, rightRenderer);

                return ptr;
            }

            void
            forceRatio(float aspectRatio);

            void
            updateViewport(int viewportWidth, int viewportHeight);

            static
            bool
            detected();

            void
            enableLeftRenderer(bool value)
            {
                _leftRenderer->enabled(value);
            }

            bool
            leftRendererEnabled()
            {
                return _leftRenderer->enabled();
            }

            void
            enableRightRenderer(bool value)
            {
                _rightRenderer->enabled(value);
            }

            bool
            rightRendererEnabled()
            {
                return _rightRenderer->enabled();
            }

        public:
            ~VRCamera();

        private:
            VRCamera();

            void
            initialize(int viewportWidth,
                       int viewportHeight,
                       float zNear,
                       float zFar,
                       uint rendererClearColor,
                       void* window,
                       Renderer::Ptr leftRenderer = nullptr,
                       Renderer::Ptr rightRenderer = nullptr);

            void
            updateCamera(std::shared_ptr<scene::Node> leftCamera, std::shared_ptr<scene::Node> rightCamera);

            void
            targetAdded(NodePtr target) override;

            void
            targetRemoved(NodePtr target) override;

            void
            findSceneManager();

            void
            setSceneManager(SceneMgrPtr);
        };
    }
}
