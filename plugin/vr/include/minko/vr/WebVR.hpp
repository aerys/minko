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
#include "minko/Signal.hpp"
#include "minko/log/Logger.hpp"

#include "VRImpl.hpp"

namespace minko
{
    namespace vr
    {
        class WebVR : public VRImpl
        {
        public:
            typedef std::shared_ptr<WebVR> Ptr;

        private:
            Signal<std::shared_ptr<minko::component::Renderer>>::Slot   _renderingEndSlot;

            float _zNear;
            float _zFar;
            std::shared_ptr<minko::component::Renderer> _leftRenderer;
            std::shared_ptr<minko::component::Renderer> _rightRenderer;
        public:

            void
            initialize(std::shared_ptr<component::SceneManager> sceneManager) override;

            void
            initializeVRDevice(std::shared_ptr<component::Renderer> leftRenderer, std::shared_ptr<component::Renderer> rightRenderer, void* window = nullptr) override;

            void
            targetAdded() override;

            void
            targetRemoved() override;

            float
            getLeftEyeFov() override;

            float
            getRightEyeFov() override;

            void
            updateCamera(std::shared_ptr<scene::Node> target, std::shared_ptr<scene::Node> leftCamera, std::shared_ptr<scene::Node> rightCamera) override;

            void
            updateViewport(int viewportWidth, int viewportHeight) override;

            static
            bool
            supported();

            static
            bool
            detected();

            inline
            float
            zNear() override
            {
                return _zNear;
            }

            inline
            float
            zFar() override
            {
                return _zFar;
            }

            inline static
            Ptr
            create(int viewportWidth, int viewportHeight, float zNear, float zFar)
            {
                LOG_INFO("Create a WebVR instance.");

                auto ptr = std::shared_ptr<WebVR>(new WebVR(viewportWidth, viewportHeight, zNear, zFar));

                return ptr;
            }

        private:
            WebVR(int viewportWidth, int viewportHeight, float zNear, float zFar);
        };
    }
}
