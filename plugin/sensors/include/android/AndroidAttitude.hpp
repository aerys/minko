/*
Copyright (c) 2015 Aerys

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
#include "minko/sensors/AbstractAttitude.hpp"
#include "minko/log/Logger.hpp"

#include <jni.h>

namespace minko
{
    namespace android
    {
        namespace sensors
        {
            class AndroidAttitude : public minko::sensors::AbstractAttitude
            {
            public:
                typedef std::shared_ptr<AndroidAttitude> Ptr;

            private:
                AndroidAttitude();

            public:
                static
                inline
                Ptr
                create()
                {
                    return std::shared_ptr<AndroidAttitude>(new AndroidAttitude());
                }

                void
                initialize();

                void
                startTracking();

                void
                stopTracking();
                
                math::mat4
                rotationMatrix();

                const math::quat&
                quaternion();

                bool
                isSupported();

                inline
                float
                yaw() override
                {
                    // fixme
                    return 0.f;
                }

                inline
                float
                pitch() override
                {
                    // fixme
                    return 0.f;
                }

                inline
                float
                roll() override
                {
                    // fixme
                    return 0.f;
                }

                static math::mat4 rotationMatrixValue;
                static math::quat quaternionValue;
                static std::mutex rotationMatrixMutex;

            private:

                math::mat4
                getRotateEulerMatrix(float x, float y, float z);

                math::mat4 _worldToInertialReferenceFrame;
                math::mat4 _deviceToDisplay;
                
                // JNI part

                // Java Objects
                jobject _attitude = nullptr;

                // Java Method IDs
                jmethodID _startTrackingMethod = nullptr;
                jmethodID _stopTrackingMethod = nullptr;
                jmethodID _isSupportedMethod = nullptr;
            };
        }
    }
}