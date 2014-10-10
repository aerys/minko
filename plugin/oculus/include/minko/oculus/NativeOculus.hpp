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
#include "OculusImpl.hpp"

extern "C" 
{
    struct ovrHmdDesc_;
    typedef const ovrHmdDesc_* ovrHmd;
}

namespace minko
{
    namespace oculus
    {
        class NativeOculus : public OculusImpl
        {
        private:
            struct HMDInfo
            {
                float hResolution;
                float vResolution;
                float hScreenSize;
                float vScreenSize;
                float vScreenCenter;
                float interpupillaryDistance;
                float lensSeparationDistance;
                float eyeToScreenDistance;
                std::shared_ptr<math::Vector4> distortionK;
            };

            ovrHmd  _hmd;

        public:
            typedef std::shared_ptr<NativeOculus> Ptr;

            void
            initializeOVRDevice(
                std::shared_ptr<component::Renderer> leftRenderer,
                std::shared_ptr<component::Renderer> rightRenderer,
                uint& renderTargetWidth,
                uint& renderTargetHeight,
                std::array<std::pair<Vector2Ptr, Vector2Ptr>, 2>& uvScaleOffset
            );

            void
            destroy();

            std::array<std::shared_ptr<geometry::Geometry>, 2>
            createDistortionGeometry(std::shared_ptr<render::AbstractContext> context);

            EyeFOV
            getDefaultLeftEyeFov();

            EyeFOV
            getDefaultRightEyeFov();

            void
            updateCameraOrientation(
                std::array<std::shared_ptr<math::Matrix4x4>, 2> viewMatrixes,
                std::shared_ptr<scene::Node> ppScene
            );

            inline static
            Ptr
            create()
            {
                auto ptr = std::shared_ptr<NativeOculus>(new NativeOculus());

                return ptr;
            }

        };
    }
}
