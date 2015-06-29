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
#include "VRImpl.hpp"
#include "minko/Signal.hpp"

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
#include "minko/sensors/Attitude.hpp"
#include "minko/sensors/Magnetometer.hpp"
#endif

namespace minko
{
    namespace oculus
    {
        class Cardboard : public VRImpl
        {
        private:
            std::shared_ptr<scene::Node>                        _ppScene;
            uint                                                _renderTargetWidth;
            uint                                                _renderTargetHeight;
            std::shared_ptr<render::Texture>                    _renderTarget;
            std::array<std::pair<math::vec2, math::vec2>, 2>    _uvScaleOffset;

#if MINKO_PLATFORM == MINKO_PLATFORM_IOS || MINKO_PLATFORM == MINKO_PLATFORM_ANDROID
            std::shared_ptr<sensors::Attitude>                  _attitude;
            std::shared_ptr<sensors::Magnetometer>              _magnetometer;
#endif

            float                                               _zNear;
            float                                               _zFar;
            bool                                                _isMagnetDown;
            std::vector<std::vector<float>>                     _magnetSensorData;
            std::vector<float>                                  _magnetOffset;
            Signal<float, float, float>::Slot                   _magnetChangedSlot;
            Signal<>::Ptr                                       _magnetPressed;
            Signal<>::Ptr                                       _magnetReleased;

        public:
            typedef std::shared_ptr<Cardboard> Ptr;

            void
            initialize(std::shared_ptr<component::SceneManager> sceneManager);

            void
            initializeVRDevice(std::shared_ptr<component::Renderer> leftRenderer, std::shared_ptr<component::Renderer> rightRenderer, void* window = nullptr);

            void
            targetRemoved();

            void
            updateCameraOrientation(std::shared_ptr<scene::Node> target);

            void
            updateViewport(int viewportWidth, int viewportHeight);

            float
            getLeftEyeFov();

            float
            getRightEyeFov();

            float
            zNear()
            {
                return _zNear;
            }

            float
            zFar()
            {
                return _zFar;
            }

            inline
            Signal<>::Ptr
            actionButtonPressed()
            {
                return _magnetPressed;
            }

            inline
            Signal<>::Ptr
            actionButtonReleased()
            {
                return _magnetReleased;
            }

            inline static
            Ptr
            create(int viewportWidth, int viewportHeight, float zNear, float zFar)
            {
                auto ptr = std::shared_ptr<Cardboard>(new Cardboard(viewportWidth, viewportHeight, zNear, zFar));

                return ptr;
            }

        private:
            Cardboard(int viewportWidth, int viewportHeight, float zNear, float zFar);

            void
            evaluateMagnetModel();

            std::vector<float>
            computeMagnetOffsets(int start, std::vector<float>);

            float
            computeMean(std::vector<float> offsets);

            float
            computeMaximum(std::vector<float> offsets);

            float
            computeMinimum(std::vector<float> offsets);
        };
    }
}
